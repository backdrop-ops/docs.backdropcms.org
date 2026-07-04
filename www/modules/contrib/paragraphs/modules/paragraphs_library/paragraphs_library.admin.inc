<?php
/**
 * @file
 * Paragraphs library administration functions.
 */

/**
 * Delete confirmation form.
 */
function paragraphs_library_delete_form($form, &$form_state, $library_item) {
  $form_state['paragraphs_library_item'] = $library_item;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['paragraphs_library_type_id'] = [
    '#type' => 'value',
    '#value' => entity_plus_id('paragraphs_library_item', $library_item),
  ];
  $library_item_uri = $library_item->uri();
  return confirm_form($form,
    t('Are you sure you want to delete Paragraphs library item %title?', [
      '%title' => entity_label('paragraphs_library_item', $library_item),
    ]),
    $library_item_uri['path'],
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Delete form submit handler.
 */
function paragraphs_library_delete_form_submit($form, &$form_state) {
  $library_item = $form_state['paragraphs_library_item'];
  paragraphs_library_item_delete($library_item);

  backdrop_set_message(t('Paragraphs library item  %title deleted.', [
    '%title' => entity_label('paragraphs_library_item', $library_item),
  ]));

  $form_state['redirect'] = '<front>';
}

/**
 * Delete confirmation for multiple paragraphs_library_item entities through action
 * and bulk operation.
 */
function paragraphs_library_multiple_delete_confirm($form, &$form_state) {
  if (isset($form_state['item_ids'])) {
    $library_item_ids = $form_state['item_ids'];
  }
  elseif (isset($_SESSION['paragraphs_library_delete_action']['timestamp']) && (REQUEST_TIME - $_SESSION['paragraphs_library_delete_action']['timestamp'] < 6000)) {
    $library_item_ids = $_SESSION['paragraphs_library_delete_action']['item_ids'];
    $form_state['item_ids'] = $library_item_ids;
    $form_state['cache'] = TRUE;
    unset($_SESSION['paragraphs_library_delete_action']);
  }
  else {
    $library_item_ids = [];
  }

  $form['#tree'] = TRUE;

  if (empty($library_item_ids)) {
    $destination = isset($_GET['destination']) ? $_GET['destination'] : '<front>';
    $form['empty']['#markup'] = '<p>' . t('Return to the <a href="!url">front page</a>.', [
      '!url' => url($destination),
    ]) . '</p>';
    backdrop_set_message(t('No Paragraphs library items have been selected for deletion.'), 'error');
    return $form;
  }

  $form['paragraphs_library_list'] = [
    '#theme' => 'item_list',
    '#items' => [],
  ];

  $rows = db_query('SELECT item_id, title FROM {paragraphs_library} WHERE item_id IN (:item_ids)', [':item_ids' => $library_item_ids])->fetchAllKeyed();
  foreach ($rows as $library_item_id => $title) {
    $form['paragraphs_library_entities'][$library_item_id] = [
      '#type' => 'hidden',
      '#value' => $library_item_id,
    ];
    $form['paragraphs_library_list']['#items'][] = check_plain($title);
  }

  $confirm_question = format_plural(count($rows), 'Are you sure you want to delete this item?', 'Are you sure you want to delete these items?');
  return confirm_form($form, $confirm_question, '<front>', t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

/**
 * Form submission handler for paragraphs_library_multiple_delete_confirm().
 */
function paragraphs_library_multiple_delete_confirm_submit($form, &$form_state) {
  paragraphs_library_item_delete_multiple(array_keys($form_state['values']['paragraphs_library_entities']));
  cache_clear_all();
  $count = count($form_state['values']['paragraphs_library_entities']);
  watchdog('paragraphs_library', 'Deleted @count basic entities.', ['@count' => $count]);

  backdrop_set_message(format_plural($count, 'Deleted 1 paragraphs_library.', 'Deleted @count basic entities.'));
  $form_state['redirect'] = '<front>';
}

/**
 * Displays a simple page with all available Paragraphs library types, linked to
 * the administration page.
 */
function paragraphs_library_type_view() {
  $all_types = paragraphs_library_type_load();
  $field_ui = module_exists('field_ui');
  $page = [];
  $rows = [];
  $operations = [];

  $header = array(
    t('Label'),
    t('Description'),
    t('Operations'),
  );
  foreach ($all_types as $library_type) {
    $type_url_str = strtr($library_type->type, array('_' => '-'));
    $row = array(theme('label_machine_name__paragraphs', array(
      'label' => $library_type->label,
      'machine_name' => $library_type->type,
    )));
    $row['description'] = $library_type->description;

    // Manage library type.
    $operations['edit_type'] = array(
      'title' => t('Configure'),
      'href' => 'admin/structure/paragraphs/library/types/' . $type_url_str . '/edit',
    );

    if ($field_ui) {
      // Manage fields.
      $operations['manage_fields'] = array(
        'title' => t('Manage fields'),
        'href' => 'admin/structure/paragraphs/library/types/' . $type_url_str . '/fields',
      );

      // Display fields.
      $operations['manage_display'] = array(
        'title' => t('Manage display'),
        'href' => 'admin/structure/paragraphs/library/types/' . $type_url_str . '/display',
      );
    }

    // Delete bundle.
    $operations['delete_type'] = array(
      'title' => t('Delete'),
      'href' => 'admin/structure/paragraphs/library/types/' . $type_url_str . '/delete',
    );

    $row['operations'] = array(
      'data' => array(
        '#type' => 'dropbutton',
        '#links' => $operations,
      ),
    );

    $rows[$library_type->type] = $row;
  }

  // Sort rows by type.
  ksort($rows);

  // Render paragraphs library type table.
  $page['paragraphs_library_type_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No library types have been defined yet.'),
  );

  return $page;
}

/**
 * Form to create a new Paragraphs library item type.
 */
function paragraphs_library_type_form($form, &$form_state, ?Object $paragraphs_library_type = NULL) {
  $form['label'] = [
    '#type' => 'textfield',
    '#title' => t('Paragraphs library type name'),
    '#description' => !empty($paragraphs_library_type) ? t('Machine name: @type', ['@type' => $paragraphs_library_type->type]) : '',
    '#required' => TRUE,
    '#default_value' => !empty($paragraphs_library_type->label) ? $paragraphs_library_type->label : '',
  ];
  if (empty($paragraphs_library_type)) {
    $form['type'] = [
      '#type' => 'machine_name',
      '#title' => 'Paragraphs library type machine name',
      '#maxlength' => 21,
      '#required' => TRUE,
      '#machine_name' => [
        'exists' => 'paragraphs_library_type_exists',
        'source' => ['label'],
      ],
    ];
  }
  else {
    $form['type'] = [
      '#type' => 'value',
      '#value' => $paragraphs_library_type->type,
    ];
  }
  $form['description'] = [
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('A short description of this Paragraphs library type.'),
    '#description_display' => 'after',
    '#default_value' => !empty($paragraphs_library_type->description) ? $paragraphs_library_type->description : '',
  ];
  $form['actions'] = [
    '#type' => 'actions',
    'submit' => [
      '#type' => 'submit',
      '#value' => !empty($paragraphs_library_type) ? t('Save library type') : t('Create library type'),
    ],
  ];
  return $form;
}

/**
 * Callback to be sure the bundle machine name is unique.
 */
function paragraphs_library_type_exists($value) {
  $names = config_get_names_with_prefix('paragraphs_library.type.' . $value);
  return !empty($names);
}

/**
 * Paragraphs library type form submit handler.
 */
function paragraphs_library_type_form_submit($form, &$form_state) {
  $config = config('paragraphs_library.type.' . $form_state['values']['type']);
  $config->set('label', $form_state['values']['label']);
  $config->set('type', $form_state['values']['type']);
  $config->set('description', $form_state['values']['description']);
  $config->save();

  // Notify Field API of the new bundle.
  field_attach_create_bundle('paragraphs_library_item', $form_state['values']['type']);

  backdrop_set_message(t('Paragraphs library type %label saved.', [
    '%label' => $form_state['values']['label']]),
  );
  $form_state['redirect'] = 'admin/structure/paragraphs/library/types';
  menu_rebuild();
}

/**
 * Confirm form to delete a paragraphs_library_type.
 */
function paragraphs_library_type_delete_form($form, &$form_state, Object $paragraphs_library_type) {
  // Send the paragraphs_library_item ID as a value field to the form.
  $form['paragraphs_library_type'] = [
    '#type' => 'value',
    '#value' => $paragraphs_library_type->type,
  ];
  return confirm_form(
    $form,
    t('Are you sure you want to delete library type %title?', ['%title' => check_plain($paragraphs_library_type->label)]),
    'admin/structure/paragraphs/library/types/',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Submit handler for the paragraphs_library_type delete form.
 *
 * Deletes the paragraphs_library_type and displays a message.
 */
function paragraphs_library_type_delete_form_submit($form, &$form_state) {
  $library_type_name = strtr($form_state['values']['paragraphs_library_type'], array('_' => '-'));
  $library_type = paragraphs_library_type_load($library_type_name);
  if (is_object($library_type)) {
    field_attach_delete_bundle('paragraphs_library_type', $library_type->type);
    $config = config('paragraphs_library.type.' . $library_type->type);
    $config->delete();
    paragraphs_library_type_load('', TRUE);
    entity_info_cache_clear();
    state_set('menu_rebuild_needed', TRUE);
  }
  backdrop_set_message(t('Paragraphs library type %title has been deleted.', ['%title' => check_plain($paragraphs_library_type->label)]));
  $form_state['redirect'] = 'admin/structure/paragraphs/library/types';
}
