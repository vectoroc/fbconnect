<?php
// $Id$

/** 
 * @file 
 * Administration page callbacks for the fbconnect module. 
 */ 

/** 
 * Form builder. Configure fbconnect. 
 * 
 * @ingroup forms 
 * @see system_settings_form(). 
 */
function fbconnect_admin_settings() {
   $form['api'] = array(
      '#type' => 'fieldset',
      '#title' => t('Facebook API config'),
      '#collapsible' => FALSE,
   );
   $form['api']['fbconnect_api_key'] = array( 
      '#type' => 'textfield', 
      '#title' => t('Facebook API KEY'), 
      '#default_value' => variable_get('fbconnect_api_key', ''), 
   );   
   $form['api']['fbconnect_secret_api_key'] = array(
      '#type' => 'textfield',
      '#title' => t('Facebook Secret API KEY'),
      '#default_value' => variable_get('fbconnect_secret_api_key', ''),
   );   
   $form['friend'] = array(
      '#type' => 'fieldset',
      '#title' => t('Friends invite'),
      '#collapsible' => FALSE,
   );   
   $form['friend']['fbconnect_invitef_content'] = array(
      '#type' => 'textfield',
      '#title' => t('Invite message'),      
      '#default_value' => variable_get('fbconnect_invitef_content', t('Enjoy the new drupal facebook connect module')),
   );
   $form['friend']['fbconnect_invitef_type'] = array(
      '#type' => 'textfield',
      '#title' => t('Your site name'),      
      '#default_value' => variable_get('fbconnect_invitef_type', variable_get('site_name', '')),
   );
   $form['button'] = array(
      '#type' => 'fieldset',
      '#title' => t('Connect button type'),
      '#collapsible' => FALSE,
    );
   $module_path = drupal_get_path('module', 'fbconnect') .'/images/';
   $button_options = array(
     'small_short' => theme_image($module_path .'Connect_white_small_short.gif'),
     'medium_short' => theme_image($module_path .'Connect_white_medium_short.gif'),
     'medium_long' => theme_image($module_path .'Connect_white_medium_long.gif'),
     'large_short' => theme_image($module_path .'Connect_white_large_short.gif'),
     'large_long' => theme_image($module_path .'Connect_white_large_long.gif'),
   );
   $form['button']['fbconnect_button_type'] = array(
     '#type' => 'radios',
     '#default_value' => variable_get('fbconnect_button_type', 'large_long'),
     '#options' => $button_options,
   );
   $form['feed'] = array(
       '#type' => 'fieldset',
       '#title' => t('Story feed'),
       '#collapsible' => FALSE,
    );
    $form['feed']['fbconnect_reg_feed_id'] = array(
       '#type' => 'textfield',
       '#title' => t('Registration feed bundle ID'),
       '#default_value' => variable_get('fbconnect_reg_feed_id', FBCONNECT_REG_FEED_BUNDLEID),
    );
    $form['feed']['fbconnect_com_feed_id'] = array(
       '#type' => 'textfield',
       '#title' => t('Comment feed bundle ID'),
       '#default_value' => variable_get('fbconnect_com_feed_id', FBCONNECT_COMMENT_FEED_BUNDLEID),
    );
    $form['story_feed']['fbconnect_reg_feed'] = array(
       '#type' => 'checkbox',
       '#title' => t('Display story feed prompt after user registration'),      
       '#default_value' => variable_get('fbconnect_reg_feed', array('register_feed')),
    );  
   global $base_url;   
   $form['friend']['fbconnect_invitef_redirect'] = array(
      '#type' => 'textfield',
      '#title' => t('Redirect url, when user valid or skip invite friend form'),
      '#default_value' => variable_get('fbconnect_invitef_redirect', $base_url),
   );
   $form['import'] = array(
      '#type' => 'fieldset',
      '#title' => t('Information to import'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
   );   
   $form['import']['fbconnect_field_to_import'] = array(
      '#type' => 'checkboxes',
      '#options' => variable_get('facebook_user_fields', NULL),
      '#default_value' => variable_get('fbconnect_field_to_import', array_keys(variable_get('facebook_user_fields', NULL))),
   );
   return system_settings_form($form); 
}