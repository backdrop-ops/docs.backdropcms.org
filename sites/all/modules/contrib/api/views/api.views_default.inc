<?php

/**
 * Default views for the API module, for lists of documentation items.
 */

/**
 * Implements hook_views_default_views().
 *
 * Defines views used in the API module for admin pages and user-facing pages.
 */
function api_views_default_views() {
  $views = array();

  // API Listings view.

  $view = new view();
  $view->name = 'api_listings';
  $view->description = 'Blocks for listing API documentation items. Used directly by the API module. Do not delete or disable.';
  $view->tag = 'default';
  $view->base_table = 'api_documentation';
  $view->human_name = 'API Listings';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'access API reference';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'title' => 'title',
    'file_name' => 'file_name',
    'object_type' => 'object_type',
    'summary' => 'summary',
  );
  $handler->display->display_options['style_options']['default'] = 'title';
  $handler->display->display_options['style_options']['info'] = array(
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'file_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'object_type' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'summary' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Field: API documentation: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Name';
  $handler->display->display_options['fields']['title']['link_to_docs'] = 'object';
  /* Field: API documentation: File name */
  $handler->display->display_options['fields']['file_name']['id'] = 'file_name';
  $handler->display->display_options['fields']['file_name']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['file_name']['field'] = 'file_name';
  $handler->display->display_options['fields']['file_name']['label'] = 'Location';
  $handler->display->display_options['fields']['file_name']['link_to_docs'] = 'file_base';
  /* Field: API documentation: Summary */
  $handler->display->display_options['fields']['summary']['id'] = 'summary';
  $handler->display->display_options['fields']['summary']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['summary']['field'] = 'summary';
  $handler->display->display_options['fields']['summary']['label'] = 'Description';
  /* Sort criterion: API documentation: Object name */
  $handler->display->display_options['sorts']['object_name']['id'] = 'object_name';
  $handler->display->display_options['sorts']['object_name']['table'] = 'api_documentation';
  $handler->display->display_options['sorts']['object_name']['field'] = 'object_name';
  /* Contextual filter: API documentation: Object type */
  $handler->display->display_options['arguments']['object_type']['id'] = 'object_type';
  $handler->display->display_options['arguments']['object_type']['table'] = 'api_documentation';
  $handler->display->display_options['arguments']['object_type']['field'] = 'object_type';
  $handler->display->display_options['arguments']['object_type']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['object_type']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['object_type']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['object_type']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['object_type']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['object_type']['limit'] = '0';
  /* Contextual filter: API documentation: Branch */
  $handler->display->display_options['arguments']['branch_id']['id'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['table'] = 'api_documentation';
  $handler->display->display_options['arguments']['branch_id']['field'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['branch_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['branch_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['branch_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['branch_id']['summary_options']['items_per_page'] = '25';

  /* Display: Default List Block */
  $handler = $view->new_display('block', 'Default List Block', 'block_listing');
  $handler->display->display_options['display_description'] = 'Block for listing functions, constants, etc.';
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: API documentation: Class documentation ID */
  $handler->display->display_options['filters']['class_did']['id'] = 'class_did';
  $handler->display->display_options['filters']['class_did']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['class_did']['field'] = 'class_did';
  $handler->display->display_options['filters']['class_did']['value']['value'] = '0';
  /* Filter criterion: API documentation: Object name */
  $handler->display->display_options['filters']['object_name']['id'] = 'object_name';
  $handler->display->display_options['filters']['object_name']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['object_name']['field'] = 'object_name';
  $handler->display->display_options['filters']['object_name']['operator'] = 'contains';
  $handler->display->display_options['filters']['object_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['object_name']['expose']['operator_id'] = 'object_name_op';
  $handler->display->display_options['filters']['object_name']['expose']['label'] = 'Name contains';
  $handler->display->display_options['filters']['object_name']['expose']['description'] = 'Contains filters are case sensitive';
  $handler->display->display_options['filters']['object_name']['expose']['operator'] = 'object_name_op';
  $handler->display->display_options['filters']['object_name']['expose']['identifier'] = 'object_name';
  $handler->display->display_options['filters']['object_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: API documentation: Summary */
  $handler->display->display_options['filters']['summary']['id'] = 'summary';
  $handler->display->display_options['filters']['summary']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['summary']['field'] = 'summary';
  $handler->display->display_options['filters']['summary']['operator'] = 'contains';
  $handler->display->display_options['filters']['summary']['exposed'] = TRUE;
  $handler->display->display_options['filters']['summary']['expose']['operator_id'] = 'summary_op';
  $handler->display->display_options['filters']['summary']['expose']['label'] = 'Description contains';
  $handler->display->display_options['filters']['summary']['expose']['operator'] = 'summary_op';
  $handler->display->display_options['filters']['summary']['expose']['identifier'] = 'summary';
  $handler->display->display_options['filters']['summary']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: File List Block */
  $handler = $view->new_display('block', 'File List Block', 'block_files');
  $handler->display->display_options['display_description'] = 'Block for listing files';
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'title' => 'title',
    'object_name' => 'object_name',
    'namespace' => 'namespace',
    'summary' => 'summary',
  );
  $handler->display->display_options['style_options']['default'] = 'title';
  $handler->display->display_options['style_options']['info'] = array(
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'object_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'namespace' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'summary' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['relationships'] = FALSE;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: API documentation: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'File name';
  /* Field: API documentation: Object name */
  $handler->display->display_options['fields']['object_name']['id'] = 'object_name';
  $handler->display->display_options['fields']['object_name']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['object_name']['field'] = 'object_name';
  $handler->display->display_options['fields']['object_name']['label'] = 'Location';
  $handler->display->display_options['fields']['object_name']['link_to_docs'] = 'object';
  /* Field: API documentation: Namespace */
  $handler->display->display_options['fields']['namespace']['id'] = 'namespace';
  $handler->display->display_options['fields']['namespace']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['namespace']['field'] = 'namespace';
  /* Field: API documentation: Summary */
  $handler->display->display_options['fields']['summary']['id'] = 'summary';
  $handler->display->display_options['fields']['summary']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['summary']['field'] = 'summary';
  $handler->display->display_options['fields']['summary']['label'] = 'Description';
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: API documentation: Branch */
  $handler->display->display_options['arguments']['branch_id']['id'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['table'] = 'api_documentation';
  $handler->display->display_options['arguments']['branch_id']['field'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['branch_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['branch_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['branch_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['branch_id']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: API documentation: Object type */
  $handler->display->display_options['filters']['object_type']['id'] = 'object_type';
  $handler->display->display_options['filters']['object_type']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['object_type']['field'] = 'object_type';
  $handler->display->display_options['filters']['object_type']['value'] = 'file';
  $handler->display->display_options['filters']['object_type']['group'] = 1;
  /* Filter criterion: API documentation: Object name */
  $handler->display->display_options['filters']['object_name']['id'] = 'object_name';
  $handler->display->display_options['filters']['object_name']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['object_name']['field'] = 'object_name';
  $handler->display->display_options['filters']['object_name']['operator'] = 'contains';
  $handler->display->display_options['filters']['object_name']['group'] = 1;
  $handler->display->display_options['filters']['object_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['object_name']['expose']['operator_id'] = 'object_name_op';
  $handler->display->display_options['filters']['object_name']['expose']['label'] = 'Location contains';
  $handler->display->display_options['filters']['object_name']['expose']['description'] = 'Contains filters are case sensitive';
  $handler->display->display_options['filters']['object_name']['expose']['operator'] = 'object_name_op';
  $handler->display->display_options['filters']['object_name']['expose']['identifier'] = 'object_name';
  $handler->display->display_options['filters']['object_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: API documentation: Namespace */
  $handler->display->display_options['filters']['namespace']['id'] = 'namespace';
  $handler->display->display_options['filters']['namespace']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['namespace']['field'] = 'namespace';
  $handler->display->display_options['filters']['namespace']['operator'] = 'contains';
  $handler->display->display_options['filters']['namespace']['group'] = 1;
  $handler->display->display_options['filters']['namespace']['exposed'] = TRUE;
  $handler->display->display_options['filters']['namespace']['expose']['operator_id'] = 'namespace_op';
  $handler->display->display_options['filters']['namespace']['expose']['label'] = 'Namespace contains';
  $handler->display->display_options['filters']['namespace']['expose']['operator'] = 'namespace_op';
  $handler->display->display_options['filters']['namespace']['expose']['identifier'] = 'namespace';
  $handler->display->display_options['filters']['namespace']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: API documentation: Summary */
  $handler->display->display_options['filters']['summary']['id'] = 'summary';
  $handler->display->display_options['filters']['summary']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['summary']['field'] = 'summary';
  $handler->display->display_options['filters']['summary']['operator'] = 'contains';
  $handler->display->display_options['filters']['summary']['group'] = 1;
  $handler->display->display_options['filters']['summary']['exposed'] = TRUE;
  $handler->display->display_options['filters']['summary']['expose']['operator_id'] = 'summary_op';
  $handler->display->display_options['filters']['summary']['expose']['label'] = 'Description contains';
  $handler->display->display_options['filters']['summary']['expose']['operator'] = 'summary_op';
  $handler->display->display_options['filters']['summary']['expose']['identifier'] = 'summary';
  $handler->display->display_options['filters']['summary']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Class List Block */
  $handler = $view->new_display('block', 'Class List Block', 'block_class');
  $handler->display->display_options['display_description'] = 'Block for listing classes and interfaces';
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'object_name' => 'object_name',
    'modifiers' => 'modifiers',
    'object_type' => 'object_type',
    'namespace' => 'namespace',
    'file_name' => 'file_name',
    'summary' => 'summary',
  );
  $handler->display->display_options['style_options']['default'] = 'object_name';
  $handler->display->display_options['style_options']['info'] = array(
    'object_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'modifiers' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'object_type' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'namespace' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'file_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'summary' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['relationships'] = FALSE;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: API documentation: Object name */
  $handler->display->display_options['fields']['object_name']['id'] = 'object_name';
  $handler->display->display_options['fields']['object_name']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['object_name']['field'] = 'object_name';
  $handler->display->display_options['fields']['object_name']['label'] = 'Name';
  $handler->display->display_options['fields']['object_name']['link_to_docs'] = 'object';
  /* Field: API documentation: Modifiers */
  $handler->display->display_options['fields']['modifiers']['id'] = 'modifiers';
  $handler->display->display_options['fields']['modifiers']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['modifiers']['field'] = 'modifiers';
  $handler->display->display_options['fields']['modifiers']['label'] = '';
  $handler->display->display_options['fields']['modifiers']['exclude'] = TRUE;
  $handler->display->display_options['fields']['modifiers']['element_label_colon'] = FALSE;
  /* Field: API documentation: Object type */
  $handler->display->display_options['fields']['object_type']['id'] = 'object_type';
  $handler->display->display_options['fields']['object_type']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['object_type']['field'] = 'object_type';
  $handler->display->display_options['fields']['object_type']['label'] = 'Type';
  $handler->display->display_options['fields']['object_type']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['object_type']['alter']['text'] = '[modifiers] [object_type]';
  $handler->display->display_options['fields']['object_type']['hide_alter_empty'] = FALSE;
  /* Field: API documentation: Namespace */
  $handler->display->display_options['fields']['namespace']['id'] = 'namespace';
  $handler->display->display_options['fields']['namespace']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['namespace']['field'] = 'namespace';
  /* Field: API documentation: File name */
  $handler->display->display_options['fields']['file_name']['id'] = 'file_name';
  $handler->display->display_options['fields']['file_name']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['file_name']['field'] = 'file_name';
  $handler->display->display_options['fields']['file_name']['label'] = 'Location';
  $handler->display->display_options['fields']['file_name']['link_to_docs'] = 'file_base';
  /* Field: API documentation: Summary */
  $handler->display->display_options['fields']['summary']['id'] = 'summary';
  $handler->display->display_options['fields']['summary']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['summary']['field'] = 'summary';
  $handler->display->display_options['fields']['summary']['label'] = 'Description';
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: API documentation: Branch */
  $handler->display->display_options['arguments']['branch_id']['id'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['table'] = 'api_documentation';
  $handler->display->display_options['arguments']['branch_id']['field'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['branch_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['branch_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['branch_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['branch_id']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['filter_groups']['groups'] = array(
    1 => 'AND',
    2 => 'OR',
  );
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: API documentation: Class documentation ID */
  $handler->display->display_options['filters']['class_did']['id'] = 'class_did';
  $handler->display->display_options['filters']['class_did']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['class_did']['field'] = 'class_did';
  $handler->display->display_options['filters']['class_did']['value']['value'] = '0';
  $handler->display->display_options['filters']['class_did']['group'] = 1;
  /* Filter criterion: API documentation: Object name */
  $handler->display->display_options['filters']['object_name']['id'] = 'object_name';
  $handler->display->display_options['filters']['object_name']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['object_name']['field'] = 'object_name';
  $handler->display->display_options['filters']['object_name']['operator'] = 'contains';
  $handler->display->display_options['filters']['object_name']['group'] = 1;
  $handler->display->display_options['filters']['object_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['object_name']['expose']['operator_id'] = 'object_name_op';
  $handler->display->display_options['filters']['object_name']['expose']['label'] = 'Name contains';
  $handler->display->display_options['filters']['object_name']['expose']['description'] = 'Contains filters are case sensitive';
  $handler->display->display_options['filters']['object_name']['expose']['operator'] = 'object_name_op';
  $handler->display->display_options['filters']['object_name']['expose']['identifier'] = 'object_name';
  $handler->display->display_options['filters']['object_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: API documentation: Namespace */
  $handler->display->display_options['filters']['namespace']['id'] = 'namespace';
  $handler->display->display_options['filters']['namespace']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['namespace']['field'] = 'namespace';
  $handler->display->display_options['filters']['namespace']['operator'] = 'contains';
  $handler->display->display_options['filters']['namespace']['group'] = 1;
  $handler->display->display_options['filters']['namespace']['exposed'] = TRUE;
  $handler->display->display_options['filters']['namespace']['expose']['operator_id'] = 'namespace_op';
  $handler->display->display_options['filters']['namespace']['expose']['label'] = 'Namespace contains';
  $handler->display->display_options['filters']['namespace']['expose']['operator'] = 'namespace_op';
  $handler->display->display_options['filters']['namespace']['expose']['identifier'] = 'namespace';
  $handler->display->display_options['filters']['namespace']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: API documentation: Summary */
  $handler->display->display_options['filters']['summary']['id'] = 'summary';
  $handler->display->display_options['filters']['summary']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['summary']['field'] = 'summary';
  $handler->display->display_options['filters']['summary']['operator'] = 'contains';
  $handler->display->display_options['filters']['summary']['group'] = 1;
  $handler->display->display_options['filters']['summary']['exposed'] = TRUE;
  $handler->display->display_options['filters']['summary']['expose']['operator_id'] = 'summary_op';
  $handler->display->display_options['filters']['summary']['expose']['label'] = 'Description contains';
  $handler->display->display_options['filters']['summary']['expose']['operator'] = 'summary_op';
  $handler->display->display_options['filters']['summary']['expose']['identifier'] = 'summary';
  $handler->display->display_options['filters']['summary']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: API documentation: Object type */
  $handler->display->display_options['filters']['object_type']['id'] = 'object_type';
  $handler->display->display_options['filters']['object_type']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['object_type']['field'] = 'object_type';
  $handler->display->display_options['filters']['object_type']['value'] = 'class';
  $handler->display->display_options['filters']['object_type']['group'] = 2;
  /* Filter criterion: API documentation: Object type */
  $handler->display->display_options['filters']['object_type_1']['id'] = 'object_type_1';
  $handler->display->display_options['filters']['object_type_1']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['object_type_1']['field'] = 'object_type';
  $handler->display->display_options['filters']['object_type_1']['value'] = 'interface';
  $handler->display->display_options['filters']['object_type_1']['group'] = 2;

  /* Display: Topic List Block */
  $handler = $view->new_display('block', 'Topic List Block', 'block_groups');
  $handler->display->display_options['display_description'] = 'Block for listing groups/topics';
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'title' => 'title',
    'summary' => 'summary',
  );
  $handler->display->display_options['style_options']['default'] = 'title';
  $handler->display->display_options['style_options']['info'] = array(
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'summary' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: API documentation: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Name';
  $handler->display->display_options['fields']['title']['link_to_docs'] = 'object';
  /* Field: API documentation: Summary */
  $handler->display->display_options['fields']['summary']['id'] = 'summary';
  $handler->display->display_options['fields']['summary']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['summary']['field'] = 'summary';
  $handler->display->display_options['fields']['summary']['label'] = 'Description';
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: API documentation: Branch */
  $handler->display->display_options['arguments']['branch_id']['id'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['table'] = 'api_documentation';
  $handler->display->display_options['arguments']['branch_id']['field'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['branch_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['branch_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['branch_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['branch_id']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: API documentation: Class documentation ID */
  $handler->display->display_options['filters']['class_did']['id'] = 'class_did';
  $handler->display->display_options['filters']['class_did']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['class_did']['field'] = 'class_did';
  $handler->display->display_options['filters']['class_did']['value']['value'] = '0';
  /* Filter criterion: API documentation: Object type */
  $handler->display->display_options['filters']['object_type']['id'] = 'object_type';
  $handler->display->display_options['filters']['object_type']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['object_type']['field'] = 'object_type';
  $handler->display->display_options['filters']['object_type']['value'] = 'group';
  /* Filter criterion: API documentation: Object name */
  $handler->display->display_options['filters']['object_name']['id'] = 'object_name';
  $handler->display->display_options['filters']['object_name']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['object_name']['field'] = 'object_name';
  $handler->display->display_options['filters']['object_name']['operator'] = 'contains';
  $handler->display->display_options['filters']['object_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['object_name']['expose']['operator_id'] = 'object_name_op';
  $handler->display->display_options['filters']['object_name']['expose']['label'] = 'Name contains';
  $handler->display->display_options['filters']['object_name']['expose']['description'] = 'Contains filters are case sensitve';
  $handler->display->display_options['filters']['object_name']['expose']['operator'] = 'object_name_op';
  $handler->display->display_options['filters']['object_name']['expose']['identifier'] = 'object_name';
  $handler->display->display_options['filters']['object_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: API documentation: Summary */
  $handler->display->display_options['filters']['summary']['id'] = 'summary';
  $handler->display->display_options['filters']['summary']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['summary']['field'] = 'summary';
  $handler->display->display_options['filters']['summary']['operator'] = 'contains';
  $handler->display->display_options['filters']['summary']['exposed'] = TRUE;
  $handler->display->display_options['filters']['summary']['expose']['operator_id'] = 'summary_op';
  $handler->display->display_options['filters']['summary']['expose']['label'] = 'Description contains';
  $handler->display->display_options['filters']['summary']['expose']['operator'] = 'summary_op';
  $handler->display->display_options['filters']['summary']['expose']['identifier'] = 'summary';
  $handler->display->display_options['filters']['summary']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Items in File List Block */
  $handler = $view->new_display('block', 'Items in File List Block', 'block_items_file');
  $handler->display->display_options['display_description'] = 'Block for listing functions, constants, etc. in a particular file';
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: API documentation: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Name';
  $handler->display->display_options['fields']['title']['link_to_docs'] = 'object';
  /* Field: API documentation: Summary */
  $handler->display->display_options['fields']['summary']['id'] = 'summary';
  $handler->display->display_options['fields']['summary']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['summary']['field'] = 'summary';
  $handler->display->display_options['fields']['summary']['label'] = 'Description';
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: API documentation: File name */
  $handler->display->display_options['arguments']['file_name']['id'] = 'file_name';
  $handler->display->display_options['arguments']['file_name']['table'] = 'api_documentation';
  $handler->display->display_options['arguments']['file_name']['field'] = 'file_name';
  $handler->display->display_options['arguments']['file_name']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['file_name']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['file_name']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['file_name']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['file_name']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['file_name']['limit'] = '0';
  /* Contextual filter: API documentation: Object type */
  $handler->display->display_options['arguments']['object_type']['id'] = 'object_type';
  $handler->display->display_options['arguments']['object_type']['table'] = 'api_documentation';
  $handler->display->display_options['arguments']['object_type']['field'] = 'object_type';
  $handler->display->display_options['arguments']['object_type']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['object_type']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['object_type']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['object_type']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['object_type']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['object_type']['limit'] = '0';
  /* Contextual filter: API documentation: Branch */
  $handler->display->display_options['arguments']['branch_id']['id'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['table'] = 'api_documentation';
  $handler->display->display_options['arguments']['branch_id']['field'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['branch_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['branch_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['branch_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['branch_id']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: API documentation: Class documentation ID */
  $handler->display->display_options['filters']['class_did']['id'] = 'class_did';
  $handler->display->display_options['filters']['class_did']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['class_did']['field'] = 'class_did';
  $handler->display->display_options['filters']['class_did']['value']['value'] = '0';

  /* Display: Deprecated Block */
  $handler = $view->new_display('block', 'Deprecated Block', 'block_deprecated');
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: API documentation: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Name';
  $handler->display->display_options['fields']['title']['link_to_docs'] = 'object';
  /* Field: API documentation: File name */
  $handler->display->display_options['fields']['file_name']['id'] = 'file_name';
  $handler->display->display_options['fields']['file_name']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['file_name']['field'] = 'file_name';
  $handler->display->display_options['fields']['file_name']['label'] = 'Location';
  $handler->display->display_options['fields']['file_name']['link_to_docs'] = 'file_base';
  /* Field: API documentation: Object type */
  $handler->display->display_options['fields']['object_type']['id'] = 'object_type';
  $handler->display->display_options['fields']['object_type']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['object_type']['field'] = 'object_type';
  $handler->display->display_options['fields']['object_type']['label'] = 'Type';
  /* Field: API documentation: Summary */
  $handler->display->display_options['fields']['summary']['id'] = 'summary';
  $handler->display->display_options['fields']['summary']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['summary']['field'] = 'summary';
  $handler->display->display_options['fields']['summary']['label'] = 'Description';
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: API documentation: Branch */
  $handler->display->display_options['arguments']['branch_id']['id'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['table'] = 'api_documentation';
  $handler->display->display_options['arguments']['branch_id']['field'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['branch_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['branch_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['branch_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['branch_id']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: API documentation: Deprecated */
  $handler->display->display_options['filters']['deprecated']['id'] = 'deprecated';
  $handler->display->display_options['filters']['deprecated']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['deprecated']['field'] = 'deprecated';
  $handler->display->display_options['filters']['deprecated']['value'] = '1';
  /* Filter criterion: API documentation: Object type */
  $handler->display->display_options['filters']['object_type']['id'] = 'object_type';
  $handler->display->display_options['filters']['object_type']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['object_type']['field'] = 'object_type';
  $handler->display->display_options['filters']['object_type']['exposed'] = TRUE;
  $handler->display->display_options['filters']['object_type']['expose']['operator_id'] = 'object_type_op';
  $handler->display->display_options['filters']['object_type']['expose']['label'] = 'Type';
  $handler->display->display_options['filters']['object_type']['expose']['operator'] = 'object_type_op';
  $handler->display->display_options['filters']['object_type']['expose']['identifier'] = 'type';
  $handler->display->display_options['filters']['object_type']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  $views[$view->name] = $view;

  // API Projects view.

  $view = new view();
  $view->name = 'api_projects';
  $view->description = 'Blocks for listing projects in the API module. Used directly by the API module. Do not delete or disable.';
  $view->tag = 'default';
  $view->base_table = 'api_project';
  $view->human_name = 'API Projects';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'access API reference';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['style_plugin'] = 'list';
  $handler->display->display_options['row_plugin'] = 'fields';
  /* Field: API project: Project title */
  $handler->display->display_options['fields']['project_title']['id'] = 'project_title';
  $handler->display->display_options['fields']['project_title']['table'] = 'api_project';
  $handler->display->display_options['fields']['project_title']['field'] = 'project_title';
  $handler->display->display_options['fields']['project_title']['label'] = '';
  $handler->display->display_options['fields']['project_title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['project_title']['link_to_project'] = 'project';
  /* Sort criterion: API project: Project title */
  $handler->display->display_options['sorts']['project_title']['id'] = 'project_title';
  $handler->display->display_options['sorts']['project_title']['table'] = 'api_project';
  $handler->display->display_options['sorts']['project_title']['field'] = 'project_title';

  /* Display: Project List Block */
  $handler = $view->new_display('block', 'Project List Block', 'block_project_list');
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'list';
  $handler->display->display_options['style_options']['wrapper_class'] = 'api-project-links';
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['defaults']['row_options'] = FALSE;

  /* Display: Project Admin Block */
  $handler = $view->new_display('block', 'Project Admin Block', 'block_project_admin');
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['defaults']['access'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer API reference';
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'project_name' => 'project_name',
    'project_title' => 'project_title',
    'project_type' => 'project_type',
  );
  $handler->display->display_options['style_options']['default'] = 'project_name';
  $handler->display->display_options['style_options']['info'] = array(
    'project_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'project_title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'project_type' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['empty'] = FALSE;
  /* No results behavior: Global: Unfiltered text */
  $handler->display->display_options['empty']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['empty']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['label'] = 'No matches';
  $handler->display->display_options['empty']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['empty']['area_text_custom']['content'] = 'No projects match your filters.';
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: API project: Project name */
  $handler->display->display_options['fields']['project_name']['id'] = 'project_name';
  $handler->display->display_options['fields']['project_name']['table'] = 'api_project';
  $handler->display->display_options['fields']['project_name']['field'] = 'project_name';
  $handler->display->display_options['fields']['project_name']['label'] = 'Project identifier';
  /* Field: API project: Project title */
  $handler->display->display_options['fields']['project_title']['id'] = 'project_title';
  $handler->display->display_options['fields']['project_title']['table'] = 'api_project';
  $handler->display->display_options['fields']['project_title']['field'] = 'project_title';
  $handler->display->display_options['fields']['project_title']['label'] = 'Title';
  $handler->display->display_options['fields']['project_title']['element_label_colon'] = FALSE;
  /* Field: API project: Project type */
  $handler->display->display_options['fields']['project_type']['id'] = 'project_type';
  $handler->display->display_options['fields']['project_type']['table'] = 'api_project';
  $handler->display->display_options['fields']['project_type']['field'] = 'project_type';
  $handler->display->display_options['fields']['project_type']['label'] = 'Type';
  /* Field: API project: Project operations */
  $handler->display->display_options['fields']['project_ops']['id'] = 'project_ops';
  $handler->display->display_options['fields']['project_ops']['table'] = 'api_project';
  $handler->display->display_options['fields']['project_ops']['field'] = 'project_ops';
  $handler->display->display_options['fields']['project_ops']['label'] = 'Operations';
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: API project: Project title */
  $handler->display->display_options['sorts']['project_title']['id'] = 'project_title';
  $handler->display->display_options['sorts']['project_title']['table'] = 'api_project';
  $handler->display->display_options['sorts']['project_title']['field'] = 'project_title';
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: API project: Project type */
  $handler->display->display_options['filters']['project_type']['id'] = 'project_type';
  $handler->display->display_options['filters']['project_type']['table'] = 'api_project';
  $handler->display->display_options['filters']['project_type']['field'] = 'project_type';
  $handler->display->display_options['filters']['project_type']['exposed'] = TRUE;
  $handler->display->display_options['filters']['project_type']['expose']['operator_id'] = 'project_type_op';
  $handler->display->display_options['filters']['project_type']['expose']['label'] = 'Filter by project type';
  $handler->display->display_options['filters']['project_type']['expose']['operator'] = 'project_type_op';
  $handler->display->display_options['filters']['project_type']['expose']['identifier'] = 'project_type';
  $handler->display->display_options['filters']['project_type']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  $handler->display->display_options['filters']['project_type']['group_info']['label'] = 'Project type';
  $handler->display->display_options['filters']['project_type']['group_info']['identifier'] = 'project_type';
  $handler->display->display_options['filters']['project_type']['group_info']['remember'] = FALSE;
  $handler->display->display_options['filters']['project_type']['group_info']['group_items'] = array(
    1 => array(),
    2 => array(),
    3 => array(),
  );

  $views[$view->name] = $view;

  // API Branches view.

  $view = new view();
  $view->name = 'api_branches';
  $view->description = 'Blocks for listing branches in the API module. Used directly by the API module. Do not delete or disable.';
  $view->tag = 'default';
  $view->base_table = 'api_branch';
  $view->human_name = 'API Branches';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer API reference';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'project_name' => 'project_name',
    'core_compatibility' => 'core_compatibility',
    'title' => 'title',
    'branch_name' => 'branch_name',
  );
  $handler->display->display_options['style_options']['default'] = 'project_name';
  $handler->display->display_options['style_options']['info'] = array(
    'project_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'core_compatibility' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'branch_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Relationship: API documentation: Project */
  $handler->display->display_options['relationships']['project']['id'] = 'project';
  $handler->display->display_options['relationships']['project']['table'] = 'api_branch';
  $handler->display->display_options['relationships']['project']['field'] = 'project';
  $handler->display->display_options['relationships']['project']['required'] = TRUE;
  /* Field: API project: Project name */
  $handler->display->display_options['fields']['project_name']['id'] = 'project_name';
  $handler->display->display_options['fields']['project_name']['table'] = 'api_project';
  $handler->display->display_options['fields']['project_name']['field'] = 'project_name';
  $handler->display->display_options['fields']['project_name']['relationship'] = 'project';
  $handler->display->display_options['fields']['project_name']['label'] = 'Project';
  /* Field: API documentation: Core compatibility */
  $handler->display->display_options['fields']['core_compatibility']['id'] = 'core_compatibility';
  $handler->display->display_options['fields']['core_compatibility']['table'] = 'api_branch';
  $handler->display->display_options['fields']['core_compatibility']['field'] = 'core_compatibility';
  /* Field: API documentation: Branch title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'api_branch';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  /* Field: API documentation: Branch name */
  $handler->display->display_options['fields']['branch_name']['id'] = 'branch_name';
  $handler->display->display_options['fields']['branch_name']['table'] = 'api_branch';
  $handler->display->display_options['fields']['branch_name']['field'] = 'branch_name';
  $handler->display->display_options['fields']['branch_name']['label'] = 'Branch identifier';
  /* Field: API documentation: Branch operations */
  $handler->display->display_options['fields']['branch_ops']['id'] = 'branch_ops';
  $handler->display->display_options['fields']['branch_ops']['table'] = 'api_branch';
  $handler->display->display_options['fields']['branch_ops']['field'] = 'branch_ops';
  $handler->display->display_options['fields']['branch_ops']['label'] = 'Operations';
  /* Filter criterion: API project: Project name */
  $handler->display->display_options['filters']['project_name']['id'] = 'project_name';
  $handler->display->display_options['filters']['project_name']['table'] = 'api_project';
  $handler->display->display_options['filters']['project_name']['field'] = 'project_name';
  $handler->display->display_options['filters']['project_name']['relationship'] = 'project';
  $handler->display->display_options['filters']['project_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['project_name']['expose']['operator_id'] = 'project_name_op';
  $handler->display->display_options['filters']['project_name']['expose']['label'] = 'Filter by project';
  $handler->display->display_options['filters']['project_name']['expose']['operator'] = 'project_name_op';
  $handler->display->display_options['filters']['project_name']['expose']['identifier'] = 'project_name';
  $handler->display->display_options['filters']['project_name']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Branch Admin Block */
  $handler = $view->new_display('block', 'Branch Admin Block', 'block_branch_admin');
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;

  $views[$view->name] = $view;

  // API Namespaces view.

  $view = new view();
  $view->name = 'api_namespaces';
  $view->description = 'Blocks for listing API namespaces. Used directly by the API module. Do not delete or disable.';
  $view->tag = 'default';
  $view->base_table = 'api_documentation';
  $view->human_name = 'API Namespaces';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'access API reference';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['distinct'] = TRUE;
  $handler->display->display_options['query']['options']['pure_distinct'] = TRUE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'object_name' => 'object_name',
    'file_name' => 'file_name',
  );
  $handler->display->display_options['style_options']['default'] = 'object_name';
  $handler->display->display_options['style_options']['info'] = array(
    'object_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'file_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Field: API documentation: Namespace */
  $handler->display->display_options['fields']['namespace']['id'] = 'namespace';
  $handler->display->display_options['fields']['namespace']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['namespace']['field'] = 'namespace';
  $handler->display->display_options['fields']['namespace']['link_to_namespace'] = 'object';
  /* Sort criterion: API documentation: Namespace */
  $handler->display->display_options['sorts']['namespace']['id'] = 'namespace';
  $handler->display->display_options['sorts']['namespace']['table'] = 'api_documentation';
  $handler->display->display_options['sorts']['namespace']['field'] = 'namespace';
  /* Contextual filter: API documentation: Branch */
  $handler->display->display_options['arguments']['branch_id']['id'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['table'] = 'api_documentation';
  $handler->display->display_options['arguments']['branch_id']['field'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['branch_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['branch_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['branch_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['branch_id']['summary_options']['items_per_page'] = '25';
  /* Filter criterion: API documentation: Namespace */
  $handler->display->display_options['filters']['namespace']['id'] = 'namespace';
  $handler->display->display_options['filters']['namespace']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['namespace']['field'] = 'namespace';
  $handler->display->display_options['filters']['namespace']['operator'] = 'longerthan';
  $handler->display->display_options['filters']['namespace']['value'] = '2';
  /* Filter criterion: API documentation: Namespace */
  $handler->display->display_options['filters']['namespace_1']['id'] = 'namespace_1';
  $handler->display->display_options['filters']['namespace_1']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['namespace_1']['field'] = 'namespace';
  $handler->display->display_options['filters']['namespace_1']['operator'] = 'contains';
  $handler->display->display_options['filters']['namespace_1']['exposed'] = TRUE;
  $handler->display->display_options['filters']['namespace_1']['expose']['operator_id'] = 'namespace_1_op';
  $handler->display->display_options['filters']['namespace_1']['expose']['label'] = 'Namespace contains';
  $handler->display->display_options['filters']['namespace_1']['expose']['description'] = 'Contains filters are case sensitive';
  $handler->display->display_options['filters']['namespace_1']['expose']['operator'] = 'namespace_1_op';
  $handler->display->display_options['filters']['namespace_1']['expose']['identifier'] = 'namespace_1';
  $handler->display->display_options['filters']['namespace_1']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Namespace List Block */
  $handler = $view->new_display('block', 'Namespace List Block', 'block_namespace_list');
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;

  /* Display: Items in Namespace List Block */
  $handler = $view->new_display('block', 'Items in Namespace List Block', 'block_items_namespace');
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: API documentation: Object name */
  $handler->display->display_options['fields']['object_name']['id'] = 'object_name';
  $handler->display->display_options['fields']['object_name']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['object_name']['field'] = 'object_name';
  $handler->display->display_options['fields']['object_name']['label'] = 'Class';
  $handler->display->display_options['fields']['object_name']['link_to_docs'] = 'object';
  /* Field: API documentation: File name */
  $handler->display->display_options['fields']['file_name']['id'] = 'file_name';
  $handler->display->display_options['fields']['file_name']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['file_name']['field'] = 'file_name';
  $handler->display->display_options['fields']['file_name']['label'] = 'Location';
  /* Field: API documentation: Summary */
  $handler->display->display_options['fields']['summary']['id'] = 'summary';
  $handler->display->display_options['fields']['summary']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['summary']['field'] = 'summary';
  $handler->display->display_options['fields']['summary']['label'] = 'Description';
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: API documentation: Object name */
  $handler->display->display_options['sorts']['object_name']['id'] = 'object_name';
  $handler->display->display_options['sorts']['object_name']['table'] = 'api_documentation';
  $handler->display->display_options['sorts']['object_name']['field'] = 'object_name';
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: API documentation: Branch */
  $handler->display->display_options['arguments']['branch_id']['id'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['table'] = 'api_documentation';
  $handler->display->display_options['arguments']['branch_id']['field'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['branch_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['branch_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['branch_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['branch_id']['summary_options']['items_per_page'] = '25';
  /* Contextual filter: API documentation: Namespace */
  $handler->display->display_options['arguments']['namespace']['id'] = 'namespace';
  $handler->display->display_options['arguments']['namespace']['table'] = 'api_documentation';
  $handler->display->display_options['arguments']['namespace']['field'] = 'namespace';
  $handler->display->display_options['arguments']['namespace']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['namespace']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['namespace']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['namespace']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['namespace']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['namespace']['limit'] = '0';
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['filter_groups']['groups'] = array(
    1 => 'OR',
  );
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: API documentation: Object type */
  $handler->display->display_options['filters']['object_type']['id'] = 'object_type';
  $handler->display->display_options['filters']['object_type']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['object_type']['field'] = 'object_type';
  $handler->display->display_options['filters']['object_type']['value'] = 'class';
  $handler->display->display_options['filters']['object_type']['group'] = 1;
  /* Filter criterion: API documentation: Object type */
  $handler->display->display_options['filters']['object_type_1']['id'] = 'object_type_1';
  $handler->display->display_options['filters']['object_type_1']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['object_type_1']['field'] = 'object_type';
  $handler->display->display_options['filters']['object_type_1']['value'] = 'interface';
  $handler->display->display_options['filters']['object_type_1']['group'] = 1;

  $views[$view->name] = $view;

  // API Members view.

  $view = new view();
  $view->name = 'api_members';
  $view->description = 'Blocks for listing API class members. Used directly by the API module. Do not delete or disable.';
  $view->tag = 'default';
  $view->base_table = 'api_members';
  $view->human_name = 'API Members';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'access API reference';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'modifiers' => 'modifiers',
    'title' => 'title',
    'object_type' => 'object_type',
    'summary' => 'summary',
    'title_1' => 'summary',
  );
  $handler->display->display_options['style_options']['default'] = 'title';
  $handler->display->display_options['style_options']['info'] = array(
    'modifiers' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'object_type' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'summary' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => ' ',
      'empty_column' => 0,
    ),
    'title_1' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Relationship: API documentation: Documentation object of member item */
  $handler->display->display_options['relationships']['did']['id'] = 'did';
  $handler->display->display_options['relationships']['did']['table'] = 'api_members';
  $handler->display->display_options['relationships']['did']['field'] = 'did';
  $handler->display->display_options['relationships']['did']['label'] = 'Item';
  $handler->display->display_options['relationships']['did']['required'] = TRUE;
  /* Relationship: API documentation: Overrides information */
  $handler->display->display_options['relationships']['override_did']['id'] = 'override_did';
  $handler->display->display_options['relationships']['override_did']['table'] = 'api_documentation';
  $handler->display->display_options['relationships']['override_did']['field'] = 'override_did';
  $handler->display->display_options['relationships']['override_did']['relationship'] = 'did';
  $handler->display->display_options['relationships']['override_did']['label'] = 'Overrides';
  /* Relationship: API documentation: Documentation object of member being overridden */
  $handler->display->display_options['relationships']['overrides_did']['id'] = 'overrides_did';
  $handler->display->display_options['relationships']['overrides_did']['table'] = 'api_overrides';
  $handler->display->display_options['relationships']['overrides_did']['field'] = 'overrides_did';
  $handler->display->display_options['relationships']['overrides_did']['relationship'] = 'override_did';
  $handler->display->display_options['relationships']['overrides_did']['label'] = 'Overridden item';
  /* Field: API documentation: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['relationship'] = 'did';
  $handler->display->display_options['fields']['title']['label'] = 'Name';
  $handler->display->display_options['fields']['title']['link_to_docs'] = 'object';
  /* Field: API documentation: Modifiers */
  $handler->display->display_options['fields']['modifiers']['id'] = 'modifiers';
  $handler->display->display_options['fields']['modifiers']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['modifiers']['field'] = 'modifiers';
  $handler->display->display_options['fields']['modifiers']['relationship'] = 'did';
  /* Field: API documentation: Object type */
  $handler->display->display_options['fields']['object_type']['id'] = 'object_type';
  $handler->display->display_options['fields']['object_type']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['object_type']['field'] = 'object_type';
  $handler->display->display_options['fields']['object_type']['relationship'] = 'did';
  $handler->display->display_options['fields']['object_type']['label'] = 'Type';
  /* Field: API documentation: Summary */
  $handler->display->display_options['fields']['summary']['id'] = 'summary';
  $handler->display->display_options['fields']['summary']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['summary']['field'] = 'summary';
  $handler->display->display_options['fields']['summary']['relationship'] = 'did';
  $handler->display->display_options['fields']['summary']['label'] = 'Description';
  /* Field: API documentation: Title */
  $handler->display->display_options['fields']['title_1']['id'] = 'title_1';
  $handler->display->display_options['fields']['title_1']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['title_1']['field'] = 'title';
  $handler->display->display_options['fields']['title_1']['relationship'] = 'overrides_did';
  $handler->display->display_options['fields']['title_1']['label'] = '';
  $handler->display->display_options['fields']['title_1']['element_type'] = '0';
  $handler->display->display_options['fields']['title_1']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title_1']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['title_1']['link_to_docs'] = 'object';
  $handler->display->display_options['fields']['title_1']['prefix'] = '<em class="api-inheritance-source">Overrides ';
  $handler->display->display_options['fields']['title_1']['suffix'] = '</em>';
  /* Contextual filter: API documentation: Documentation ID of class for members */
  $handler->display->display_options['arguments']['class_did']['id'] = 'class_did';
  $handler->display->display_options['arguments']['class_did']['table'] = 'api_members';
  $handler->display->display_options['arguments']['class_did']['field'] = 'class_did';
  $handler->display->display_options['arguments']['class_did']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['class_did']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['class_did']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['class_did']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['class_did']['summary_options']['items_per_page'] = '25';
  /* Filter criterion: API documentation: Title */
  $handler->display->display_options['filters']['title']['id'] = 'title';
  $handler->display->display_options['filters']['title']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['title']['field'] = 'title';
  $handler->display->display_options['filters']['title']['relationship'] = 'did';
  $handler->display->display_options['filters']['title']['operator'] = 'contains';
  $handler->display->display_options['filters']['title']['group'] = 1;
  $handler->display->display_options['filters']['title']['exposed'] = TRUE;
  $handler->display->display_options['filters']['title']['expose']['operator_id'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['label'] = 'Name contains';
  $handler->display->display_options['filters']['title']['expose']['description'] = 'Contains filters are case sensitive';
  $handler->display->display_options['filters']['title']['expose']['operator'] = 'title_op';
  $handler->display->display_options['filters']['title']['expose']['identifier'] = 'title';
  $handler->display->display_options['filters']['title']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: API documentation: Title */
  $handler->display->display_options['filters']['title_1']['id'] = 'title_1';
  $handler->display->display_options['filters']['title_1']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['title_1']['field'] = 'title';
  $handler->display->display_options['filters']['title_1']['relationship'] = 'did';
  $handler->display->display_options['filters']['title_1']['operator'] = 'not';
  $handler->display->display_options['filters']['title_1']['group'] = 1;
  $handler->display->display_options['filters']['title_1']['exposed'] = TRUE;
  $handler->display->display_options['filters']['title_1']['expose']['operator_id'] = 'title_1_op';
  $handler->display->display_options['filters']['title_1']['expose']['label'] = 'Name does not contain';
  $handler->display->display_options['filters']['title_1']['expose']['operator'] = 'title_1_op';
  $handler->display->display_options['filters']['title_1']['expose']['identifier'] = 'title_1';
  $handler->display->display_options['filters']['title_1']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );
  /* Filter criterion: API documentation: Object type */
  $handler->display->display_options['filters']['object_type']['id'] = 'object_type';
  $handler->display->display_options['filters']['object_type']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['object_type']['field'] = 'object_type';
  $handler->display->display_options['filters']['object_type']['relationship'] = 'did';
  $handler->display->display_options['filters']['object_type']['group'] = 1;
  $handler->display->display_options['filters']['object_type']['exposed'] = TRUE;
  $handler->display->display_options['filters']['object_type']['expose']['operator_id'] = 'object_type_op';
  $handler->display->display_options['filters']['object_type']['expose']['label'] = 'Type';
  $handler->display->display_options['filters']['object_type']['expose']['operator'] = 'object_type_op';
  $handler->display->display_options['filters']['object_type']['expose']['identifier'] = 'object_type';
  $handler->display->display_options['filters']['object_type']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Member List Block */
  $handler = $view->new_display('block', 'Member List Block', 'block_member_list');
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;

  $views[$view->name] = $view;

  // API Search view.

  $view = new view();
  $view->name = 'api_search';
  $view->description = 'Blocks for searching API documentation. Used directly by the API module. Do not delete or disable.';
  $view->tag = 'default';
  $view->base_table = 'api_documentation';
  $view->human_name = 'API Search';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'access API reference';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'title' => 'title',
    'object_type' => 'object_type',
    'file_name' => 'file_name',
    'summary' => 'summary',
  );
  $handler->display->display_options['style_options']['default'] = 'title';
  $handler->display->display_options['style_options']['info'] = array(
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'object_type' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'file_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'summary' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* No results behavior: Global: Unfiltered text */
  $handler->display->display_options['empty']['area_text_custom']['id'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['table'] = 'views';
  $handler->display->display_options['empty']['area_text_custom']['field'] = 'area_text_custom';
  $handler->display->display_options['empty']['area_text_custom']['label'] = 'No matches';
  $handler->display->display_options['empty']['area_text_custom']['empty'] = TRUE;
  $handler->display->display_options['empty']['area_text_custom']['content'] = 'No search results found.';
  /* Field: API documentation: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Name';
  $handler->display->display_options['fields']['title']['link_to_docs'] = 'object';
  /* Field: API documentation: Object type */
  $handler->display->display_options['fields']['object_type']['id'] = 'object_type';
  $handler->display->display_options['fields']['object_type']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['object_type']['field'] = 'object_type';
  $handler->display->display_options['fields']['object_type']['label'] = 'Type';
  /* Field: API documentation: File name */
  $handler->display->display_options['fields']['file_name']['id'] = 'file_name';
  $handler->display->display_options['fields']['file_name']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['file_name']['field'] = 'file_name';
  $handler->display->display_options['fields']['file_name']['label'] = 'Location';
  $handler->display->display_options['fields']['file_name']['link_to_docs'] = 'file_base';
  /* Field: API documentation: Summary */
  $handler->display->display_options['fields']['summary']['id'] = 'summary';
  $handler->display->display_options['fields']['summary']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['summary']['field'] = 'summary';
  $handler->display->display_options['fields']['summary']['label'] = 'Description';
  /* Sort criterion: API documentation: Object name */
  $handler->display->display_options['sorts']['object_name']['id'] = 'object_name';
  $handler->display->display_options['sorts']['object_name']['table'] = 'api_documentation';
  $handler->display->display_options['sorts']['object_name']['field'] = 'object_name';
  /* Contextual filter: API documentation: Branch */
  $handler->display->display_options['arguments']['branch_id']['id'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['table'] = 'api_documentation';
  $handler->display->display_options['arguments']['branch_id']['field'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['branch_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['branch_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['branch_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['branch_id']['summary_options']['items_per_page'] = '25';
  /* Contextual filter: API documentation: Title */
  $handler->display->display_options['arguments']['title']['id'] = 'title';
  $handler->display->display_options['arguments']['title']['table'] = 'api_documentation';
  $handler->display->display_options['arguments']['title']['field'] = 'title';
  $handler->display->display_options['arguments']['title']['default_action'] = 'default';
  $handler->display->display_options['arguments']['title']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['title']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['title']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['title']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['title']['limit'] = '0';
  $handler->display->display_options['arguments']['title']['partial_match'] = TRUE;
  /* Filter criterion: API documentation: Object type */
  $handler->display->display_options['filters']['object_type']['id'] = 'object_type';
  $handler->display->display_options['filters']['object_type']['table'] = 'api_documentation';
  $handler->display->display_options['filters']['object_type']['field'] = 'object_type';
  $handler->display->display_options['filters']['object_type']['exposed'] = TRUE;
  $handler->display->display_options['filters']['object_type']['expose']['operator_id'] = 'object_type_op';
  $handler->display->display_options['filters']['object_type']['expose']['label'] = 'Type';
  $handler->display->display_options['filters']['object_type']['expose']['operator'] = 'object_type_op';
  $handler->display->display_options['filters']['object_type']['expose']['identifier'] = 'object_type';
  $handler->display->display_options['filters']['object_type']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
  );

  /* Display: Search Block */
  $handler = $view->new_display('block', 'Search Block', 'block_search_results');
  $handler->display->display_options['display_description'] = 'Block for search results';
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;

  $views[$view->name] = $view;

  // API References view.

  $view = new view();
  $view->name = 'api_references';
  $view->description = 'Blocks for listing references in the API module. Used directly by the API module. Do not delete or disable.';
  $view->tag = 'default';
  $view->base_table = 'api_reference_storage';
  $view->human_name = 'API References';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'access API reference';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'title' => 'title',
    'file_name' => 'file_name',
    'summary' => 'summary',
  );
  $handler->display->display_options['style_options']['default'] = 'title';
  $handler->display->display_options['style_options']['info'] = array(
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'file_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'summary' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Relationship: API references: Documentation object where reference was found */
  $handler->display->display_options['relationships']['from_did']['id'] = 'from_did';
  $handler->display->display_options['relationships']['from_did']['table'] = 'api_reference_storage';
  $handler->display->display_options['relationships']['from_did']['field'] = 'from_did';
  $handler->display->display_options['relationships']['from_did']['label'] = 'Referencing object';
  $handler->display->display_options['relationships']['from_did']['required'] = TRUE;
  /* Field: API documentation: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['relationship'] = 'from_did';
  $handler->display->display_options['fields']['title']['label'] = 'Name';
  $handler->display->display_options['fields']['title']['link_to_docs'] = 'object';
  /* Field: API documentation: File name */
  $handler->display->display_options['fields']['file_name']['id'] = 'file_name';
  $handler->display->display_options['fields']['file_name']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['file_name']['field'] = 'file_name';
  $handler->display->display_options['fields']['file_name']['relationship'] = 'from_did';
  $handler->display->display_options['fields']['file_name']['label'] = 'Location';
  $handler->display->display_options['fields']['file_name']['link_to_docs'] = 'file_base';
  /* Field: API documentation: Summary */
  $handler->display->display_options['fields']['summary']['id'] = 'summary';
  $handler->display->display_options['fields']['summary']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['summary']['field'] = 'summary';
  $handler->display->display_options['fields']['summary']['relationship'] = 'from_did';
  $handler->display->display_options['fields']['summary']['label'] = 'Description';
  /* Contextual filter: API references: Branch on reference */
  $handler->display->display_options['arguments']['branch_id']['id'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['table'] = 'api_reference_storage';
  $handler->display->display_options['arguments']['branch_id']['field'] = 'branch_id';
  $handler->display->display_options['arguments']['branch_id']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['branch_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['branch_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['branch_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['branch_id']['summary_options']['items_per_page'] = '25';
  /* Contextual filter: API references: Reference name */
  $handler->display->display_options['arguments']['object_name']['id'] = 'object_name';
  $handler->display->display_options['arguments']['object_name']['table'] = 'api_reference_storage';
  $handler->display->display_options['arguments']['object_name']['field'] = 'object_name';
  $handler->display->display_options['arguments']['object_name']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['object_name']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['object_name']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['object_name']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['object_name']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['object_name']['limit'] = '0';
  /* Contextual filter: API documentation: Object type */
  $handler->display->display_options['arguments']['object_type']['id'] = 'object_type';
  $handler->display->display_options['arguments']['object_type']['table'] = 'api_documentation';
  $handler->display->display_options['arguments']['object_type']['field'] = 'object_type';
  $handler->display->display_options['arguments']['object_type']['relationship'] = 'from_did';
  $handler->display->display_options['arguments']['object_type']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['object_type']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['object_type']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['object_type']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['object_type']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['object_type']['limit'] = '0';
  /* Filter criterion: API references: Reference type */
  $handler->display->display_options['filters']['object_type']['id'] = 'object_type';
  $handler->display->display_options['filters']['object_type']['table'] = 'api_reference_storage';
  $handler->display->display_options['filters']['object_type']['field'] = 'object_type';
  $handler->display->display_options['filters']['object_type']['value'] = 'group';

  /* Display: Items in Topic List Block */
  $handler = $view->new_display('block', 'Items in Topic List Block', 'block_items_in_group');
  $handler->display->display_options['display_description'] = 'All items in a particular group/topic';
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;

  /* Display: Related Topics Block */
  $handler = $view->new_display('block', 'Related Topics Block', 'block_related_topics');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['display_description'] = 'Topics (groups) related to a documentation item';
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['defaults']['style_plugin'] = FALSE;
  $handler->display->display_options['style_plugin'] = 'api_dl_list';
  $handler->display->display_options['style_options']['default_row_class'] = FALSE;
  $handler->display->display_options['style_options']['row_class_special'] = FALSE;
  $handler->display->display_options['style_options']['class'] = 'api-related-topics';
  $handler->display->display_options['style_options']['wrapper_class'] = '';
  $handler->display->display_options['defaults']['style_options'] = FALSE;
  $handler->display->display_options['defaults']['row_plugin'] = FALSE;
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['defaults']['row_options'] = FALSE;
  $handler->display->display_options['defaults']['relationships'] = FALSE;
  /* Relationship: API references: Related topic object */
  $handler->display->display_options['relationships']['related_topic']['id'] = 'related_topic';
  $handler->display->display_options['relationships']['related_topic']['table'] = 'api_reference_storage';
  $handler->display->display_options['relationships']['related_topic']['field'] = 'related_topic';
  $handler->display->display_options['relationships']['related_topic']['required'] = TRUE;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: API documentation: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['relationship'] = 'related_topic';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['exclude'] = TRUE;
  $handler->display->display_options['fields']['title']['alter']['text'] = '<dt>[title]</dt>';
  $handler->display->display_options['fields']['title']['element_type'] = '0';
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['title']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['title']['link_to_docs'] = 'object';
  /* Field: API documentation: Summary */
  $handler->display->display_options['fields']['summary']['id'] = 'summary';
  $handler->display->display_options['fields']['summary']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['summary']['field'] = 'summary';
  $handler->display->display_options['fields']['summary']['relationship'] = 'related_topic';
  $handler->display->display_options['fields']['summary']['label'] = '';
  $handler->display->display_options['fields']['summary']['exclude'] = TRUE;
  $handler->display->display_options['fields']['summary']['alter']['text'] = '<dd>[summary]</dd>';
  $handler->display->display_options['fields']['summary']['element_type'] = '0';
  $handler->display->display_options['fields']['summary']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['summary']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['summary']['element_default_classes'] = FALSE;
  /* Field: DT/DL element */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['ui_name'] = 'DT/DL element';
  $handler->display->display_options['fields']['nothing']['label'] = '';
  $handler->display->display_options['fields']['nothing']['alter']['text'] = '<dt>[title]</dt>
<dd>[summary]</dd>
';
  $handler->display->display_options['fields']['nothing']['element_type'] = '0';
  $handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['nothing']['element_wrapper_type'] = '0';
  $handler->display->display_options['fields']['nothing']['element_default_classes'] = FALSE;
  $handler->display->display_options['defaults']['sorts'] = FALSE;
  /* Sort criterion: API documentation: Title */
  $handler->display->display_options['sorts']['title']['id'] = 'title';
  $handler->display->display_options['sorts']['title']['table'] = 'api_documentation';
  $handler->display->display_options['sorts']['title']['field'] = 'title';
  $handler->display->display_options['sorts']['title']['relationship'] = 'related_topic';
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: API references: Documentation ID of item where reference was found */
  $handler->display->display_options['arguments']['from_did']['id'] = 'from_did';
  $handler->display->display_options['arguments']['from_did']['table'] = 'api_reference_storage';
  $handler->display->display_options['arguments']['from_did']['field'] = 'from_did';
  $handler->display->display_options['arguments']['from_did']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['from_did']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['from_did']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['from_did']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['from_did']['summary_options']['items_per_page'] = '25';

  $views[$view->name] = $view;

  return $views;
}
