<?php

/**
 * @file
 * Decode HTML entities.
 */

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_html_entity_decode_form($importer, $element_key, $settings) {
  $form = array();
  $form['html'] = array(
    '#markup' => t('Convert all HTML entities such as &amp;amp; and &amp;quot; to &amp; and &quot;.'),
  );
  return $form;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_html_entity_decode_callback($result, $item_key, $element_key, &$field, $settings, $source) {
  $field = html_entity_decode($field, ENT_QUOTES, 'UTF-8');
}
