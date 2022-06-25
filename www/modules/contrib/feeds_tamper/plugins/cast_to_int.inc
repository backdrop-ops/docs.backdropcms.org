<?php

/**
 * @file
 * Cast the value to an integer.
 */

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_cast_to_int_form($importer, $element_key, $settings) {
  $form = array();
  $form['info'] = array(
    '#markup' => t('This plugin will convert any value to its integer form.'),
  );
  return $form;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_cast_to_int_callback($result, $item_key, $element_key, &$field, $settings) {
  $field = (int) $field;
}
