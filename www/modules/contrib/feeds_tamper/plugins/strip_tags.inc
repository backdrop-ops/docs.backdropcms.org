<?php

/**
 * @file
 * Strip HTML tags.
 */

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_strip_tags_form($importer, $element_key, $settings) {
  $form = array();
  $form['help']['#markup'] = t('Remove all HTML tags except:');
  $form['allowed_tags'] = array(
    '#type' => 'textfield',
    '#title' => t('Allowed tags'),
    '#default_value' => isset($settings['allowed_tags']) ? $settings['allowed_tags'] : '',
    '#description' => t('A list of allowed tags, such as %a%b.', array('%a' => '<a>', '%b' => '<em>')),
  );
  return $form;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_strip_tags_validate(&$settings) {
  $settings['allowed_tags'] = trim($settings['allowed_tags']);
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_strip_tags_callback($result, $item_key, $element_key, &$field, $settings, $source) {
  $field = strip_tags($field, $settings['allowed_tags']);
}
