<?php

/**
 * @file
 * Format a string with printf format.
 */

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_sprintf_form($importer, $element_key, $settings) {
  $form = array();
  $form['format'] = array(
    '#type' => 'textfield',
    '#title' => t('Format'),
    '#default_value' => isset($settings['format']) ? $settings['format'] : '%s',
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => FALSE,
    '#description' => t('See the <a href="http://www.php.net/manual/en/function.sprintf.php">sprintf</a> documentation for more details.'),
  );
  return $form;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_sprintf_callback($result, $item_key, $element_key, &$field, $settings, $source) {
  $field = sprintf($settings['format'], $field);
}
