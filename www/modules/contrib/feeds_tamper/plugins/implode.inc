<?php

/**
 * @file
 * Convery an array to a string.
 */

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_implode_form($importer, $element_key, $settings) {
  $form = array();
  $form['glue'] = array(
    '#type' => 'textfield',
    '#title' => t('String glue'),
    '#default_value' => isset($settings['glue']) ? $settings['glue'] : ',',
    '#description' => t('Join array elements into a string. For example, array(\'a\', \'b\', \'c\') would become "a, b, c".
                        A space can be represented by %s, tabs by %t, and newlines by %n.'),
  );
  return $form;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_implode_validate(&$settings) {
  $settings['real_glue'] = str_replace(array('%s', '%t', '%n'), array(' ', "\t", "\n"), $settings['glue']);
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_implode_callback($result, $item_key, $element_key, &$field, $settings, $source) {
  $field = implode($settings['real_glue'], $field);
}
