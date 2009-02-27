// $Id$
/*
 * Our <fb:login-button> specifies this function in its onlogin attribute,
 * which is triggered after the user authenticates the app in the Connect
 * dialog and the Facebook session has been set in the cookies.
 */
function facebook_onlogin_ready() {
 $('#fbconnect-autoconnect-form').submit();
}

/*
 * Show the feed form. This would be typically called in response to the
 * onclick handler of a "Publish" button, or in the onload event after
 * the user submits a form with info that should be published.
 *
 */
function facebook_publish_feed_story(form_bundle_id, template_data) {
  // Load the feed form
  FB.ensureInit(function() {
          FB.Connect.showFeedDialog(form_bundle_id, template_data);
  });
}
