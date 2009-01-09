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
   if (fbconnect_get_fbuid() && !user_is_logged_in()) {
     $fbuid = fbconnect_get_fbuid();
     switch (arg(2)) {
       case 'step1':
         drupal_set_title(t('Import your profile information from Facebook'));  
         $output = fbconnect_render_avatar($fbuid);
         $output .= drupal_get_form('fbconnect_registerForm');
       break;
       case 'step2':
         drupal_set_title(t('Create a new account'));
         $output  = drupal_get_form('fbconnect_registerForm');
         $output .= theme('fb_connected_friends', fbconnect_get_connected_friends($fbuid));
       break;
     }    
     return $output;
   }
 }

 /**
  * This form is display when we register a new user with facebook account
  */
 function fbconnect_registerForm() {
   $fbuid = fbconnect_get_fbuid();
   switch (arg(2)) {
     case 'step1':
       $form['import'] = array(
         '#type' => 'fieldset',
         '#title' => t('Import your information from Facebook'),
         '#collapsible' => TRUE,
         '#collapsed' => FALSE,
       );       
       $form['import']['field_to_import'] = array(
         '#type' => 'checkboxes',
         '#options' => fbconnect_available_import_fields($fbuid),
         '#default_value' => variable_get('fbconnect_field_to_import', ''),
         '#description' => t('Select the fields you wish to import'),
       );      
     break;
     case 'step2':
       $form['register'] = array(
         '#type' => 'fieldset',
         '#title' => t('Create a new account'),
         '#collapsible' => TRUE,
         '#collapsed' => FALSE,
       );
       $form['register']['fbconnect_username'] = array(
         '#type' => 'textfield',
         '#title' => t('Username'),
         '#required' => TRUE,
         '#default_value' => fbconnect_get_facebook_username($fbuid),
         '#description' => t('Spaces are allowed; punctuation is not allowed except for
         periods, hyphens, and underscores.'),
       );
       $form['register']['fbconnect_mail'] = array(
         '#type' => 'textfield',
         '#title' => t('E-mail address'),
         '#required' => TRUE,
         '#description' => t('A valid e-mail address. All e-mails from the system will be sent to this address.')
       );
       $form['register']['fbconnect_visibility'] = array(
         '#type' => 'checkbox',
         '#title' => t('Visiblity'),
         '#description' => t('Visiblity setting'),
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
 function fbconnect_registerForm_validate($form, &$form_state) {
   switch (arg(2)) {
     case 'step1':
       $_SESSION['fb_reg_import'] = $form_state['values']['field_to_import'];
     break;
     case 'step2':
       // Validate the username
       $name = $form_state['values']['fbconnect_username'];
       if (user_validate_name($name)) {
         form_set_error('fbconnect_username', user_validate_name($name));
       }
       else if (db_result(db_query("SELECT COUNT(*) FROM {users} WHERE LOWER(name) = LOWER('%s')", $name)) > 0) {
         form_set_error('fbconnect_username', t('The name %name is already taken.',
          array('%name' => $name)));
       }
       else if (drupal_is_denied('user', $name)) {
         form_set_error('fbconnect_username', t('The name %name is a reserved username.',
          array('%name' => $name)));
       }
       // Validate e-mail
       $mail = $form_state['values']['fbconnect_mail'];
       if (user_validate_mail($mail)) {
         form_set_error('fbconnect_mail', user_validate_mail($mail));
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
 function fbconnect_registerForm_submit($form, &$form_state) {
   switch (arg(2)) {
     case 'step1':
     $form_state['redirect'] = 'fbconnect/register/step2';
     return;

     case 'step2':            
     // If sucess, user_external_login_register will return NULL
     $register_result = user_external_login_register($form_state['values']['fbconnect_username'], 'fbconnect');

     // In case of register false
     if ($register_result != NULL) {
       watchdog('fbconnect', 'User external register failed for %user.', array('%user' => $form_state['values']['fbconnect_username']));
       drupal_set_message(t('Register error'), 'error');
       $form_state['redirect'] = '';
       return;
     }
     global $user;
     $fbuid = fbconnect_get_fbuid();
     // Insert new user in fbconnect_users table.
     fbconnect_register($user->uid, $fbuid);
     // Update user visibility, default = 1
     if ($form_state['values']['fbconnect_visibility'] == 0) {
      fbconnect_set_user_visibility($user, $form_state['values']['fbconnect_visibility']);
     }
     // Store e-mail
     $extra_data['mail'] = $extra_data['init'] = $form_state['values']['fbconnect_mail'];
     // Save facebook picture as avatar.
     if (variable_get('user_pictures', 0)) {
       $picture = fbconnect_get_fb_avatar($fbuid);
       if ($picture) {
         $extra_data['picture'] = $picture;      
       }
     }
     user_save($user, $extra_data);
     // An array of fields to query
     if ($_SESSION['fb_reg_import']) {
       $fields = array_filter($_SESSION['fb_reg_import'], 'fbconnect_import_filter');
       fbconnect_insert_user_info($user, $fields);
     }
     // Registration complete message
     drupal_set_message(t('Registration successful, tell your friend about this website'), 'status');
     if (variable_get('fbconnect_registered_feed', 1) && $form_state['values']['fbconnect_visibility']) {
       $_SESSION['fbconnect_feed_prompt']['type'] = 'registration';
     }
     // redirect to invite friends page
     $redirect_path = ($form_state['values']['fbconnect_visibility']) ? 'fbconnect/invite/friends' : '';
     $form_state['redirect'] = $redirect_path;
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
  $rows[] = array(t('This site supports Facebook Connect.'));  
  // utilisteur donc le compte est déja associé avec un compte facebook
  if ($account->fbuid) {    
    $rows[] = array(t('Your account is linked with your Facebook account'));
    $rows[] = array(t('Facebook UID : @fbuid', array('@fbuid' => $account->fbuid))); 
       
    // Si la session facebook courant ne correpond pas à l'utilisateur courant.
    if ((fbconnect_get_fbuid() > 0) && $account->fbuid != fbconnect_get_fbuid()) {
      drupal_set_message('Votre session Facebook ne correspond pas a l\' utilisateur courant, vous ne pouvez pas changer les parametres.', 'warning');
      return theme('table', $header, $rows);
    }    
    // Pas de session facebook ouverte, on propose d'ouvrir une session
    // pour avoir une mise a jours des options d'importation.
    if (!fbconnect_get_fbuid()) {
      $rows[] = array(t('Open a Facebook session for more settings options.'));
      $rows[] = array(fbconnect_render_button('medium'));
    }    
    $output = theme('table', $header, $rows);
    $output .= drupal_get_form('fbconnect_user_settingsForm');
    return $output;
  }  
  // Pas de session facebook detecté, on affiche le boutton facebook connect.
  if (!fbconnect_get_fbuid()) {
    $rows[] = array(t('Pas de Facebook session detecté, cliquez sur le boutton ci dessous
    pour vous identifier et linker votre compte avec votre compte facebook'));
    $rows[] = array(fbconnect_render_button('small'));
    return theme('table', $header, $rows);
  } 
  // Session facebook detecté, on propose l'association du compte.
  $rows[] = array(t('Facebook session detecté, completé le formulaire
  pour associer votre compte à votre compte Facebook'));
  $output = theme('table', $header, $rows);
  $output .= drupal_get_form('fbconnect_associationForm');
  
  return $output;
}

/**
 * Render the user fbconnect association form
 * Ce formulaire permet d'associer un compte drupal à un facebook uid
 */
function fbconnect_associationForm() {
  $form['fbconnect_association'] = array(
    '#type' => 'checkbox',
    '#title' => t('Associer ce compte avec mon compte Facebook'),
    '#default_value' => 'fbconnect_association',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add Facebook Connect link'),
  );
  return $form;
}

/**
 * Render the user fbconnect setting form
 * Ce formulaire determine les parametres d'importation du compte.
 */
function fbconnect_user_settingsForm() {
  // arg(1) contient le user uid
  $user = user_load(array('uid' => arg(1)));
      
  // Si les avatars sont activé
  // l'utilisateur peut choisir d'utiliser son avatar facebook
  // l'avatar est reimporté tous les 24heures, par le cron.
  if (variable_get('user_pictures', 0)) {
    $avatar_default_value = fbconnect_get_user_avatar_setting($user);
  
    $form['avatar'] = array(
      '#type' => 'fieldset',
      '#title' => t('Avatar settings'),
      '#collapsible' => FALSE,
    );
    // Ce champ permet de connaitre la valeur avant modification.
    $form['avatar']['fbconnect_avatar_settings_hidden'] = array(
      '#type' => 'hidden',
      '#default_value' => $avatar_default_value,
    );
    $form['avatar']['fbconnect_avatar_settings'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use my facebook avatar as avatar'),
      '#default_value' => $avatar_default_value,
    );
  }

  $form['visibility'] = array(
    '#type' => 'fieldset',
    '#title' => t('Visibility settings'),
    '#collapsible' => FALSE,
  );
  $form['avatar']['fbconnect_visibility_settings_hidden'] = array(
    '#type' => 'hidden',
    '#default_value' => $user->fb_visibility,
  );
  $form['visibility']['fbconnect_visibility_settings'] = array(
    '#type' => 'checkbox',
    '#title' => t('Visibility'),
    '#default_value' => $user->fb_visibility,
  );
  
  // Si une session facebook est ouverte, l'utilisateur 
  // peut changer ses parametres d'importation.
  if (fbconnect_get_fbuid() > 0) {
    // Valeur précédemment enregistrer pour l'utilisateur courant.
    $default_value = fbconnect_get_user_import_setting($user);    
    $form['import'] = array(
      '#type' => 'fieldset',
      '#title' => t('Profile information settings'),
      '#collapsible' => FALSE,
    );
    $form['import']['fbconnect_user_import_settings'] = array(
      '#type' => 'checkboxes',
      '#options' => fbconnect_available_import_fields(fbconnect_get_fbuid()),
      '#default_value' => ($default_value) ? $default_value : variable_get('fbconnect_field_to_import', ''),
    );
  }
  
  if ($form) {  
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save settings'),
    );
    return $form;
  }
}

/**
 * Handle post-validation user_settingsForm submission. 
 */
function fbconnect_user_settingsForm_submit($form, &$form_state) {
  $user = user_load(array('uid' => arg(1)));
  // Avatar settings
  if ($form_state['values']['fbconnect_avatar_settings_hidden'] != $form_state['values']['fbconnect_avatar_settings']) {
    fbconnect_get_user_avatar_setting($user, $form_state['values']['fbconnect_avatar_settings']);
    
    if ($form_state['values']['fbconnect_avatar_settings'] == 1) {
      db_query("UPDATE {users} SET picture = '%s' WHERE uid = %d", fbconnect_get_fb_avatar($user->fbuid), $user->uid);
    }
  }
  // Visibility settings
  if ($form_state['values']['fbconnect_visibility_settings_hidden'] != $form_state['values']['fbconnect_visibility_settings']) {
    fbconnect_set_user_visibility($user, $form_state['values']['fbconnect_visibility_settings']);
  }
  // Filtrage des champs vide.  
  $fields = array_filter($form_state['values']['fbconnect_user_import_settings'], 'fbconnect_import_filter');
  // On efface les precedents donnée importé
  db_query('DELETE FROM {fbconnect_profile} WHERE uid = %d', $user->uid); 
  // On enregistre les parametres d'importation, et reimportation des données
  fbconnect_insert_user_info($user, $fields);
}

/** 
 * Handle post-validation association_Form submission. 
 */ 
function fbconnect_associationForm_submit($form, &$form_state) {
  if (fbconnect_get_fbuid() > 0) {
    $fbuid = fbconnect_get_fbuid();
    // Case, association cocher
    if ($form_state['values']['fbconnect_association'] == 1) {
      // arg(1) contient le user uid
      $user = user_load(array('uid' => arg(1)));    
      // Update de la table fbconnect_users, on associe le fbuid courant au drupal uid courant.
      $result = fbconnect_register($user->uid, $fbuid);
      if (!$result) {
        drupal_set_message(t('Association failed'), 'error');
        watchdog('fbconnect', 'User association failed for uid: %user - fbuid: %fbuid.', array('%user' => $user->uid, '%fbuid' => $fbuid));
      }else{      
        drupal_set_message(t('Your account is now associated with your Facebook account'), 'status');
        watchdog('fbconnect', 'User uid: %user is now associated with the fbuid: %fbuid.', array('%user' => $user->uid, '%fbuid' => $fbuid));
      }
    }
  }
  return;
}