<?php

/**
 * @file
 * Encode HTML entities.
 */

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_html_entity_encode_form($importer, $element_key, $settings) {
  $form = array();
  $form['html'] = array(
    '#markup' => t('This will convert all HTML special characters such as &gt; and &amp; to &amp;gt; and &amp;apm;.'),
  );
  return $form;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_html_entity_encode_callback($result, $item_key, $element_key, &$field, $settings, $source) {
  $field = check_plain($field);
}
