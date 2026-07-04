<?php
/**
 * @file
 * Provide views data and handlers for paragraphs_library.module.
 *
 * @ingroup views_module_handlers
 */

/**
 * Implements hook_views_data_alter().
 *
 * Defines custom views field handlers to view, edit and delete.
 */
function paragraphs_library_views_data_alter(&$data) {

  $data['paragraphs_library_item']['view_paragraphs_library'] = [
    'field' => [
      'title' => t('Link to Paragraphs library item'),
      'help' => t('Provide a simple link to the Paragraphs library item.'),
      'handler' => 'views_handler_field_paragraphs_library_link',
    ],
  ];

  $data['paragraphs_library_item']['edit_paragraphs_library'] = [
    'field' => [
      'title' => t('Link to edit Paragraphs library item'),
      'help' => t('Provide a simple link to edit the Paragraphs library item.'),
      'handler' => 'views_handler_field_paragraphs_library_link_edit',
    ],
  ];

  $data['paragraphs_library_item']['delete_paragraphs_library'] = [
    'field' => [
      'title' => t('Link to delete Paragraphs library item'),
      'help' => t('Provide a simple link to delete the Paragraphs library item.'),
      'handler' => 'views_handler_field_paragraphs_library_link_delete',
    ],
  ];

  $data['paragraphs_library_item']['changed'] = [
    'title' => t('Updated date'),
    'help' => t('The date the content was last updated.'),
    'field' => [
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ],
    'sort' => [
      'handler' => 'views_handler_sort_date',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_date',
    ],
  ];
}
