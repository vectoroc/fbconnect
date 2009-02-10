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
 if (($fbuid = fbconnect_get_fbuid()) && !user_is_logged_in()) {   
   switch (arg(2)) {
     case 'import':
       drupal_set_title(t('Import your profile information from Facebook'));  
       $output = fbconnect_render_avatar($fbuid);
       $output .= drupal_get_form('fbconnect_register_form');
     break;
     case 'create':
       drupal_set_title(t('Create a new account'));
       $output  = drupal_get_form('fbconnect_register_form');
       if ($friends = fbconnect_get_connected_friends($fbuid)) {
        $output .= t('Facebook friend already registered');
        $output .= theme('render_friends_list_fbconnect', $friends);
       }
     break;
   }    
   return $output;
 }
}

 /**
  * This form is display when we register a new user with facebook account
  */
function fbconnect_register_form() {
 $fbuid = fbconnect_get_fbuid();
 switch (arg(2)) {
   case 'import':
    // display the form if the facebook importation settings is activated
    if (variable_get('fbconnect_import', array('fbconnect_import'))) {
      $form['imp'] = array(
        '#type' => 'fieldset',
        '#collapsible' => FALSE,
      );       
      $form['imp']['field_import'] = array(
        '#type' => 'checkboxes',
        '#options' => fbconnect_available_import_fields($fbuid),
        '#default_value' => variable_get('fbconnect_field_to_import', NULL),
        '#description' => t('Select the public Facebook profile information you wish to import.
        This information will be displayed on your public profile.'),
      );
    }
   break;
   case 'create':
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
          array('@sitename' => variable_get('site_name', 'this website'))),
       '#description' => t('My Facebook friends will be able to see that I own an account on @sitename.',
        array('@sitename' => variable_get('site_name', t('this website')))),
       '#default_value' => 1,
     );
   break;
 }
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
 switch (arg(2)) {
   case 'import':
     if ($fields = array_filter((array)$form_state['values']['field_import'], 'fbconnect_import_filter')) {
      $_SESSION['fb_reg_import'] = $fields;
     }
   break;
   case 'create':
     // Validate the username
     $name = $form_state['values']['username'];
     if (user_validate_name($name)) {
       form_set_error('username', user_validate_name($name));
     }
     else if (db_result(db_query("SELECT COUNT(*) FROM {users} WHERE LOWER(name) = LOWER('%s')", $name)) > 0) {
       form_set_error('username', t('The name %name is already taken.',
        array('%name' => $name)));
     }
     else if (drupal_is_denied('user', $name)) {
       form_set_error('username', t('The name %name is a reserved username.',
        array('%name' => $name)));
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
       form_set_error('mail', t('The e-mail address %email has been denied access.',
        array('%email' => $mail)));
     }
   break;
 }
} 

/** 
 * Handle post-validation register form submission. 
 */ 
