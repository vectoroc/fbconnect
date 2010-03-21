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
Facebook PHP Library: http://svn.facebook.com/svnroot/platform/clients/packages/facebook-platform.tar.gz 
Facebook API key: http://www.facebook.com/developers/


-- INSTALLATION --

  1. Upload the 'fbconnect' folder into your module directory, 
  
  2. Download the Facebook PHP libraries from http://svn.facebook.com/svnroot/platform/clients/packages/facebook-platform.tar.gz. 
    Copy the content of the "php" folder into the "facebook-client" folder (see facebook-client/README.TXT)
      
  3. Create a new application: http://www.facebook.com/developers/createapp.php  
    (more information here: http://developers.facebook.com/get_started.php), leave app default settings. 

  4. Activate the module through your website's modules administration pages, 
  
  5. Configure the module through the 'Fbconnect' section of the 'Site configuration' menu, using the information 
    provided by Facebook (API key, Secret API key).  When editing configuration, make sure your facebook session's 
    is active. If it is'nt active then perhaps you will see message "Unable to autoconfigure your application settings, 
    make sure you are logged into Facebook". In this case do 6 from this instruction and try to connect your drupal 
    admin account with Facebook (developper of the App) on '/user/1/fbconnect' page.
     
  6. Edit the "page.tpl.php" file of your current theme. edit the <html> tag and add the facebook xmlns for rendering XFBML.
    example :
    <html xmlns:fb="http://www.facebook.com/2008/fbml" 
       xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" 
       lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">     

    Ensure that your page.tpl.php contains <?php print $closure ?> code. 
     
  7. Activate the 'fbconnect friend' block from the 'build/block' menu if you want