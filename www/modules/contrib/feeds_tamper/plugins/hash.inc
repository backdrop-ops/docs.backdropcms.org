<?php

/**
 * @file
 * Calculate hash of a feed item.
 */

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_hash_form($importer, $element_key, $settings) {
  return array(
    'help' => array(
      '#markup' => t('Makes the value a hash of the values being imported. This allows for importing content that does not have a unique ID.'),
    ),
    'override' => array(
      '#type' => 'checkbox',
      '#title' => t('Override set value'),
      '#description' => t('If checked, the existing value of this field will be overridden.'),
      '#default_value' => isset($settings['override']) ? $settings['override'] : FALSE,
    ),
  );
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_hash_callback($result, $item_key, $element_key, &$field, $settings, $source) {
  if ($settings['override'] || !trim($field)) {
    $field = md5(serialize($result->items[$item_key]));
  }
}