function fbconnect_register_form_submit($form, &$form_state) {
 switch (arg(2)) {
   case 'import':
   $form_state['redirect'] = 'fbconnect/register/create';
   return;

   case 'create':            
   // If sucess, user_external_login_register will return NULL
   $register_result = user_external_login_register($form_state['values']['username'], 'fbconnect');
   // In case of register false
   if ($register_result != NULL) {
     watchdog('fbconnect', 'User external register failed for %user.', array('%user' => $form_state['values']['username']));
     drupal_set_message(t('Register error'), 'error');
     $form_state['redirect'] = '';
     return;
   }
   global $user;
   $fbuid = fbconnect_get_fbuid();
   fbconnect_register($user->uid, $fbuid);
   // Update user visibility, default = 1
   if ($form_state['values']['visibility'] == 0) {
    fbconnect_set_user_visibility($user, $form_state['values']['visibility']);
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
   if ($_SESSION['fb_reg_import']) {
     fbconnect_insert_user_info($user, $_SESSION['fb_reg_import']);
   }
   if (variable_get('fbconnect_reg_feed', 1) && $form_state['values']['visibility']) {
     $_SESSION['fbconnect_feed']['type'] = 'registration';
   }
   if ($form_state['values']['visibility']) {
     $form_state['redirect'] = 'fbconnect/invite/friends';
     drupal_set_message(t('Registration successful. You may now invite friends to join you on @sitename', 
      array('@sitename' => variable_get('site_name', t('this website')))), 'status');
   }
   else {
     drupal_set_message(t('Registration successful.'));  
     $form_state['redirect'] = '';        
   }
   return;
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
       
    if ((fbconnect_get_fbuid() > 0) && $account->fbuid != fbconnect_get_fbuid()) {
      drupal_set_message('Invalid Facebook session, you\'re not authorized to modify the parameters.');
      return theme('table', $header, $rows);
    }    
    if (!fbconnect_get_fbuid()) {
      $rows[] = array(t('Open a Facebook session for more settings options.'));
      $rows[] = array(fbconnect_render_button());
    }
    $output = theme('table', $header, $rows);
    $output .= drupal_get_form('fbconnect_user_settingsForm');
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
  $output .= drupal_get_form('fbconnect_associationForm');  
  return $output;
}

/**
 * Render the user fbconnect association form
 */
function fbconnect_associationForm() {
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
 * Render the user fbconnect setting form
 */
function fbconnect_user_settingsForm() {
  $user = user_load(array('uid' => arg(1)));
  $form['view'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
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
  if (fbconnect_get_fbuid() > 0) {
    // If user avatar is actived
    // The user can choose to use his Facebook avatar as drupal avatar.
    if (variable_get('user_pictures', 0)) {
      $form['avatar'] = array(
        '#type' => 'fieldset',
        '#collapsible' => FALSE,
      );
      $form['avatar']['fb_avatar'] = array(
        '#type' => 'checkbox',
        '#title' => t('Use my Facebook picture on @sitename', 
          array('@sitename' => variable_get('sitename', t('this website')))),
        '#description' => t('Your picture will be updated every 24 hours.'),
        '#default_value' => fbconnect_user_avatar_setting($user),
      );
    }
    // Importation settings
    $fields = array_filter(variable_get('fbconnect_field_to_import', ''), 'fbconnect_import_filter');
    if (variable_get('fbconnect_import', array('fbconnect_import')) && $fields) {
      $form['import'] = array(
        '#type' => 'fieldset',
        '#collapsible' => FALSE,
      );
      $default_value = fbconnect_get_user_import_setting($user);    
      $form['import']['import_setting'] = array(
        '#type' => 'checkboxes',
        '#description' => t('This information is displayed on my profile.'),
        '#options' => fbconnect_available_import_fields(fbconnect_get_fbuid()),
        '#default_value' => ($default_value) ? $default_value : NULL,
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
function fbconnect_user_settingsForm_submit($form, &$form_state) {
  $user = user_load(array('uid' => arg(1)));  
  // Visibility settings
  if ($form_state['values']['visibility_hid'] != $form_state['values']['visibility']) {
    fbconnect_set_user_visibility($user, $form_state['values']['visibility']);
  }
  if (fbconnect_get_fbuid() == 0) {
    return;
  }
  if (variable_get('user_pictures', 0)) {  
    fbconnect_user_avatar_setting($user, $form_state['values']['fb_avatar']);        
    if ($form_state['values']['fb_avatar'] == 1) {
      fbconnect_refresh_user_avatar($user->fbuid, $user->uid);
    }
    else {
      if ($user->picture) {
        file_delete($user->picture);
        db_query("UPDATE {users} SET picture = '' WHERE uid = %d", $user->uid);
      }
    }
  }
  if (variable_get('fbconnect_import', array('fbconnect_import'))) {
    if ($fields = array_filter((array)$form_state['values']['import_setting'], 'fbconnect_import_filter')) {
      fbconnect_insert_user_info($user, $fields);
    }
    else {
      db_query("UPDATE {fbconnect_users} SET import_setting = '' WHERE uid = %d", $user->uid);
      db_query('DELETE FROM {fbconnect_profile} WHERE uid = %d', $user->uid);
    }
  }
}

/** 
 * Handle post-validation association_Form submission. 
 */ 
function fbconnect_associationForm_submit($form, &$form_state) {
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