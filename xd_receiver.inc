<?php 

/**
 * Without a receiver file, cross-domain javascript will not work.
 *
 * In their infinite wisdom, facebook has decreed that the URL for
 * this static page be in the same place as the app's callback URL.
 * So we have to make a Drupal callback for what would otherwise be a
 * simple file.
 *
 * Supports SSL, http://wiki.developers.facebook.com/index.php/Facebook_Connect_Via_SSL.
 */
function fbconnect_receiver() {
	$conf = fbconnect_get_config();
  $src_suffix = $conf['debug']
					    ? '.debug'
					    : '' ;
  
  $src = (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] == 'on') 
    ? "https://ssl.connect.facebook.com/js/api_lib/v0.4/XdCommReceiver$src_suffix.js" 
    : "http://static.ak.connect.facebook.com/js/api_lib/v0.4/XdCommReceiver$src_suffix.js" ;
  
  $output = <<<HTML
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" >
<body>
  <!-- Drupal for Facebook cross-domain receiver. -->
  <!-- http://wiki.developers.facebook.com/index.php/Cross_Domain_Communication_Channel -->
  <script src="$src" type="text/javascript"></script>
</body>
</html>
HTML;
  print $output;
  die(); // prevent Drupal from writing anything else.
}