<?php
/*
 * DeDS Client - Device Detection Service module for Drupal 6
 * Copyright (C) 2011 Catalyst IT
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * @author Ben Bradshaw - ben@catalyst.net.nz
 */
class DedsClient {
  private $dds_uri = null;
  private $cache_lifetime = null; // We'll use Drupal cache for the results

  function __construct($dds_uri, $cache_lifetime) {
    $this->dds_uri = $dds_uri;
    $this->cache_lifetime = $cache_lifetime;
  }
  /*
   * The only user-facing function - provide a user agent and an array of capabilities you want
   * $user_agent - User Agent string of the device
   * $capabilities - An array of values to be queried
   */
  function query($user_agent, $capabilities) {
    $data = false;
    $all_capabilities = null;
    // Check cache - will only return data if it's not stale
    if ($cache_data = $this->_cache_get($user_agent)) {

      // Sadly, we have to make sure all the values asked for are already cached
      $all_capabilities = true;
      foreach($capabilities as $key) {
        // Note: If you request a value DeDS can't return, then it will always not be found
        // and will have to query each time
        if (!isset($cache_data->$key)) {
          $all_capabilities = false;
          break;
        }
      }

      if ($all_capabilities) {
        return $cache_data;
      }
      else {
        // We could have _some_ of the info, but not all of it
        // Do a DeDS query and add the result to the cache (using $all_capabilities as the flag)
      }

    }
    // Else, fall through to the DeDS query
    $http_code = $this->_deds_get($user_agent, $capabilities, $json_response);

    if ($http_code == 200) {
      $data = json_decode($json_response);
      // Add to cache
      if ($all_capabilities === null) {
        $this->_cache_set(&$user_agent, $data);
      }
      else {
        $this->_cache_merge(&$user_agent, $data); // Add the new information to the existing cached record
      }
      return $data; // Still, only return the data we asked for
    }
    else {
      watchdog('deds_client', t('Error: DeDS returned with HTTP ' . $http_code));
      return false;
    }
  }

  private function _deds_get($user_agent, $capabilities, &$json_response = null) {
    $request_url = $this->dds_uri . 'get_capabilities?';

    // Encode the User Agent
    $headers = array('User-Agent' => $user_agent);
    $request_url .= 'headers='.urlencode(json_encode($headers));

    // Request the capabilities we want
    foreach ($capabilities as $name) {
      $request_url .= '&capability='.$name;
    }
    $ch = curl_init($request_url);
    curl_setopt($ch, CURLOPT_USERAGENT, 'curl');
    curl_setopt($ch, CURLOPT_TIMEOUT_MS, 2000);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    $json_response = curl_exec($ch);
    $http_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
    curl_close($ch);

    return $http_code;
  }


  private function _cache_set($user_agent, $data) {
    cache_set('deds_'.md5($user_agent), $data, 'cache', time() + $this->cache_lifetime);
  }

  private function _cache_merge($user_agent, $data) {
    if ($cache = cache_get('deds_'.md5($user_agent))) {
      if (time() < $cache->expire) {
        foreach ($data as $capability => $value) {
          if (!isset($cache->data->$capability)) {
            d("Adding $capability => $value");
            $cache->data->$capability = $value;
          }
        }
      }
      // Add back to the cache, but don't increment the expiry time, we don't want stale data piggybacking on fresh
      cache_set('deds_'.md5($user_agent), $cache->data, 'cache', $cache->expire);
    }
  }

  private function _cache_get($user_agent) {
    // The cache key is dds_ followed by the md5 hash of the user agent
    if ($cache = cache_get('deds_'.md5($user_agent))) {
      // Is the data older than the lifetime?
      if (time() > $cache->expire) {
        return false;
      }
      // If we get a cache hit and the data is valid, return it
      else {
        return $cache->data;
      }
    }
    // Cache miss, we need to query
    else {
      return false;
    }
  }

}

?>