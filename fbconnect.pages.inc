<?php
// $Id$

/**
 * @file
 * User page callbacks for the fbconnect module.
 */
 
/**
 * Menu callback.
 * Called when user perform facebook registration
 */
function fbconnect_register_page() {
  if ($fbuid = fbconnect_get_fbuid()) {   
    drupal_set_title(t('Create a new account'));
    $output  = drupal_get_form('fbconnect_register_form');

    if ($friends = fbconnect_get_connected_friends($fbuid)) {
      $output .= t('Facebook friend already registered');
      $output .= theme('render_friends_list_fbconnect', $friends);
    }   
    return $output;
  }
}

 /**
  * This form is display when we register a new user.
  */
function fbconnect_register_form() {
 $fbuid = fbconnect_get_fbuid();
 $form['reg'] = array(
   '#type' => 'fieldset',
   '#collapsible' => FALSE,
 );
 $form['reg']['username'] = array(
   '#type' => 'textfield',
   '#title' => t('Username'),
   '#required' => TRUE,
   '#default_value' => fbconnect_get_fbname($fbuid),
   '#description' => t('Spaces are allowed; punctuation is not allowed except for periods, hyphens, and underscores.'),
 );
 $form['reg']['mail'] = array(
   '#type' => 'textfield',
   '#title' => t('E-mail address'),
   '#required' => TRUE,
   '#description' => t('A valid e-mail address. All e-mails from the system will be sent to this address.')
 );
 $form['reg']['visibility'] = array(
   '#type' => 'checkbox',
   '#title' => t('Let my Facebook friends see me on @sitename', 
      array('@sitename' => variable_get('site_name', t('this website')))),
   '#description' => t('My Facebook friends will be able to see that I own an account on @sitename.',
      array('@sitename' => variable_get('site_name', t('this website')))),
   '#default_value' => 1,
 );
 $form['submit'] = array( 
 '#type' => 'submit',
 '#value' => t('Submit') 
 );
 
 return $form; 
}

/** 
 * Validate the register form. 
 */ 
function fbconnect_register_form_validate($form, &$form_state) {
  // Validate the username
  $name = $form_state['values']['username'];
  if (user_validate_name($name)) {
   form_set_error('username', user_validate_name($name));
  }
  else if (db_result(db_query("SELECT COUNT(*) FROM {users} WHERE LOWER(name) = LOWER('%s')", $name)) > 0) {
   form_set_error('username', t('The name %name is already taken.', array('%name' => $name)));
  }
  else if (drupal_is_denied('user', $name)) {
   form_set_error('username', t('The name %name is a reserved username.', array('%name' => $name)));
  }
  // Validate e-mail
  $mail = $form_state['values']['mail'];
  if (user_validate_mail($mail)) {
   form_set_error('mail', user_validate_mail($mail));
  }
  else if (db_result(db_query("SELECT COUNT(*) FROM {users} WHERE LOWER(mail) = LOWER('%s')", $mail)) > 0) {
   form_set_error('mail', t('The e-mail address %email is already registered. <a href="@password">Have you forgotten your password?</a>',
    array('%email' => $mail, '@password' => url('user/password'))));
  }
  else if (drupal_is_denied('mail', $mail)) {
   form_set_error('mail', t('The e-mail address %email has been denied access.', array('%email' => $mail)));
  }
}

/** 
 * Handle post-validation register form submission. 
 */ 
function fbconnect_register_form_submit($form, &$form_state) {
  $visible = $form_state['values']['visibility'];
  $name    = $form_state['values']['username'];

  // If sucess, this function will return NULL
  $register_result = user_external_login_register($name, 'fbconnect');
  // In case of registration failed.
  if ($register_result != NULL) {
   watchdog('fbconnect', 'User external register failed for %user.', array('%user' => $name));
   drupal_set_message(t('Register error'), 'error');
   $form_state['redirect'] = '';
   return;
  }
   
  global $user;
  $fbuid = fbconnect_get_fbuid();
  fbconnect_register($user->uid, $fbuid);   
  // Update user visibility, default = 1
  if (!$visible) {
    fbconnect_set_user_visibility($user, $visible);
  }

  $extra_data['mail'] = $extra_data['init'] = $form_state['values']['mail'];
  $extra_data['login'] = time();   
  // Save facebook picture as avatar.
  if (variable_get('user_pictures', 0)) {
    $picture = fbconnect_get_fb_avatar($fbuid);
    if ($picture) {
      $extra_data['picture'] = $picture;
    }
  }
  user_save($user, $extra_data);

  if ($_SESSION['fb_reg_import'] && module_exists('fbconnect_profile')) {
    // Saving import settings.
    fbconnect_profile_insert_user_info($user, $_SESSION['fb_reg_import']);
    unset($_SESSION['fb_reg_import']);
  }

  // If the user chose to be visible, and the registration feed parameter is enabled.
  // We store a value in the session variable, 
  // this variable will be read by the _fbconnect_render_js function.
  if (variable_get('fbconnect_reg_feed', 1) && $visible) {
    $_SESSION['fbconnect_feed']['type'] = 'registration';
  }
  // The user has chosen to be visible, just redirect to the friends invite page.
  if ($visible) {
    $form_state['redirect'] = 'fbconnect/invite/friends';
    $sitename = variable_get('site_name', t('this website'));
    $welcome_msg = t('Registration successful. You may now invite friends to join you on @site',  array('@site' => $sitename));
    drupal_set_message($welcome_msg, 'status');
  }
  else {
    drupal_set_message(t('Registration successful.'));  
    $form_state['redirect'] = '';        
  }
}

/**
 * Menu callback fbconnect identities
 */
