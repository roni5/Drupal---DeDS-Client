== About ==

DeDS is a Device Detection Service that uses your browser User Agent header to reveal the identity of your browser and the capabilities it has. It is designed to be used to identify mobile devices like iPhone/iOS, Android, Nokia and others using the WURFL library. You can run DeDS yourself or use a hosted DeDS.

This module is for developers who want to get up and running with Drupal and DeDS quickly and don't want to reinvent the wheel. It doesn't do any detection itself, instead it accepts a User Agent that it sends to a DeDS server and returns device information in a way PHP can understand.

The DeDS server is available on github: https://github.com/jkyamog/DeDS

== Download ==

This module is also available for download on Drupal.org: http://drupal.org/project/deds_client

== Installing ==

After enabling the module, visit /admin/settings/deds-client to configure your DeDS URL and cache lifetime. Your URL should end in the DeDS version, like http://{your DeDS URL}/dds/services/v1/

== Calling ==

Below is an example of how to use the module. The versions and browsers you deal with will be different, but this shows you how to query the device OS, OS version and browser.

Because DeDS uses wurfl you should have access to the capabilites wurfl stores. You can see a list at http://wurfl.sourceforge.net/help_doc.php

if (module_exists('deds_client')) {
  if ($dds_client = deds_client_get_client()) {

    $ua = $_SERVER['HTTP_USER_AGENT'];
    // Request the Device OS, OS version and type of browser
    // See http://wurfl.sourceforge.net/help_doc.php for a list of capabilities that can be queried
    $capabilities = $dds_client->query($ua, array('device_os', 'device_os_version', 'mobile_browser'));

    if ($capabilities != false) { // We receive false if DeDS is down & the cache is empty
      // We can check the OS and version of the device
      switch ($capabilities->device_os) {
        case 'iPhone OS':
          if ($capabilities->mobile_browser == 'Safari') {
            // Code for iPhone running Safari
          }
          else {
            // Code for iPhone not running Safari
          }
        break;
        case 'Android':
          if (version_compare($capabilities->device_os_version, '2.0', '>=')) {
            // Code for Android devices 2.0 and up
          }
        break;
        case '':
          // Most likely a Desktop client - PHP lets us use browser_info instead
          $browser = browser_info($ua);
          if ($browser['chrome'] && version_compare($browser['chrome'], '7.0', '>=')) {
            // Google Chrome version 7 and higher on a desktop
          }
        break;
      }
      else {
        watchdog('your_module', 'Unable to query DeDS for handset "' . $ua . '"');
      }
    }
  }
  else {
    watchdog('your_module', 'Unable to get DeDS client');
  }
}

Enjoy!

== Thanks ==
This module was sponsored by New Zealand Post and built by Catalyst IT Ltd.
