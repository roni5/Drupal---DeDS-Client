<?php
/**
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

/**
 * @file
 * Form builder; Displays DeDS API administration settings
 * page.
 *
 * @see system_settings_form()
 * @author Ben Bradshaw - ben@catalyst.net.nz
 */
function deds_client_admin_settings() {
  $form = array();

  $form['deds_client_api_uri'] = array(
    '#type'        => 'textfield',
    '#title'       => t('DeDS URI'),
    '#description' => t('The URI to use for the Device Detection API'),
    '#required'    => TRUE,
    '#size'        => 32,
    '#maxlength'   => 255,
    '#default_value' => variable_get('deds_client_api_uri', ''),
  );

  $form['deds_client_cache_time'] = array(
    '#type'        => 'textfield',
    '#title'       => t('Cache time (seconds)'),
    '#description' => t('How long should we remember the information DeDS gives us?'),
    '#required'    => TRUE,
    '#size'        => 32,
    '#maxlength'   => 32,
    '#default_value' => variable_get('deds_client_cache_time', '86400'),
  );

  return system_settings_form($form);
}

