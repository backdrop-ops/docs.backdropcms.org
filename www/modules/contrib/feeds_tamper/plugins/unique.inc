<?php

/**
 * @file
 * Ensure a multi-valued field has unique values.
 */

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_unique_form($importer, $element_key, $settings) {
  $form = array();
  $form['unique']['#markup'] = t('Makes the elements in a multivalued field unique.');
  return $form;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_unique_callback($result, $item_key, $element_key, &$field, $settings, $source) {
  $unique = array();
  foreach ($field as $f) {
    if (!in_array($f, $unique)) {
      $unique[] = $f;
    }
  }
  $field = $unique;
}
