// $Id: fbconnect.js,v 1.4 2010/03/21 16:26:03 vectoroc Exp $

Drupal.fbconnect = {};
Drupal.fbconnect.init = function () {
	Drupal.behaviors.fbconnect = function(context) {
		FB.XFBML.parse(context);
//		switch (Drupal.settings.fbconnect.loginout_mode) {
//		case 'auto':
//			Drupal.fbconnect.initLogoutLinks(context);
//			$(document.body).bind(
//					'fbconnect:ifUserConnected',
//					Drupal.fbconnect.reload_ifUserConnected
//				);
//			break;
//			
//		case 'ask':
//			Drupal.fbconnect.initLogoutLinks(context);
//			break;
//		}
	}
	
	Drupal.behaviors.fbconnect(document);
	
	
//	FB.Event.subscribe('auth.sessionChange', function(response) {
//	console.dir(response);
//});

//FB.Bootstrap.requireFeatures(["Connect","XFBML"], function() {//			
//	var appInitSettins = {
//		ifUserConnected    : function(fbuid) {
//			$.event.trigger('fbconnect:ifUserConnected', [fbuid, settings.fbuid]);
//		},
//		ifUserNotConnected : function(fbuid) {
//			$.event.trigger('fbconnect:ifUserNotConnected', [fbuid, settings.fbuid]);
//		}
//	};
//});


}

Drupal.fbconnect.reload_ifUserConnected = function(e, fbuid, old_fbuid) {
	if (Drupal.settings.fbconnect.user.uid) return;
	if (fbuid != old_fbuid) window.location.reload();
};

Drupal.fbconnect.initLogoutLinks = function(context) {
	var loginout_mode = Drupal.settings.fbconnect.loginout_mode;
	var user          = Drupal.settings.fbconnect.user;
	var basePath      = Drupal.settings.basePath;
	var logout_url    = basePath + 'logout'; 
	var links         = $('a[href='+ logout_url +']', context).not('.logout_link_inited');
	
	links.addClass('logout_link_inited').click(function() {
		if (!user.fbuid || user.fbuid != FB.Connect.get_loggedInUser()) return;
		if (loginout_mode == 'auto') { 
			FB.Connect.logoutAndRedirect(logout_url);	
		}
		else if (loginout_mode == 'ask') {		
			var t_args  = {'!site_name' : Drupal.settings.fbconnect.invite_name};
			var buttons = [
			    { 
			    	'label': Drupal.t('Facebook and !site_name', t_args), 
			    	'click': function() {
			    		this.close();
			    		FB.Connect.logoutAndRedirect(logout_url);
			    	}
			    }, {
			    	'name': 'cancel', 
			    	'label': Drupal.t('!site_name Only', t_args), 
			    	'click': function() {
				    	this.close();
				    	console.log('window.location.href = ' + logout_url);
//				    	window.location.href = logout_url; 
			    	}
			    }					    
			];
		
			var dialog = new Drupal.fbconnect.PopupDialog({
				'title'   : Drupal.t('Logout'),
				'message' : Drupal.t('Do you also want to logout from your Facebook account?'),
				'buttons' : buttons 
			});
		}
		
		return false;
	});
};

function facebook_onlogin_ready() {
	$("#fbconnect-autoconnect-form").submit();
}

Drupal.theme.prototype.fb_popup_dialog_buttons = function(buttons, dialog) {
	buttons = buttons || {};
	var container = $('<div class="fb_dialog_buttons"></div>');
	
	jQuery.each(buttons, function(i, btn) {
		var button = $('<input type="button" class="fb_inputbutton">');
		if (!btn['name']) btn['name'] = i;
		if (btn.attr) button.attr(btn.attr);
		if (btn['class']) button.addClass(btn['class']);
		if (btn['name'] == 'cancel') button.addClass('fb_inputaux');
		button.addClass('fb_button_' + i);
		button.attr('value', btn.label);
		button.click(dialog.createHandler('click', btn));
		button.appendTo(container);
	});
		
	return container.get(0);
};

