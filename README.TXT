/* $Id: README.TXT,v 1.12 2010/03/21 16:26:03 vectoroc Exp $ */


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
Facebook PHP Library: http://github.com/facebook/php-sdk/
Facebook API key: http://www.facebook.com/developers/


-- INSTALLATION --

  1. Upload the 'fbconnect' folder into your module directory,
  
  2. Nothing here now, go to next step,  
  
  3. Activate the module through your website's modules administration pages,
      
  4. Create a new application: http://www.facebook.com/developers/createapp.php
    (more information here: http://wiki.developers.facebook.com/index.php/Connect/Setting_Up_Your_Site).
    
  5. Enter the Connect URL on Connect settings tab (e.g. copy value from admin/settings/fbconnect field “Connect url”).
  
  6. Add your server to the Server Whitelist on Advanced settings tab on Facebook or disable Session Secret Whitelist Exception. 

  7. Configure the module through the Fbconnect module (admin/settings/fbconnect) using the information provided by Facebook 
    (Application ID, Application Secret). If you are unsure what to do on tab  App Settings just press the Save configuration button.
     
  8. In order for the user's browser to correctly recognize XFBML tags, you need to specify that the page is in XHTML. 
    Edit the "page.tpl.php" file of your current theme. edit the <html> tag and add the facebook xmlns for rendering XFBML.
    
    Example : <html xmlns:fb="http://www.facebook.com/2008/fbml" 
                    xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" 
                    lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
       
    Note: The xmlns:fb attribute (xmlns:fb="http://www.facebook.com/2008/fbml") is required for proper rendering on IE. 
     (Namespaces don't have to point to a valid page, they just need to be a valid Internationalized Resource Identifier (IRI).     
     
     
  9. If you wish activate the 'fbconnect friend' block from the 'build/block' menu. Check "invite facebook friends" permission.