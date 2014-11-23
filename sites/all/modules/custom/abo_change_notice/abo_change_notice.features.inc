<?php
/**
 * @file
 * abo_change_notice.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function abo_change_notice_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_node_info().
 */
function abo_change_notice_node_info() {
  $items = array(
    'changenotice' => array(
      'name' => t('changenotice'),
      'base' => 'node_content',
      'description' => t('Reverse-engineered from https://www.drupal.org/node/add/changenotice

'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