Drupal.theme.prototype.fb_popup_dialog = function(options) {
	options = options || {buttons:{}};
	var container = document.createDocumentFragment();
	var elements  =	[
     	'<div class="fb_confirmation_stripes"></div>',
		'<div class="fb_confirmation_content">',
		options.message.toString(),
		'</div>'
	];
	
	$(elements.join("\n")).each(function() {
		container.appendChild(this);
	});
	if (options.buttons) {
		container.appendChild(
			Drupal.theme('fb_popup_dialog_buttons', options.buttons, options.dialog)
		);
	}

	return container;
};

Drupal.fbconnect.PopupDialog = function(options) {
	this.prepareDefaults(options);
	this.container = Drupal.theme('fb_popup_dialog', this.options);
	var oThis = this;
	
	FB.ensureInit(function() {		
		var popup = new FB.UI.PopupDialog(
			oThis.options.title, 
			oThis.container, 
			oThis.options.showLoading,
			oThis.options.hideUntilLoaded
		);
		oThis.popup = popup;
		oThis.callback('load', popup);
	});
};

Drupal.fbconnect.PopupDialog.prototype.options = {};
Drupal.fbconnect.PopupDialog.prototype.createHandler = function(event, data) {
	var oThis = this;
	return function() { oThis.callback(event, data); };
};
Drupal.fbconnect.PopupDialog.prototype.prepareDefaults = function(options) {
	var defaults = {
		'title'           : '',
		'message'         : ' - ',
		'buttons'         : {},
		'showLoading'     : false,
		'hideUntilLoaded' : false
	};	
	$.extend(this.options, defaults, options);
	
	this.__close_handler = this.createHandler('close', {});
	this.options.dialog = this; 
	if (this.options.callback instanceof Function) {
		this.callback = this.options.callback;
	}
};

Drupal.fbconnect.PopupDialog.prototype.show = function() {
	if (this.popup) {
		this.popup.add_closing(this.__close_handler);
		this.popup.show();
	}
};

Drupal.fbconnect.PopupDialog.prototype.close = function() {
	if (this.popup) {
		this.popup.remove_closing(this.__close_handler);
		this.popup.close(true);
	}
};

Drupal.fbconnect.PopupDialog.prototype.findButton = function(name) {
	var button = null;
	jQuery.each(this.options.buttons, function(i, btn) {
		if (btn.name == name) {
			button = btn;
			return true;
		}
	});
	
	return button;
}

Drupal.fbconnect.PopupDialog.prototype.callback = function(event, data) {
	data = data || {};
	switch (event) {
	case 'click':
		var btn = data;
		if (btn.click instanceof Function) btn.click.apply(this, [btn]);
		else if (btn.name == 'cancel') this.close();
		break;
		
	case 'close':
		this.popup.remove_closing(this.__close_handler);
		var btn = this.findButton('cancel'); 
		if (btn) this.callback('click', btn);
		break;
		
	case 'load':
		this.show();
		break;
	}
};

Drupal.theme.prototype.fbml_name = function(fbuid, options) {
	var output = ['<fb:name uid="', fbuid, '"'];
	var defaults = {
		'useyou' : false,
		'linked' : false
	};
	
	options = $.extend({}, defaults, options);
	
	output.push('" useyou="'+ (!!options.useyou ? 'true' : 'false') +'"');
	output.push('" linked="'+ (!!options.linked ? 'true' : 'false') +'"');
	output.push('></fb:name>');
	
	return output.join('');
};

Drupal.theme.prototype.fbml_profile_pic = function(fbuid, options) {
	var output = ['<fb:profile-pic uid="', fbuid, '"'];
	options = options || {};
	
	if (options.width)  output.push('" width="'+ options.width +'"');
	if (options.height) output.push('" height="'+ options.height +'"');
	if (options.size)   output.push('" size="'+ options.size +'"');
	
	output.push('" facebook-logo="'+ (!!options['facebook-logo'] ? 'true' : 'false') +'"')
	output.push('" linked="'+ (!!options.linked ? 'true' : 'false') +'"');	
	output.push('></fb:profile-pic>');
	
	return output.join('');
};
