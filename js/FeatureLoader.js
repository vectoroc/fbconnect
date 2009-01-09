/*    HTTP Host:  www.facebook.com                                             */
/*    Generated:  December 8th 2008 10:40:00 AM PST                            */
/*      Machine:  10.16.100.129                                                */

/*
 * FeatureLoader.js.php is designed to be the minimal set of code necessary to
 * use the Facebook Javascript API.
 *
 * The majority of the API is split into "features" which can be loaded
 * separately or all at once. Most features are packaged together into
 * connect.js.pkg.php, so it doesn't really matter which ones you pull in.
 *
 */

// hack namespace
var FB = {}

FB.Bootstrap = {

  /*
   * Use this to request loading of features in Facebook Client JavaScript library
   * @param features  array of features (see wiki for options)
   * @param callback  callback function to be executed when all required features
   *                  are finished loading
   */
  requireFeatures : function(features, callback) {
    FB.Bootstrap.enqueueFeatureRequest({"features": features,
                                        "callback": callback,
                                        "loadedCount": 0});
    if (FB.FeatureLoader && FB.Bootstrap.FeatureMap) {
      FB.FeatureLoader.singleton.checkRequestQueue();
    }
  },

  /*
   * Convenient wrapper for calling Facebook API calls. Because the Facebook
   * API is dynamically loaded, this guarantees that your function isn't called
   * until both the libraries are loaded and initialized.
   *
   * Use in conjunction with FB.init.
   * Example usage:
      FB.ensureInit (  function () {
       // ... any code in the Facebook library
       });
   *
   * @param callback   function to call when Facebook is dynamically loaded.
   * @throws exception if FB.init is not called within the document.
   */
  ensureInit : function(callback) {
    if (!callback) {
      throw("FB.ensureInit called without a valid callback");
    }

    // short-circuit if initialization has already been called
    if (FB.Facebook &&
        FB.Facebook.get_initialized &&
        FB.Facebook.get_initialized().get_isReady() &&
        FB.Facebook.get_initialized().result) {
      return callback();
    }

    // if it's not already initialized, then queue it up
    // by the time this callback is executed, FB.Facebook.init
    // must have been called or else
    FB.Bootstrap.requireFeatures(FB.Bootstrap.features, function() {
        if (!FB.Facebook.get_initialized().get_isReady()) {
          throw("FB.ensureInit was called without initializing Facebook libraries. Did you call FB.init?");
        }

        callback();
      });
  },

  /*
   * This safely initializes the Facebook API for use on a Connect or iframe site.
   *
   * It is a wrapper around FB.Facebook.init, provided here so that it is available
   * before the rest of the library is dynamically loaded. All subsequent calls
   * must be wrapped in FB.ensureInit() in order to guarantee that the init function is
   * called first.
   *
   * Example Usage:
   *
   <body>
   ...
   <script type="text/javascript" src="http://static.ak.connect.facebook.com/js/api_lib/v0.4/FeatureLoader.js.php"></script>
   <script type="text/javascript">
       FB.init("API_KEY", "xd_receiver.php")
   </script>
   </body>
   *
   *  @param api_key       your API key provided by the developer app
   *  @param xd_receiver   The cross-domain receiver file on your domain.
   *                       Suggest using an absolute URL like "/xd_receiver.htm"
   */
  init : function(api_key, xd_receiver) {
    // bind to the onload handler
    FB.Bootstrap.requireFeatures(FB.Bootstrap.features, function() {
        if (FB.Facebook) {
          // init has changed definition by now
          FB.Facebook.init(api_key, xd_receiver);
        }
      });
  },

  /*
   * Dynamically add a script tag to the document.
   */
  addScript : function(src) {
    var scriptElement = document.createElement("script");
    scriptElement.type = "text/javascript";
    scriptElement.src = src;
    document.getElementsByTagName("HEAD")[0].appendChild(scriptElement);
  },

  /*
   * Use detectDOMContentReady to determine whether window is loaded.
   * Because there is no way to determine a window is loaded after it is
   * already loaded, we must initialize the state to false in a code that
   * will be executed before the window is loaded, then listen to the window
   * load event.
   *
   * Since FeatureLoader.js.php is the only script we have that is not
   * dynamically loaded, we must place this code in this file.
   */
  detectDOMContentReady : function() {
    if (window.navigator.userAgent.toLowerCase().indexOf("msie") >= 0) {
      window.attachEvent("onload", function() {
          FB.Bootstrap.IsDomContentReady = true;
        });
    } else {
      window.addEventListener("DOMContentLoaded", function() {
          FB.Bootstrap.IsDomContentReady = true;
        }, false);
    }
  },

  /*
   * Create a hidden DOM container element. This is used to store hidden
   * iframes. If developers do not want the document.write to be called,
   * they can create their own hidden div named "FB_HiddenContainer".
   */
  createHiddenDiv : function() {
    if (document.getElementById('FB_HiddenContainer') == null) {
      document.write('<div id="FB_HiddenContainer" '
                     + 'style="position:absolute; top:-10000px; left:-10000px; width:0px; height:0px;" >'
                     + '</div>');
    }
  },

  /*
   * Loads the map of feature => file that enables dynamic loading of JS files.
   * Note that for now, these are pretty much all pointing to the same file,
   * but we hope to implement some optimizations in the future to make this
   * more customizable.
   * @param featureMap          map of feature => file
   * @parm  staticResourceMap   map of static resource identifier => file
   */
  loadResourceMaps : function(featureMap, staticResourceMap) {
    if(!this.FeatureMap.length) {
      this.FeatureMap = featureMap;
      this.StaticResourceVersions = staticResourceMap;
      if (FB.FeatureLoader) {
        FB.FeatureLoader.singleton.checkRequestQueue();
      } else {
        this.addScript(this.FeatureMap["Base"].src);
      }
    }
  },

  /*
   *
   */
  enqueueFeatureRequest : function(request) {
    this.FeatureRequestQueue[this.FeatureRequestQueue.length] = request;
  },

  /*
   * Global state variables
   */
  features                 : ["XFBML", "CanvasUtil"],
  IsDomContentReady        : false,
  FeatureRequestQueue      : [],
  FeatureMap               : [],
  StaticResourceVersions   : [],
  CustomFeatureMap         : []
};

