<?php

/**
 * @file
 * Convert a country name to ISO code.
 */

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_country_to_code_form($importer, $element_key, $settings) {
  $form = array();
  $form['help'] = array(
    '#markup' => t('Converts this field from a country name string to the two character ISO 3166-1 alpha-2 code.'),
  );
  return $form;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_country_to_code_callback($result, $item_key, $element_key, &$field, $settings, $source) {
  include_once BACKDROP_ROOT . '/includes/locale.inc';
  static $countries = array();

  if (empty($countries)) {
    $countries = country_get_list();
    array_walk($countries, '_feeds_tamper_country_to_code_strtolower');
    $countries = array_flip($countries);
  }

  // If it's already a valid country code, leave it alone.
  if (in_array($field, $countries)) {
    return;
  }

  // Trim whitespace, set to lowercase.
  $country = t(backdrop_strtolower(trim($field)));
  if (isset($countries[$country])) {
    $field = $countries[$country];
  }
  else {
    // If country name cannot be found, return nothing.
    $field = '';
  }
}

/**
 * Callback for array_walk().
 */
function _feeds_tamper_country_to_code_strtolower(&$word) {
  $word = backdrop_strtolower($word);
}