function fbconnect_user_identities($account) {
  drupal_set_title(check_plain($account->name));
  $header = array(t('Facebook Connect identities'));
  
  $rows = array();  
  $rows[] = array(t('This website supports Facebook Connect and allow you to log in with your Facebook account.'));  
  
  if ($account->fbuid) {    
    $rows[] = array(t('Your account is linked with your Facebook account.'));
    $rows[] = array(t('Your Facebook user id : @fbuid', array('@fbuid' => $account->fbuid))); 
       
    if (fbconnect_get_fbuid() && $account->fbuid != fbconnect_get_fbuid()) {
      drupal_set_message('Invalid Facebook session, you\'re not authorized to modify the parameters.');
      return theme('table', $header, $rows);
    }    
    if (!fbconnect_get_fbuid()) {
      $rows[] = array(t('Open a Facebook session for more settings options.'));
      $rows[] = array(fbconnect_render_button());
    }    
    $output = theme('table', $header, $rows);
    $output .= drupal_get_form('fbconnect_user_settings_form');
    return $output;
  }
    
  if (!fbconnect_get_fbuid()) {
    $rows[] = array(t('Click on login button below to link your account on @sitename with your Facebook account.', 
      array('@sitename' => variable_get('site_name', t('this website')))));
    $rows[] = array(fbconnect_render_button());
    return theme('table', $header, $rows);
  }
  
  $rows[] = array(t('Facebook session detected, please confirm that you wish to link your @sitename account and Facebook account.',
   array('@sitename' => variable_get('site_name', t('this website')))));
  $output = theme('table', $header, $rows);
  $output .= drupal_get_form('fbconnect_association_form');  
  return $output;
}

/**
 * Render the user fbconnect association form
 */
function fbconnect_association_form() {
  $form['link'] = array(
    '#type' => 'checkbox',
    '#title' => t('Link my account on @sitename and my Facebook account', 
      array('@sitename' => variable_get('sitename', t('this website')))),
    '#default_value' => 1,
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

/**
 *  This form allows the user to manage their facebook connect settings.
 *
 *  Some options are visible only if facebook session is active.
 */
function fbconnect_user_settings_form() {
  $user = user_load(array('uid' => arg(1)));  
  // These fields manage the visibility of the user, 
  // if the option is enabled, user's facebook friends 
  // will be able to see his presence on this site
  $form['view'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#weight' => -2,
  );  
  $form['visibility_hid'] = array(
    '#type' => 'hidden',
    '#default_value' => $user->fb_visibility,
  );  
  $form['view']['visibility'] = array(
    '#type' => 'checkbox',
    '#title' => t('Let my Facebook friends see me on @sitename', 
    array('@sitename' => variable_get('site_name', 'this website'))),
    '#description' => t('My Facebook friends will be able to see that I own an account on this website.'),
    '#default_value' => $user->fb_visibility,
  );

  // This fields is visible only if we got a active Facebook session.
  // Whether user picture is enabled, 
  // users can choose to use his facebook avatar as avatar
  if (fbconnect_get_fbuid()) {
    if (variable_get('user_pictures', 0)) {
      $form['avatar'] = array(
        '#type' => 'fieldset',
        '#collapsible' => FALSE,
        '#weight' => -3,
      );
      $form['avatar']['fb_avatar'] = array(
        '#type' => 'checkbox',
        '#title' => t('Use my Facebook picture on @sitename', 
          array('@sitename' => variable_get('sitename', t('this website')))),
        '#description' => t('Your picture will be updated every 24 hours.'),
        '#default_value' => fbconnect_user_avatar_setting($user),
      );
    }
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
  $user = user_load(array('uid' => arg(1)));  

  if ($form_state['values']['visibility_hid'] != $form_state['values']['visibility']) {
    fbconnect_set_user_visibility($user, $form_state['values']['visibility']);
  }  
  if (!fbconnect_get_fbuid()) {
    return;
  }
  
  if (variable_get('user_pictures', 0)) {
    fbconnect_user_avatar_setting($user, $form_state['values']['fb_avatar']);        
    if ($form_state['values']['fb_avatar'] == 1) {
      fbconnect_refresh_user_avatar($user->fbuid, $user->uid);
    }
    else {
      $avatar_setting = fbconnect_user_avatar_setting($user);
      if ($avatar_setting && $user->picture) {
        file_delete($user->picture);
        db_query("UPDATE {users} SET picture = '' WHERE uid = %d", $user->uid);
        db_query("UPDATE {fbconnect_users} SET avatar = %d WHERE uid = %d", 0, $user->uid);
      }
    }
  }
}

/** 
 * Handle post-validation association_Form submission.
 *
 * When the user submits this form, an association of the drupal uid
 * and the facebook fbuid is inserted into the table fbconnect_users.
 */ 
function fbconnect_association_form_submit($form, &$form_state) {
  if ($fbuid = fbconnect_get_fbuid()) {
    if ($form_state['values']['link'] == 1) {
      $user = user_load(array('uid' => arg(1)));
      
      if (fbconnect_register($user->uid, $fbuid)) {
        drupal_set_message(t('Congratulations! Your account is now linked to your Facebook account'), 'status');
        watchdog('fbconnect', 'User uid: %user is now associated with the fbuid: %fbuid.', 
        array('%user' => $user->uid, '%fbuid' => $fbuid));        
      }
      else {
        drupal_set_message(t('Association failed'), 'error');
        watchdog('fbconnect', 'User association failed for uid: %user - fbuid: %fbuid.', 
        array('%user' => $user->uid, '%fbuid' => $fbuid));        
      }
    }
  }
}