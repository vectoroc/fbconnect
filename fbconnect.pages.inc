<?php
// $Id: fbconnect.pages.inc,v 1.13 2009/06/03 13:51:06 lanets Exp $

/**
 * Menu callback.
 * Called when user perform facebook registration
 */
function fbconnect_prompt_page() {
  $reg_msg  = t('Click here to create a new account with Facebook connect');
  $link_msg = t('Click here to connect your existing account with Facebook');  
  
  $reg_choice  = l($reg_msg, 'fbconnect/register/create');
  $link_choice = l($link_msg, 'user', array('query' => 'destination=fbconnect'));

  return theme('box', $reg_choice).theme('box', $link_choice);   
}

/**
 * Menu callback.
 * Called when user perform facebook registration
 */
function fbconnect_register_page() {
  $site  = variable_get('site_name', t('this website'));
  $fbuid = fbconnect_get_fbuid();
  $data  = fbconnect_get_info_from_fb($fbuid, 'name, proxied_email');    
  $form  = drupal_retrieve_form('user_register', $form_state);
  
  drupal_prepare_form('user_register', $form, $form_state);   
  
  $form['name']['#default_value'] = $data['name'];
  $form['fb_visible'] = array(
    '#type' => 'checkbox',
    '#title' => t('Let my Facebook friends see me on @sitename', array('@sitename' => $site)),
    '#default_value' => 1,
  );
  
  if (variable_get('user_pictures', 0)) {
    $form['fb_avatar'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use my Facebook picture as user picture'),
      '#description' => t('Your picture will be updated every 24 hours.'),
      '#default_value' => 1,
    );  
  }
  $form['#submit'][] = 'fbconnect_register_form_submit';  
  
  // Fast registration mode, we by pass std drupal reg form.
  if (variable_get('fbconnect_fast_reg', NULL)) {
    $form_state['values']['name']       = $data['name'];
    $form_state['values']['mail']       = $data['proxied_email'];
    $form_state['values']['pass']       = user_password();
    $form_state['values']['status']     = 1;
    $form_state['values']['fb_visible'] = 1;
    $form_state['values']['fb_avatar']  = 1;
    
    drupal_validate_form('user_register', $form, $form_state);
    if (form_get_errors()) {
      $dest = array('query' => 'destination=fbconnect');
      drupal_set_message(t('Facebook connect registration failed for the reasons listed. You may register now, or if you already have an account you can <a href="@login">log in</a> now and link your account', array('@login' => url('user/login', $dest))), 'error');
      drupal_goto('user/register');
    }
    
    fbconnect_register_form_submit($form, $form_state);
    drupal_goto();
  }
    
  return $form;
}

function fbconnect_register_form_submit($form, &$form_state) {
   $fbuid = fbconnect_get_fbuid();
   
   if (variable_get('fbconnect_fast_reg', NULL)) {
     $user = user_save(NULL, $form_state['values']);
   }
   else {
     $user = $form_state['user'];    
   }
   
   if (!$user) {
    drupal_set_message(t("Error saving user account."), 'error');
    drupal_goto();
   }
   
   fbconnect_register($user->uid, $fbuid);
   
   // Save facebook picture as avatar.
   if ($form_state['values']['fb_avatar'] && variable_get('user_pictures', 0)) {     
     if ($picture = fbconnect_get_fb_avatar($fbuid)) {
       $data['picture']   = $picture;
       $data['fb_avatar'] = 1;
     }
   }
   $data['fb_visible'] = $form_state['values']['fb_visible'];
   $data['fbuid']      = $fbuid;
   
   user_save($user, $data);   
   user_external_login($user);
   $_SESSION['fbconnect_feed']['type'] = 'register';
}

/**
 * Menu callback fbconnect identities
 */
function fbconnect_user_identities($user) {
  $fbuid = fbconnect_get_fbuid();
  $uid   = _is_fbconnect_user($fbuid);

  if ($user->uid == $uid) { 
    return drupal_get_form('fbconnect_user_settings_form');
  }
  else {
    return t('Your account is not linked with a Facebook account or you are not authorized to edit the Facebook connect settings of this account.');
  } 
}

/**
 *  This form allows the user to manage their facebook connect settings.
 *
 *  Some options are visible only if facebook session is active.
 */
function fbconnect_user_settings_form() {
  $user = user_load(arg(1));  
  // These fields manage the visibility of the user, 
  // if the option is enabled, user's facebook friends 
  // will be able to see his presence on this site
  $form['fb_visible'] = array(
    '#type' => 'checkbox',
    '#title' => t('Let my Facebook friends see me on @site', array('@site' => variable_get('site_name', 'this website'))),
    '#description' => t('My Facebook friends will be able to see that I own an account on this website.'),
    '#default_value' => $user->fb_visible,
  );

  // Whether user picture is enabled, 
  // users can choose to use his facebook avatar as avatar
  if (variable_get('user_pictures', 0)) {
    $form['fb_avatar'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use my Facebook picture as user picture'),
      '#description' => t('Your picture will be updated every 24 hours.'),
      '#default_value' => $user->fb_avatar,
    );
  
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
  );
  return $form;
}

/**
 * Handle post-validation user_settingsForm submission. 
 */
function fbconnect_user_settings_form_submit($form, &$form_state) {
  $user    = user_load(arg(1)); 
  $fbuid   = fbconnect_get_fbuid(); 
  $visible = $form_state['values']['visibility'];
  $avatar  = $form_state['values']['fb_avatar'];

  $data['fb_visible'] = $form_state['values']['fb_visible'];
  $data['fb_avatar']  = $form_state['values']['fb_avatar'];
  
  if ($data['fb_avatar'] && ($user->fbuid == $fbuid)) {
    db_query("UPDATE {users} SET picture = '%s' WHERE uid = %d", fbconnect_get_fb_avatar($fbuid), $user->uid);    
  }
  
  if ($data) user_save($user, $data);
  drupal_set_message('Your configuration options have been saved.');    
}

/**
 * Render the facebook friends invite form.
 */
function fbconnect_render_friends_invite_form() {
  global $base_url;
  $site      = drupal_to_js(variable_get('site_name', $base_url));  
  $actiontxt = drupal_to_js(t('Please select the Facebook friends you want to invite'));
  $action    = drupal_to_js(variable_get('fbconnect_invite_dest', $base_url));
  $type      = drupal_to_js(variable_get('fbconnect_invite_name', $site));
  $content   = check_plain(variable_get('fbconnect_invite_msg', t('Enjoy'))) .'  <fb:req-choice url=\''. $base_url .'\' label=\'Become a Member!\' />';
  $output    = '<fb:serverfbml>
                <script type="text/fbml">
                  <fb:fbml>
                    <fb:request-form
                      action='. $action .'
                      method="POST"
                      invite="true"
                      type='. $type .'
                      content="'. $content .'">	
                      <fb:multi-friend-selector
                      cols="6"
                      rows="6"
                      showborder="false"
                      actiontext='. $actiontxt .'
                      max="35"
                      bypass="cancel" />
                    </fb:request-form>
                  </fb:fbml>
                </script>
              </fb:serverfbml>';
  return $output;
}