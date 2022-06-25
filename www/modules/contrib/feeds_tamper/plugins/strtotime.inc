<?php

/**
 * @file
 * Convert string to timestamp.
 */

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_strtotime_form($importer, $element_key, $settings) {
  $form = array();
  $form['html'] = array(
    '#markup' => t('This will take a string containing an English date format and convert it into a Unix Timestamp.'),
  );
  return $form;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_strtotime_callback($result, $item_key, $element_key, &$field, $settings, $source) {
  $field = strtotime($field);
}
