<?php
// $Id: fbconnect.admin.inc,v 1.10 2009/06/03 13:51:06 lanets Exp $

/** 
 * @file 
 * Administration page callbacks for the fbconnect module. 
 */ 

/** 
 * Form builder. Configure fbconnect. 
 *
 * @see system_settings_form(). 
 */
function fbconnect_admin_settings() {
  global $base_url;
  $module_path = drupal_get_path('module', 'fbconnect') . '/images/';
   
  $form['global'] = array(
    '#type' => 'fieldset',
    '#title' => t('Facebook API Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['global']['fbconnect_key'] = array( 
    '#type' => 'textfield', 
    '#title' => t('Facebook key'), 
    '#default_value' => variable_get('fbconnect_key', NULL),
    '#description' => t('Your public API key'),
  );
  
  $form['global']['fbconnect_skey'] = array(
    '#type' => 'textfield',
    '#title' => t('Facebook Secret key'),
    '#default_value' => variable_get('fbconnect_skey', NULL),
    '#description' => t('Do not share your secret key with anyone'),
  );
  
  $form['global']['fbconnect_base_domaine'] = array(
    '#type' => 'textfield',
    '#title' => t('Base domain url'),
    '#description' => t('Use this to enable your Facebook Connect implementation to span multiple subdomains (e.g., using example.com would enable  www.example.com, foo.example.com or bar.example.com,).'),
    '#default_value' => variable_get('fbconnect_base_domaine', NULL),
  );
  
  $form['global']['fbconnect_connect_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Connect url'),
    '#description' => t('Your site\'s main URL'),
    '#default_value' => variable_get('fbconnect_connect_url', $base_url),
  );
  
  $form['global']['fbconnect_language_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Language code'),
    '#description' => t('Enter your country code here to get translated versions of facebook connect. (e.g. en_US or de_DE)'),
    '#default_value' => variable_get('fbconnect_language_code', 'en_US'),
  );
  
  $form['global']['fbconnect_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Debug mode'),      
    '#default_value' => variable_get('fbconnect_debug', FALSE),
  );  
  
  $form['site'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings for !site', array('!site' => variable_get('site_name', t('Your Site')))),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['site']['fbconnect_fast_reg'] = array(
    '#type' => 'checkbox',
    '#title' => t('Fast registration mode'),
    '#default_value' => variable_get('fbconnect_fast_reg', NULL),
    '#description' => t('Unable this mode will bypass drupal\'s standard registration form.'),
  );
  
  $form['site']['fbconnect_loginout_mode'] = array(
    '#type' => 'select',
    '#title' => t('Login/logout mode'),
    '#options' => array(
      'manual' => t('Manual'),
      'ask'    => t('Ask user to logout from FB'),
      'auto'   => t('Auto login/logout'),
    ),
    '#default_value' => variable_get('fbconnect_loginout_mode', FALSE),
    '#description' => t('Login to drupal/logout from facebook. Enable this if you don\'t want to logout automatically from facebook.'),
  );
  
  $form['site']['fbconnect_invite_msg'] = array(
    '#type' => 'textfield',
    '#title' => t('Invite message'),      
    '#default_value' => variable_get('fbconnect_invite_msg', t('Enjoy facebook connect')),
  );
  
  $form['site']['fbconnect_invite_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Your site name'),      
    '#default_value' => variable_get('fbconnect_invite_name', variable_get('site_name', NULL)),
  );
  
  $form['site']['fbconnect_invite_dest'] = array(
    '#type' => 'textfield',
    '#title' => t('Destination, after user\'s valid or skip invite friend form'),
    '#default_value' => variable_get('fbconnect_invite_dest', $base_url),
  );

  
  $form['display'] = array(
    '#title' => t('Display Settings'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  
  if (fbconnect_get_config()) {  
    $button = array(
      'small_short'  => fbconnect_render_button(array('size' => 'small' , 'length' => 'short', 'onclick' => 'return false;')),
      'medium_short' => fbconnect_render_button(array('size' => 'medium', 'length' => 'short', 'onclick' => 'return false;')),
      'medium_long'  => fbconnect_render_button(array('size' => 'medium', 'length' => 'long', 'onclick' => 'return false;')),
      'large_short'  => fbconnect_render_button(array('size' => 'large' , 'length' => 'short', 'onclick' => 'return false;')),
      'large_long'   => fbconnect_render_button(array('size' => 'large' , 'length' => 'long', 'onclick' => 'return false;')),
    );

    $form['display']['fbconnect_button'] = array(
      '#type' => 'radios',
      '#default_value' => variable_get('fbconnect_button', 'medium_short'),
      '#options' => $button,
    );
    $form['display']['fbconnect_button_text'] = array(
      '#title' => t('Button text'),
      '#type' => 'textfield',
      '#description' => t('Enter the text you want to display on the Facebook Connect Button.'),
      '#default_value' => variable_get('fbconnect_button_text', 'Connect'),
    );  
  }
  
  $form['display']['fbconnect_pic_allow'] = array(
    '#type' => 'select',
    '#title' => t('Facebook picture settings'),
    '#description' => t('Here you can deside whether you want to allow the use of the facebook picture - or not.'),
    '#default_value' => variable_get('fbconnect_pic_allow', 'allow'),
    '#options' => array(
      'allow' => t('Let the user choose'),
      'disallow' => t('Do not allow the use of facebook user picture at your site'),
    ),
  );
  
  $form['display']['fbconnect_pic_size'] = array(
    '#type' => 'select',
    '#title' => t('Facebook picture size'),
    '#description' => t('If your user decides to use his facebook picture, choose its size here.'),
    '#default_value' => variable_get('fbconnect_pic_size', 'square'),
    '#options' => array(
      'thumb' => t('Thumb'),
      'small' => t('Small'),
      'normal' => t('Normal'),
      'square' => t('square'),
    ),
  );

  $form['display']['fbconnect_pic_logo'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show Facebook Logo'),
    '#description' => t('Uncheck this if you don\'t want to display the facebook logo on the user picture.'),
    '#value' => variable_get('fbconnect_pic_logo', TRUE),
  );

  $form = system_settings_form($form);
  $form['#submit'][] = 'fbconnect_set_appproperties';
  return $form;
}

function fbconnect_set_appproperties($form, &$form_state) {
  if (!facebook_client()) {
    drupal_set_message(t('Unable to get a facebook client, check your api key'), 'error');
    return FALSE;  
  }
  if (!fbconnect_get_fbuid()) {
    drupal_set_message(t('Unable to autoconfigure your application settings, make sure you are logged into Facebook'), 'error');
    return FALSE;
  }
  
  $properties = array(
    'connect_url'   => $form_state['values']['fbconnect_connect_url'],
    'base_domain'   => $form_state['values']['fbconnect_base_domaine'],
    'uninstall_url' => url('fbconnect/post-remove', array('absolute' => TRUE)),
  );
  try {
    $res = facebook_client()->api_client->admin_setAppProperties($properties);
  }
  catch (Exception $e) {
    drupal_set_message(t('Exception thrown while using admin_setAppProperties : @code', array('@code' => $e->getMessage())), 'error');
  }
  if (!$res) {
    drupal_set_message(t('Checking your facebook session, you are not allowed to change this application settings'), 'error');
  }
  else {
    drupal_set_message(t('Your facebook app settings has been updated'));      
  }
    
}