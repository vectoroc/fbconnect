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
   $form['fbconnect_api_config'] = array(
      '#type' => 'fieldset',
      '#title' => t('Facebook API config'),
      '#collapsible' => FALSE,
   );
   // Facebook api key
   // Get the API key from http://facebook.com/developers
   $form['fbconnect_api_config']['fbconnect_api_key'] = array( 
      '#type' => 'textfield', 
      '#title' => t('Facebook API KEY'), 
      '#default_value' => variable_get('fbconnect_api_key', ''), 
   );   
   // Facebook api secret key
   $form['fbconnect_api_config']['fbconnect_secret_api_key'] = array(
      '#type' => 'textfield',
      '#title' => t('Facebook Secret API KEY'),
      '#default_value' => variable_get('fbconnect_secret_api_key', ''),
   );   
   $form['fbconnect_friends_invite'] = array(
      '#type' => 'fieldset',
      '#title' => t('Friends invite'),
      '#collapsible' => FALSE,
   );   
   // Facebook friends invite message
   $form['fbconnect_friends_invite']['fbconnect_invitef_content'] = array(
      '#type' => 'textfield',
      '#title' => t('Invite message'),      
      '#default_value' => variable_get('fbconnect_invitef_content', t('Enjoy the new drupal facebook connect module')),
   );
   // Facebook friends invite fields
   $form['fbconnect_friends_invite']['fbconnect_invitef_type'] = array(
      '#type' => 'textfield',
      '#title' => t('Your site name'),      
      '#default_value' => variable_get('fbconnect_invitef_type', variable_get('site_name', '')),
   );
   $form['story_feed'] = array(
       '#type' => 'fieldset',
       '#title' => t('Story feed'),
       '#collapsible' => FALSE,
    );
    $form['story_feed']['fbconnect_reg_feed_bundle_id'] = array(
       '#type' => 'textfield',
       '#title' => t('Registration feed bundle ID'),      
       '#default_value' => variable_get('fbconnect_reg_feed_bundle_id', FBCONNECT_REG_FEED_BUNDLEID),
    );
    $form['story_feed']['fbconnect_comment_feed_bundle_id'] = array(
       '#type' => 'textfield',
       '#title' => t('Comment feed bundle ID'),      
       '#default_value' => variable_get('fbconnect_comment_feed_bundle_id', FBCONNECT_COMMENT_FEED_BUNDLEID),
    );
    // Facebook after registration feed
    $form['story_feed']['fbconnect_registered_feed'] = array(
       '#type' => 'checkbox',
       '#title' => t('Display story feed prompt after user registration'),      
       '#default_value' => variable_get('fbconnect_registered_feed', array('register_feed')),
    );  
   global $base_url;   
   // Facebook friends invite fields
   $form['fbconnect_friends_invite']['fbconnect_invitef_redirect'] = array(
      '#type' => 'textfield',
      '#title' => t('Redirect url, when user valide or skip invite friend form'),
      '#default_value' => variable_get('fbconnect_invitef_redirect', $base_url),
   );
   $form['fbconnect_import'] = array(
      '#type' => 'fieldset',
      '#title' => t('Information to import'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
   );   
   // Facebook users fields to import
   $form['fbconnect_import']['fbconnect_field_to_import'] = array(
      '#type' => 'checkboxes',
      '#options' => variable_get('facebook_user_fields', NULL),
      '#default_value' => variable_get('fbconnect_field_to_import', array_keys(variable_get('facebook_user_fields', NULL))),
   );
   return system_settings_form($form); 
}