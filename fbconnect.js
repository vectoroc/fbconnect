Drupal.behaviors.fbconnect = function(context) {
	if (window.FB && Drupal.settings.fbconnect && Drupal.settings.fbconnect.api_key) {
		FB.Bootstrap.requireFeatures(["Connect","XFBML"], function() {
			FB.Facebook.init(
				Drupal.settings.fbconnect.api_key, 
				Drupal.settings.basePath + Drupal.settings.fbconnect.xd_path
			);
			$(context).each(function() {
				var elem = $(this).get(0);
				FB.XFBML.Host.parseDomElement(elem);
			});
		});
	}
};

function facebook_onlogin_ready() {
	$("#fbconnect-autoconnect-form").submit();
}