/* $Id$ /

-- SUMMARY --

	This module roughly acts like the OpenId Drupal 6.x core module (drupal account is linked to your facebook ID and give you the ability to log in the Drupal site with your facebook email/password) with some extra features :
	- You can create a Drupal account via the Facebook connect button
	- During the account creation phase, you can choose which of your Facebook public informations you want to import and synchronize with your future Drupal account, according to Facebook connect API Terms of use
	- You can see wich of your facebook friends that already have an account in this site
	- You can publish a story on your facebook feed with a customizable message
	- You can invite your facebook friends to create an account


-- REQUIREMENTS --

None.


-- INSTALLATION --

	1. Log in to the Facebook Developer application: http://www.facebook.com/developers/
	2. Create a new application, more info: http://developers.facebook.com/get_started.php
	3. Upload the `fbconnect` folder into your module directory
	4. Activate the module through the modules menu
	5. Configure the module through the 'Fbconnect settings' section of the 'Site configuration' menu
	6. Use the Facebook App info to configure the module. (API key, Secret API key).
	7. Create a new feed template for after registration feed, or leave the default id bundle. http://developers.facebook.com/tools.php?feed
	8. Activate the Facebook Connector block from the 'build/block' menu.
	9. Modify your page.tpl.php file in your theme folder.
	Add the facebook xmlns : <html xmlns="http://www.w3.org/1999/xhtml" xmlns:fb="http://www.facebook.com/2008/fbml">


-- CONFIGURATION --

