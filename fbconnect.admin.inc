<?php
// $Id$

/** 
 * @file 
 * Administration page callbacks for the fbconnect module. 
 */ 


function fbconnect_api_keys_settings(&$form_state) {
  $form['fbconnect_appid'] = array( 
    '#type' => 'textfield', 
    '#required' => true,
    '#title' => t('Application ID'), 
    '#default_value' => variable_get('fbconnect_key', NULL),
    '#description' => t('Your Application ID, <strong> not API Key </strong>'),
  );
  
  $form['fbconnect_skey'] = array(
    '#type' => 'textfield',
    '#required' => true,
    '#title' => t('Application Secret'),
    '#default_value' => variable_get('fbconnect_skey', NULL),
    '#description' => t('Do not share your secret key with anyone'),
  );
  
  $form['fbconnect_language_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Language code'),
    '#description' => t('Enter your country code here to get translated versions of facebook connect. (e.g. en_US or de_DE)'),
    '#default_value' => variable_get('fbconnect_language_code', 'en_US'),
  );
  
  $form['fbconnect_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Debug mode'),      
    '#default_value' => variable_get('fbconnect_debug', FALSE),
  );  
  
  $form['fbconnect_connect_url'] = array(
    '#type' => 'item',
    '#title' => t('Connect url'),
    '#description' => t('Copy this value into Facebook Applications on Connect settings tab'),
    '#value' => variable_get('fbconnect_connect_url', $GLOBALS['base_url'] . '/'),
  );
    
//  locale
//  connect_logo_url
//  about_url
//  base_domain
//  tos_url

  return system_settings_form($form);
}

function fbconnect_appearance_settings(&$form_state) {
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
  
  $form['site']['fbconnect_reg_options'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disable linking accounts during registration'),
    '#default_value' => variable_get('fbconnect_reg_options', 0),
    '#description' => t('Disables the option to link local account to a FB account on registration'),
  );
  
  $form['display'] = array(
    '#title' => t('Display Settings'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  
  if (fbconnect_get_config()) {  
    $button = array(
      'small_short'  => fbconnect_render_button(array('size' => 'small' , 'onclick' => 'return false;')),
      'medium_short' => fbconnect_render_button(array('size' => 'medium', 'onclick' => 'return false;')),
      'large_short'  => fbconnect_render_button(array('size' => 'large' , 'onclick' => 'return false;')),
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
    '#default_value' => variable_get('fbconnect_pic_logo', TRUE),
  );

  return system_settings_form($form);
}


function fbconnect_fbapp_settings(&$form_state) {
	if (!facebook_client()) {
		drupal_set_message(t('Ensure that you entered valid api keys.'), 'error');
		drupal_goto('admin/settings/fbconnect');
	}
	
	if (!fbconnect_get_fbuid()) {
		$button_attr = array('text' => t('Connect with your Facebook account'));
		$form['fbconnect'] = array(
		  '#value' => fbconnect_render_button($button_attr),
		);
		
		return $form;
	}
	
  $form['fbconnect_base_domaine'] = array(
    '#type' => 'textfield',
    '#title' => t('Base Domain Url'),
    '#description' => t('Use this to enable your Facebook Connect implementation to span multiple subdomains (e.g., using example.com would enable  www.example.com, foo.example.com or bar.example.com,).'),
    '#default_value' => variable_get('fbconnect_base_domaine', NULL),
  );
  
  $form['fbconnect_connect_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Connect Url'),
    '#description' => t('Your site\'s main URL.'),
    '#default_value' => variable_get('fbconnect_connect_url', $GLOBALS['base_url'] . '/'),
  );
  
  $form['fbconnect_uninstall_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Post-Remove Callback Url'),
    '#description' => t('Facebook pings this URL when a user removes your application.'),
    '#default_value' => variable_get(
      'fbconnect_uninstall_url',
      url('fbconnect/post-remove', array('absolute' => TRUE))
    ),
  );
	
  $form = system_settings_form($form);
  $form['#submit'][] = 'fbconnect_fbapp_settings_submit';
  return $form;
}

function fbconnect_fbapp_settings_submit($form, &$form_state) {
  $properties = array(
    'connect_url'   => $form_state['values']['fbconnect_connect_url'],
    'base_domain'   => $form_state['values']['fbconnect_base_domaine'],
    'uninstall_url' => $form_state['values']['fbconnect_uninstall_url'],
  );
  
  if (!facebook_client()) {
    drupal_set_message(t('Unable to get a facebook client, check your api key'), 'error');
    return FALSE;  
  }
  if (!fbconnect_get_fbuid()) {
    drupal_set_message(t('Unable to autoconfigure your application settings, make sure you are logged into Facebook'), 'warning');
    return FALSE;
  }
  
  try {
    $res = facebook_client()->api(array(
      'method' => 'admin.setAppProperties',
      'properties' => array_filter($properties),
    ));
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