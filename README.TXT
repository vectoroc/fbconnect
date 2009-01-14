/* $Id$ /


-- SUMMARY --


This module roughly acts like the OpenId Drupal 6.x core module (it gives you the ability to log in the Drupal site with your Facebook email and password after linking both accounts) with some extra features:

- Facebook and Drupal accounts synchronization: After logging through Facebook Connect, users can create a local Drupal account, 

- Public profile information import: Users can choose which of their Facebook public information they want to import and synchronize with their Drupal local account, following Facebook Connect API Terms of use, 

- Visibility: Users can see which of their Facebook friends already have an account on the Drupal website (provided that these users have checked the "Let my Facebook friends see me on !sitename" box on their profile page), 

- Story feed: Users can publish a message on their Facebook story feed announcing that they have created an account on the website (future versions could make this message customizable), 

- Friends invite form: At the end of the registration process, users can invite their Facebook friends to create an account on the Drupal website. This step will be skipped if the user has unchecked the "Let my Facebook friends see me on !sitename" box. The page will remain accessible through a link in the Fbconnect Friends List block. 

- In case the user already holds an account on the website: The user can link his Drupal account to his Facebook identity under the "Facebook Connect" tab of the profile page. After opening a Facebook session and submitting the form, the user can log in on the website using the Facebook Connect button instead of the usual identification form. 


-- IMPORTED INFORMATION --


Facebook Connect's terms of use provide that profile information imported from Facebook should not be stored over 24 hours (http://developers.facebook.com/terms.php).

In order to comply with this legal rule, profile data are automatically deleted by cron after 24 hours. If the user reconnects 12 to 24 hours following the latest import, data are updated and the 24-hour timer is reset. 


-- FACEBOOK CONNECT TAB -- 


This module adds a new Facebook Connect tab in the user's profile page. This tab allows to change: 

- the "Use Facebook profile picture" setting.
This field can be seen only if user_pictures is enabled. Value can be changed only if a Facebook session is active. 
The profile picture is updated every 24 hours by the cron process in order to comply with Facebook's terms of use.  

- the "Let my Facebook friends see me on !sitename" parameter
If this parameter is enabled, the user's profile will be visible from his Facebook friends holding an account on the website. 

- the import parameters
Allows to select the data fields imported from Facebook and to control the public information displayed on the user's profile. After the changes are submitted, selected data are uppdated and the 24-hour timer is reset. 
Values can be changed only if a Facebook session is active. 


-- REQUIREMENTS --


PHP 5.2 or higher versions. 
Drupal 6.x. 
Facebook PHP Library: http://svn.facebook.com/svnroot/platform/clients/packages/facebook-platform.tar.gz 
Facebook API key: http://www.facebook.com/developers/


-- INSTALLATION --


	1. Upload the 'fbconnect' folder into your module directory, 
	2. Download the Facebook PHP libraries from http://svn.facebook.com/svnroot/platform/clients/packages/facebook-platform.tar.gz. 
	    Copy the facebook-platform/php folder content to the facebook-client repertory, 
	3. Create a new application: http://www.facebook.com/developers/createapp.php (more information here: http://developers.facebook.com/get_started.php). Do not forget to define the callback URL (generally your website's domain name), 
	4. Activate the module through your website's modules administration pages, 
	5. Configure the module through the 'Fbconnect' section of the 'Site configuration' menu, using the information provided by Facebook (API key, Secret API key), 
	6. Activate the 'fbconnect friend list' block from the 'build/block' menu, 
	7. Edit the page.tpl.php file in your theme folder. Add the facebook xmlns for rendering FBML with Internet Explorer : <html xmlns="http://www.w3.org/1999/xhtml" xmlns:fb="http://www.facebook.com/2008/fbml">. 


-- KNOWN ISSUES -- 


Drupal's global cache seems to conflict with fbconnect's Javascript: it should be disabled before operating the module. 


-- MISCELLANEOUS --


Story feed: 

This module produces 2 story feeds. The first feed ('registration feed') will be prompted after the user has created his Drupal account. This feed will not be prompted if the user has unchecked the "Let my Facebook friends see me on !sitename" box during the registration process. It can be disabled in the "Settings" section of the module. 

The second feed ('comment feed') will be prompted after submitting the comment form, only if a Facebook session is active. 

'fbconnect friend list' block: 

This block can be enabled in Drupal's blocks administration menu. It displays a welcome message, a link to the Facebook friends invitation page and a list of the user's Facebook friends holding an account on the website. The user's friends' profile pictures are also displayed if user_pictures is enabled. 

In order to preserve performance, this block will be cached by default for 1 hour, even if Drupal's blocks cache is disabled. This block is visible only if the user is connected and a Facebook session is active. 