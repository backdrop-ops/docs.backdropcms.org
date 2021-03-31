<?php

/**
 * @file
 * Convert text to upper, lower, or title case.
 */

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_convert_case_form($importer, $element_key, $settings) {
  $form = array();
  $form['mode'] = array(
    '#type' => 'radios',
    '#title' => t('How to convert case'),
    '#default_value' => isset($settings['mode']) ? $settings['mode'] : MB_CASE_TITLE,
    '#options' => array(
      MB_CASE_TITLE => t('Title Case'),
      MB_CASE_LOWER => t('lower case'),
      MB_CASE_UPPER => t('UPPER CASE'),
      'ucfirst' => t('Sentence case'),
    ),
  );
  return $form;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_convert_case_callback($result, $item_key, $element_key, &$field, $settings, $source) {
  if ($settings['mode'] === 'ucfirst') {
    $field = mb_strtoupper(mb_substr($field, 0, 1, 'UTF-8'), 'UTF-8') . mb_strtolower(mb_substr($field, 1, mb_strlen($field, 'UTF-8'), 'UTF-8'));
  }
  else {
    $field = mb_convert_case($field, $settings['mode'], 'UTF-8');
  }
}
