// $Id$

Drupal.fbconnect = Drupal.fbconnect || {};
Drupal.fbconnect.steam_publish = function() {
  if (Drupal.settings['FB.streamPublish']) {
    FB.ui(Drupal.settings['FB.streamPublish']);
  }	
};

jQuery(document).bind('fb:init', Drupal.fbconnect.steam_publish);