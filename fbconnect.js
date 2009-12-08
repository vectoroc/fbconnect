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
		
		Drupal.fbconnect.initLogoutLinks(context);
	}
};

Drupal.fbconnect = {};
Drupal.fbconnect.initLogoutLinks = function(context) {
	if (Drupal.settings.fbconnect.loginout_mode != 'ask') return;
	var links = $('a[href=/logout]', context).not('.logout_link_inited');
	links.addClass('logout_link_inited');
	links.click(function() {
		var buttons = [
		    {
		    	'label': Drupal.t('Logout me from Facebook account'), 
		    	'click': function() {
		    		this.close();
		    		FB.Connect.logout(function() { 
		    			window.location = Drupal.settings.basePath + '/logout'; 
		    		});
		    	}
		    }, {
		    	'name': 'cancel', 
		    	'label': Drupal.t('No thanks. Keep me logged in'), 
		    	'click': function() {
			    	this.close();
		    		window.location.pathname = Drupal.settings.basePath + 'logout'; 
		    	}
		    }					    
		];
	
		var dialog = new Drupal.fbconnect.PopupDialog({
			'title' : Drupal.t('Logout'),
			'message' : Drupal.t('Do you want logout from your Facebook account'),
			'buttons' : buttons 
		});
		
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