/*
 * Define shorthand functions for ease of use.
 */
window.FB_RequireFeatures        = FB.Bootstrap.requireFeatures;
window.FB.init                   = FB.Bootstrap.init;
window.FB.ensureInit             = FB.Bootstrap.ensureInit,

/*
 * Execute functions that are only effective if executed at page load time.
 */
FB.Bootstrap.createHiddenDiv();
FB.Bootstrap.detectDOMContentReady();
FB.Bootstrap.loadResourceMaps({"Base":{"src":"http:\/\/static.ak.fbcdn.net\/rsrc.php\/z88Z2\/lpkg\/br34ulub\/fr_FR\/141\/134772\/js\/connect.js.pkg.php","dependencies":null},"Common":{"src":"http:\/\/static.ak.fbcdn.net\/rsrc.php\/z88Z2\/lpkg\/br34ulub\/fr_FR\/141\/134772\/js\/connect.js.pkg.php","dependencies":["Base"]},"XdComm":{"src":"http:\/\/static.ak.fbcdn.net\/rsrc.php\/z88Z2\/lpkg\/br34ulub\/fr_FR\/141\/134772\/js\/connect.js.pkg.php","dependencies":["Common"]},"CacheData":{"src":"http:\/\/static.ak.fbcdn.net\/rsrc.php\/z8H9R\/l\/ck0tst4k\/nu_ll\/117867\/js\/api_lib\/v0.4\/CacheData.js","dependencies":["Common","XdComm"]},"Api":{"src":"http:\/\/static.ak.fbcdn.net\/rsrc.php\/z88Z2\/lpkg\/br34ulub\/fr_FR\/141\/134772\/js\/connect.js.pkg.php","dependencies":["XdComm"]},"CanvasUtil":{"src":"http:\/\/static.ak.fbcdn.net\/rsrc.php\/z88Z2\/lpkg\/br34ulub\/fr_FR\/141\/134772\/js\/connect.js.pkg.php","dependencies":["Common","XdComm"]},"Connect":{"src":"http:\/\/static.ak.fbcdn.net\/rsrc.php\/z88Z2\/lpkg\/br34ulub\/fr_FR\/141\/134772\/js\/connect.js.pkg.php","dependencies":["CanvasUtil","Api"],"styleSheets":["http:\/\/static.ak.fbcdn.net\/rsrc.php\/z25ED\/l\/rjn5h2zn\/fr_FR\/132011\/css\/fb_connect.css"]},"XFBML":{"src":"http:\/\/static.ak.fbcdn.net\/rsrc.php\/z88Z2\/lpkg\/br34ulub\/fr_FR\/141\/134772\/js\/connect.js.pkg.php","dependencies":["CanvasUtil","Api","Connect"]},"Integration":{"src":"http:\/\/static.ak.fbcdn.net\/rsrc.php\/z5TFV\/l\/d2moxi08\/nu_ll\/132011\/js\/api_lib\/v0.4\/Integration.js","dependencies":["Connect"]}}, {"base_url_format":"http:\/\/{0}.facebook.com\/","api_channel":133168,"api_server":133168,"www_channel":132011,"xd_comm_swf_url":"http:\/\/static.ak.fbcdn.net\/swf\/XdComm.swf?6:132011","login_img_dark_small_short":"http:\/\/static.ak.fbcdn.net\/images\/fbconnect\/login-buttons\/connect_dark_small_short.gif?6:121638","login_img_dark_medium_short":"http:\/\/static.ak.fbcdn.net\/images\/fbconnect\/login-buttons\/connect_dark_medium_short.gif?6:121638","login_img_dark_medium_long":"http:\/\/static.ak.fbcdn.net\/images\/fbconnect\/login-buttons\/connect_dark_medium_long.gif?6:121638","login_img_dark_large_short":"http:\/\/static.ak.fbcdn.net\/images\/fbconnect\/login-buttons\/connect_dark_large_short.gif?6:121776","login_img_dark_large_long":"http:\/\/static.ak.fbcdn.net\/images\/fbconnect\/login-buttons\/connect_dark_large_long.gif?6:121638","login_img_light_small_short":"http:\/\/static.ak.fbcdn.net\/images\/fbconnect\/login-buttons\/connect_light_small_short.gif?6:121638","login_img_light_medium_short":"http:\/\/static.ak.fbcdn.net\/images\/fbconnect\/login-buttons\/connect_light_medium_short.gif?6:121776","login_img_light_medium_long":"http:\/\/static.ak.fbcdn.net\/images\/fbconnect\/login-buttons\/connect_light_medium_long.gif?6:121776","login_img_light_large_short":"http:\/\/static.ak.fbcdn.net\/images\/fbconnect\/login-buttons\/connect_light_large_short.gif?6:121776","login_img_light_large_long":"http:\/\/static.ak.fbcdn.net\/images\/fbconnect\/login-buttons\/connect_light_large_long.gif?6:121638","login_img_white_small_short":"http:\/\/static.ak.fbcdn.net\/images\/fbconnect\/login-buttons\/connect_white_small_short.gif?6:121638","login_img_white_medium_short":"http:\/\/static.ak.fbcdn.net\/images\/fbconnect\/login-buttons\/connect_white_medium_short.gif?6:121638","login_img_white_medium_long":"http:\/\/static.ak.fbcdn.net\/images\/fbconnect\/login-buttons\/connect_white_medium_long.gif?6:121638","login_img_white_large_short":"http:\/\/static.ak.fbcdn.net\/images\/fbconnect\/login-buttons\/connect_white_large_short.gif?6:121638","login_img_white_large_long":"http:\/\/static.ak.fbcdn.net\/images\/fbconnect\/login-buttons\/connect_white_large_long.gif?6:121638","logout_img_small":"http:\/\/static.ak.fbcdn.net\/images\/fbconnect\/logout-buttons\/logout_small.gif?6:132011","logout_img_medium":"http:\/\/static.ak.fbcdn.net\/images\/fbconnect\/logout-buttons\/logout_medium.gif?6:132011","logout_img_large":"http:\/\/static.ak.fbcdn.net\/images\/fbconnect\/logout-buttons\/logout_large.gif?6:132011"});