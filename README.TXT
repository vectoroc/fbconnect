/* $Id$ */


-- SUMMARY --

This module roughly acts like the OpenId Drupal 6.x core module :
- It lets you use your Facebook ID and password to sign-in to your drupal site.
- Users can see which of their Facebook friends had an account on your drupal site.
- Users can publish a feed on their Facebook feed announcing that they have created an account or commented an article on your website.
- Users can invite their Facebook friends to create an account on the Drupal website.
- In case the user already holds an account on the website: The user can choose to link his drupal account to his Facebook identity.

-- FACEBOOK CONNECT TAB --

This module adds a new Facebook Connect tab in the user's profile page. This tab allows to change: 
- the "Use my Facebook picture as user picture" setting.
This field can be seen only if user_pictures is enabled.
The profile picture will be displayed directly from Facebook via FBML.  

- the "Let my Facebook friends see me" setting
If this parameter is enabled, the user's will be visible by her Facebook friends.

-- REQUIREMENTS --
PHP 5.2 or higher versions. 
Drupal 6.x. 
Facebook PHP Library: http://github.com/downloads/vectoroc/fbconnect/facebook-client.zip
Facebook API key: http://www.facebook.com/developers/


-- INSTALLATION --

  1. Upload the 'fbconnect' folder into your module directory, 
  
  2. Download the Facebook PHP libraries from http://github.com/downloads/vectoroc/fbconnect/facebook-client.zip
    and place it into the fbconnect module folder
    
- Note! You can use drush_make script instead of steps 1 and 2 

 projects[fbconnect][download][type] = "git"
 projects[fbconnect][download][url] = "git://github.com/vectoroc/fbconnect.git"
 libraries[facebook-client][download][type] = "get"
 libraries[facebook-client][download][url] = "http://github.com/downloads/vectoroc/fbconnect/facebook-client.zip"
 libraries[facebook-client][destination] = "modules/fbconnect"

  3. Activate the module through your website's modules administration pages,
      
  4. Create a new application: http://www.facebook.com/developers/createapp.php
    (more information here: http://wiki.developers.facebook.com/index.php/Connect/Setting_Up_Your_Site).
    
  5. Enter the Connect URL on Connect settings tab (e.g. copy value from admin/settings/fbconnect field “Connect url”).
  
  6. Add your server to the Server Whitelist on Advanced settings tab on Facebook or disable Session Secret Whitelist Exception. 

  7. Configure the module through the Fbconnect module (admin/settings/fbconnect) using the information provided by Facebook 
    (API key, Secret API key). If you are unsure what to do on tab  App Settings just press the Save configuration button.
     
  8. Edit the "page.tpl.php" file of your current theme. edit the <html> tag and add the facebook xmlns for rendering XFBML.
    example :
    <html xmlns:fb="http://www.facebook.com/2008/fbml" 
       xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" 
       lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">     

    Ensure that your page.tpl.php contains <?php print $closure ?> code. 
     
  9. If you wish activate the 'fbconnect friend' block from the 'build/block' menu. Check "invite facebook friends" permission.