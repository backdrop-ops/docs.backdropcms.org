<?php

/**
 * @file
 * Admin functions for the paragraphs module.
 */

/**
 * Page callback to show the bundle overview page.
 *
 * @return null|string
 *   Rendered table of bundles.
 *
 * @throws Exception
 */
function paragraphs_admin_bundle_overview() {
  $page = array();
  $bundles = paragraphs_bundle_load();
  $field_ui = module_exists('field_ui');

  $header = array(
    t('Label'),
    t('Description'),
    t('Operations'),
  );
  $rows = array();
  $operations = array();
  foreach ($bundles as $bundle) {

    $type_url_str = strtr($bundle->bundle, array('_' => '-'));
    $row = array(theme('label_machine_name__paragraphs', array(
      'label' => $bundle->label,
      'machine_name' => $bundle->bundle,
    )));
    $row['description'] = $bundle->description;

    if ($field_ui) {
      // Manage fields.
      $operations['manage_fields'] = array(
        'title' => t('Manage fields'),
        'href' => 'admin/structure/paragraphs/' . $type_url_str . '/fields',
      );

      // Display fields.
      $operations['manage_display'] = array(
        'title' => t('Manage display'),
        'href' => 'admin/structure/paragraphs/' . $type_url_str . '/display',
      );
    }

    // Manage bundle.
    $operations['edit_type'] = array(
      'title' => t('Configure'),
      'href' => 'admin/structure/paragraphs/' . $type_url_str . '/edit',
    );
    // Clone bundle.
    $operations['clone_type'] = array(
      'title' => t('clone'),
      'href' => 'admin/structure/paragraphs/' . $type_url_str . '/clone',
    );

    // Manage permissions if module enabled.
    if (module_exists('paragraphs_bundle_permissions')) {
      $operations['permissions'] = array(
        'title' => t('Permissions'),
        'href' => paragraphs_bundle_permissions_url(),
        'query' => array('search' => $bundle->name . ': '),
        'attributes' => array('title' => t('Configure permissions')),
      );
    }

    // Delete bundle.
    $operations['delete_type'] = array(
      'title' => t('Delete'),
      'href' => 'admin/structure/paragraphs/' . $type_url_str . '/delete',
    );

    $row['operations'] = array(
      'data' => array(
        '#type' => 'dropbutton',
        '#links' => $operations,
      ),
    );

    $rows[$bundle->bundle] = $row;
  }

  // Sort rows by bundle.
  ksort($rows);

  // Render paragraphs bundle table.
  $page['paragraphs_bundle_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No paragraph types have been defined yet.'),
  );

  return $page;
}

/**
 * Form to create or edit an paragraphs bundle.
 *
 * @param array $form
 *   The form structure array.
 * @param array $form_state
 *   An associative array containing the current state of the form.
 * @param object $bundle
 *   The bundle.
 *
 * @return array
 *   The form structure array.
 */
function paragraphs_admin_bundle_form(array $form, array &$form_state, $bundle = NULL) {

  if (!isset($bundle) && !$bundle) {
    // This is a new bundle.
    $bundle = new stdClass();
    $bundle->name = '';
    $bundle->bundle = '';
    $bundle->label = '';
    $bundle->description = '';
    $bundle->locked = 0;
    $bundle->allow_unpublish = 1;
  }
  else {
    if (!$bundle) {
      backdrop_set_message(t('Could not load Paragraph type'), 'error');
      backdrop_goto('admin/structure/paragraphs');
    }
  }

  $form['#paragraphs_bundle'] = $bundle;

  $form['name'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => $bundle->name,
    '#description' => t('The human-readable name of this Paragraph type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  $form['bundle'] = array(
    '#type' => 'machine_name',
    '#default_value' => $bundle->bundle,
    '#maxlength' => 32,
    '#disabled' => $bundle->locked,
    '#machine_name' => array(
      'exists' => 'paragraphs_bundle_load',
    ),
    '#description' => t('A unique machine-readable name for this Paragraph type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['label'] = array(
    '#title' => t('Admin label'),
    '#type' => 'textfield',
    '#default_value' => $bundle->label,
    '#description' => t('The label for this Paragraph type as it will appear to users on edit forms. Defaults to the name value if left empty.'),
    '#size' => 30,
  );

  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textarea',
    '#default_value' => $bundle->description,
    '#description' => t('Describe this Paragraph type. The text will be displayed on the Paragraphs admin overview page.'),
  );

  $form['locked'] = array(
    '#type' => 'value',
    '#value' => $bundle->locked,
  );

  $form['allow_unpublish'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow unpublishing from the admin interface.'),
    '#default_value' => (isset($bundle->allow_unpublish)) ? $bundle->allow_unpublish : 0,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Paragraph type'),
    '#weight' => 40,
  );

  return $form;
}

/**
 * Form validation handler for paragraphs_admin_bundle_form().
 *
 * @param array $form
 *   The form structure array.
 * @param array $form_state
 *   An associative array containing the current state of the form.
 *
 * @see paragraphs_admin_bundle_form_submit()
 */
function paragraphs_admin_bundle_form_validate(array $form, array &$form_state) {
  $bundle = new stdClass();
  $bundle->name = trim($form_state['values']['name']);

  if (!$form_state['values']['locked']) {
    $bundle->bundle = trim($form_state['values']['bundle']);
    // 'theme' conflicts with theme_node_form().
    // '0' is invalid, since elsewhere we check it using empty().
    if (in_array($bundle->bundle, array('0', 'theme'))) {
      form_set_error('type', t("Invalid machine-readable name. Enter a name other than %invalid.", array('%invalid' => $bundle->bundle)));
    }
  }

}

/**
 * Submit handler for paragraphs_admin_bundle_form().
 *
 * @param array $form
 *   The form structure array.
 * @param array $form_state
 *   An associative array containing the current state of the form.
 *
 * @see paragraphs_admin_bundle_form()
 */
function paragraphs_admin_bundle_form_submit(array $form, array &$form_state) {

  $bundle = new stdClass();

  if (!$form_state['values']['locked']) {
    $bundle->bundle = trim($form_state['values']['bundle']);
  }
  else {
    $bundle->bundle = $form['#paragraphs_bundle']->bundle;
  }

  $bundle->locked = 1;

  $bundle->name = trim($form_state['values']['name']);

  $bundle->allow_unpublish = $form_state['values']['allow_unpublish'];

  // Set bundle label equal to name if empty.
  $bundle->label = empty($form_state['values']['label']) ? $bundle->name : trim($form_state['values']['label']);

  $bundle->description = trim($form_state['values']['description']);

  $variables = $form_state['values'];

  // Remove everything that's been saved already - whatever's left is assumed
  // to be a persistent variable.
  foreach ($variables as $key => $value) {
    if (isset($bundle->$key)) {
      unset($variables[$key]);
    }
  }

  unset($variables['form_token'], $variables['op'], $variables['submit'], $variables['delete'], $variables['reset'], $variables['form_id'], $variables['form_build_id']);

  $status = paragraphs_bundle_save($bundle);

  // Add permissions if paragraphs_bundle_permissions is enabled and paragraph
  // is newly created.
  if (module_exists('paragraphs_bundle_permissions') && $status == SAVED_NEW) {
    // Define the permission name for viewing paragraph content.
    $permission_name = 'view paragraph content ' . $bundle->bundle;
    // Add the permission to the anonymous role.
    user_role_change_permissions('anonymous', array($permission_name => TRUE));
    // Add the permission to the authenticated role.
    user_role_change_permissions('authenticated', array($permission_name => TRUE));

    // Define all administrator permissions.
    $admin_permissions = array('view', 'update', 'delete', 'create');
    $permissions = array();
    foreach ($admin_permissions as $permission) {
      $permission_name = $permission . ' paragraph content ' . $bundle->bundle;
      $permissions += array(
        $permission_name => TRUE,
      );
    }
    // Add the permissions to the administrator role.
    $admin_role = config_get('system.core', 'user_admin_role');
    user_role_change_permissions($admin_role, $permissions);

    // Define a message informing the user who created the paragraph type that
    // default permissions have been added.
    $roles = user_roles();
    $message = t("Default permissions have been added to the %anonymous and %authenticated roles ('View') and %admin role (all) for the %paragraph_type paragraph type. ",
      array(
        '%anonymous' => $roles['anonymous'],
        '%authenticated' => $roles['authenticated'],
        '%admin' => $roles[$admin_role],
        '%paragraph_type' => $bundle->name,
      )
    );
    // Define part of the message depending on whether the user has permission
    // to administer permissions or not.
    $conditional_message = paragraphs_bundle_permissions_conditional_message($bundle->name . ': ');

    // Display the combined message.
    backdrop_set_message($message . $conditional_message, 'info');
  }

  $t_args = array('%name' => $bundle->name);

  if ($status == SAVED_UPDATED) {
    backdrop_set_message(t('The Paragraph type %name has been updated.', $t_args));
  }
  elseif ($status == SAVED_NEW) {
    backdrop_set_message(t('The Paragraph type %name has been added.', $t_args));
    watchdog('paragraphs', 'Added Paragraph type %name.', $t_args, WATCHDOG_NOTICE, l(t('view'), 'admin/structure/paragraphs'));
  }

  $form_state['redirect'] = 'admin/structure/paragraphs';
}

/**
 * Menu callback to delete a single paragraph bundle.
 *
 * @param array $form
 *   The form structure array.
 * @param array $form_state
 *   An associative array containing the current state of the form.
 * @param object $bundle
 *   The bundle.
 *
 * @ingroup forms
 */
function paragraphs_admin_bundle_delete_form(array $form, array &$form_state, $bundle) {
  if (!$bundle) {
    backdrop_set_message(t('Could not load Paragraph type'), 'error');
    backdrop_goto('admin/structure/paragraphs');
  }

  $form['type'] = array(
    '#type' => 'value',
    '#value' => $bundle->bundle,
  );
  $form['name'] = array(
    '#type' => 'value',
    '#value' => $bundle->name,
  );

  $message = t('Are you sure you want to delete the Paragraph type %bundle?', array('%bundle' => $bundle->name));
  $caption = '<p>' . t('This action cannot be undone. Content using this Paragraph type will be broken.') . '</p>';

  return confirm_form($form, filter_xss_admin($message), 'admin/structure/paragraphs', filter_xss_admin($caption), t('Delete'));
}

/**
 * Process and confirm paragraphs bundle deletion.
 *
 * @param array $form
 *   The form structure array.
 * @param array $form_state
 *   An associative array containing the current state of the form.
 *
 * @see paragraphs_admin_bundle_delete_form()
 */
function paragraphs_admin_bundle_delete_form_submit(array $form, array &$form_state) {
  paragraphs_bundle_delete($form_state['values']['type']);

  $t_args = array('%name' => $form_state['values']['name']);
  backdrop_set_message(t('The Paragraph type %name has been deleted.', $t_args));
  watchdog('paragraphs', 'Deleted Paragraph type %name.', $t_args, WATCHDOG_NOTICE);

  $form_state['redirect'] = 'admin/structure/paragraphs';
}

/**
 * Separate paragraphs_item edit form.
 */
function paragraphs_modal_admin_edit($form, &$form_state, $paragraphs_item) {
  if (!$paragraphs_item) {
    backdrop_not_found();
  }
  $bundle = paragraphs_bundle_load($paragraphs_item->bundle);
  backdrop_set_title(t('Edit !title paragraph', array('!title' => $bundle->name)));

  $form['paragraphs_item'] = array(
    '#type' => 'value',
    '#value' => $paragraphs_item,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#weight' => 10000,
    '#value' => t('Save'),
    '#ajax' => array(
      'callback' => 'paragraphs_modal_admin_save_close',
      'effect' => 'fade',
      // 'wrapper' => $element['#wrapper_id'],
    ),
  );

  field_attach_form('paragraphs_item', $paragraphs_item, $form, $form_state);

  // Get the top-level host entity.
  $item = $paragraphs_item;
  $host = NULL;
  while (method_exists($item, 'hostEntity')) {
    $host = $item->hostEntity();
    $host_entity_type = $item->hostEntityType();
    $host_entity_id = $item->hostEntityId();
    $host_entity_bundle = $item->hostEntityBundle();
    $item = $host;
  }
  // Only show revisioning options if our host is definitely revisioned.
  $show_revision_options = FALSE;
  $use_revisions = FALSE;
  if ($host_entity_type == 'node') {
    $show_revision_options = config_get('node.type.' . $host_entity_bundle, 'settings.revision_enabled');
    $use_revisions = config_get('node.type.' . $host_entity_bundle, 'settings.revision_default');
  }
  if ($show_revision_options) {
    $form['additional_settings'] = array(
      '#type' => 'vertical_tabs',
      '#weight' => 99,
    );
    $form['revision_information'] = array(
      '#type' => 'fieldset',
      '#title' => t('Revision information'),
      '#collapsible' => TRUE,
      // Collapsed by default when "Create new revision" is unchecked.
      '#collapsed' => !$use_revisions,
      '#group' => 'additional_settings',
      '#attributes' => array(
        'class' => array('node-form-revision-information'),
      ),
      '#attached' => array(
        'js' => array(backdrop_get_path('module', 'node') . '/js/node.js'),
      ),
      '#weight' => 20,
      '#access' => user_access('administer nodes'),
    );
    $form['revision_information']['revision'] = array(
      '#type' => 'checkbox',
      '#title' => t('Create new revision'),
      '#default_value' => $use_revisions,
      '#access' => user_access('administer nodes'),
    );
  }

  return $form;
}

/**
 * Validation function for entity form for validating the fields.
 */
function paragraphs_modal_admin_edit_validate($form, &$form_state) {
  field_attach_form_validate('paragraphs_item', $form_state['values']['paragraphs_item'], $form, $form_state);
}

/**
 * Submit function for edit entity form.
 */
function paragraphs_modal_admin_edit_submit($form, &$form_state) {
  $paragraphs_item = $form_state['values']['paragraphs_item'];
  field_attach_submit('paragraphs_item', $paragraphs_item, $form, $form_state);

  // Save a new revision of our host entity?
  $save_new_revision = !empty($form_state['values']['revision']);
  if ($save_new_revision) {
    $paragraphs_item->is_new_revision = TRUE;
  }

  // You get a new revision when you save a node, even if there are no changes.
  // Ensure that we do not save the host node if $save_new_revision is FALSE.
  $paragraphs_item->save(FALSE);
}

/**
 * Save and close Ajax modal popup.
 */
function paragraphs_modal_admin_save_close($form, &$form_state) {
  $paragraphs_item = $form_state['values']['paragraphs_item'];
  $bundle = paragraphs_bundle_load($paragraphs_item->bundle);
  // Close modal dialog upon submit.
  $commands = array(
    '#type' => 'ajax',
    '#commands' => array(),
  );
  $commands['#commands'][] = ajax_command_close_modal_dialog();

  // Refresh display of Paragraph that was edited.
  if (is_object($form_state['build_info']['args'][0]) && !empty($form_state['build_info']['args'][0]->item_id)) {
    $selector = '.paragraphs-item-' . $form_state['build_info']['args'][0]->item_id;
    $revision_id = $form_state['build_info']['args'][0]->revision_id;
    $fresh_paragraph = paragraphs_item_revision_load($revision_id)->view();
    $new_element = '<div class=\'messages status paragraphs-modal-admin-message\'>' . t('Paragraph !type has been updated.', array('!type' => $bundle->name)) . '</div>';
    $new_element .= backdrop_render($fresh_paragraph);
    $commands['#commands'][] = array(
      'command' => 'paragraphs_modal_admin_refresh',
      'selector' => $selector,
      'new_element' => $new_element,
    );
    $commands['#commands'][] = array(
      'command' => 'paragraphs_modal_admin_message_hide',
    );
  }
  return $commands;
}

/**
 * Paragraphs sort dialog popup form.
 */
function paragraphs_modal_admin_sort_form($form, &$form_state, $paragraphs_item) {
  if (!$paragraphs_item) {
    backdrop_not_found();
  }
  $langcode = $paragraphs_item->langcode;
  $host_entity = $paragraphs_item->hostEntity();
  $field_name = $paragraphs_item->field_name;
  $element = $paragraphs_item->fieldInfo();
  $items = $host_entity->{$field_name};

  // If the field can hold more than one item, display it as a draggable table.
  if ($element['cardinality'] != 1 || count($form_state['build_info']['args']) > 1) {
    $form['paragraphs_items']['#tree'] = TRUE;
    $form['host_entity']['#tree'] = TRUE;
    $table_id = backdrop_html_id($field_name . '_values');

    $rows = array();

    $form['host_entity'][] = array(
      'entity_type' => array(
        '#type' => 'hidden',
        '#value' => $paragraphs_item->hostEntityType(),
      ),
      'entity_id' => array(
        '#type' => 'hidden',
        '#value' => $paragraphs_item->hostEntityId(),
      ),
      'revision_id' => array(
        '#type' => 'hidden',
        '#value' => $paragraphs_item->hostEntityRevisionId(),
      ),
      'bundle' => array(
        '#type' => 'hidden',
        '#value' => $paragraphs_item->hostEntityBundle(),
      ),
      'field_name' => array(
        '#type' => 'hidden',
        '#value' => $field_name,
      ),
    );
    foreach ($items[$langcode] as $delta => $item) {
      $item['entity'] = paragraphs_item_load($item['value']);
      $item_render_array = $item['entity']->view('paragraphs_editor_preview', $langcode);
      $item_markup = backdrop_render($item_render_array);
      if (empty($item_markup) || empty(trim(strip_tags($item_markup, '<img><a>')))) {
        $bundle = paragraphs_bundle_load($item['entity']->bundle);
        $item_markup = $bundle->name;
      }
      $form['paragraphs_items'][$item['value']] = array(
        'preview' => array(
          '#markup' => $item_markup,
        ),
        'item_id' => array(
          '#type' => 'hidden',
          '#value' => $item['value'],
        ),
        'weight' => array(
          '#type' => 'weight',
          '#title' => t('Weight'),
          '#default_value' => $delta,
          '#delta' => 10,
          '#title_display' => 'invisible',
        ),
      );
    }
    $form['actions'] = array(
      '#type' => 'actions',
    );
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save Order'),
      '#ajax' => array(
        'callback' => 'paragraphs_modal_admin_sort_form_close',
        'effect' => 'fade',
      ),
    );
  }
  return $form;
}

/**
 * Paragraphs sort dialog popup form submit.
 */
function paragraphs_modal_admin_sort_form_submit($form, &$form_state) {
  // Load host entity.
  $entity_type = $form_state['values']['host_entity'][0]['entity_type'];
  $entity_id = $form_state['values']['host_entity'][0]['entity_id'];
  $field_name = $form_state['values']['host_entity'][0]['field_name'];
  $entity = entity_load($entity_type, $entity_id);

  $unsorted_items = $entity->{$field_name}[$entity->langcode];
  unset($entity->{$field_name}[$entity->langcode]);

  foreach ($unsorted_items as $key => $entity_item) {
    $item_id = $entity_item['value'];
    $unsorted_items[$key]['weight'] = $form_state['values']['paragraphs_items'][$item_id]['weight'];
  }
  backdrop_sort($unsorted_items, array('weight'));
  $sorted_items = array_values($unsorted_items);
  $entity->{$field_name}[$entity->langcode] = $sorted_items;
  $entity->save();
}

/**
 * Close Ajax modal popup.
 */
function paragraphs_modal_admin_sort_form_close($form, &$form_state) {
  // Close modal dialog upon submit.
  $commands = array(
    '#type' => 'ajax',
    '#commands' => array(),
  );
  $commands['#commands'][] = ajax_command_close_modal_dialog();

  // Refresh display of field that was sorted.
  if (!empty($form_state['values']['host_entity'][0]['entity_id']) && !empty($form_state['values']['host_entity'][0]['entity_type'])) {
    $entity_id = $form_state['values']['host_entity'][0]['entity_id'];
    $entity_type = $form_state['values']['host_entity'][0]['entity_type'];
    $revision_id = $form_state['values']['host_entity'][0]['revision_id'];
    $field_name = $form_state['values']['host_entity'][0]['field_name'];
    $entity = entity_load($entity_type, $entity_id);
    $fresh_field = field_view_field($entity_type, $entity, $field_name);
    $new_element = '<div class=\'messages status paragraphs-modal-admin-message\'>' . t('Paragraphs have been reordered.') . '</div>';
    $new_element .= backdrop_render($fresh_field);
    $commands['#commands'][] = ajax_command_html('.paragraphs-items-' . strtr($field_name, '_', '-'), $new_element);
    $commands['#commands'][] = array(
      'command' => 'paragraphs_modal_admin_message_hide',
    );
  }
  return $commands;
}

/**
 * Paragraphs sort dialog popup form.
 */
function theme_paragraphs_modal_admin_sort_form($variables) {
  $form = $variables['form'];

  // Initialize the variable which will store our table rows.
  $rows = array();

  // Iterate over each element in our $form['example_items'] array.
  foreach (element_children($form['paragraphs_items']) as $id) {
    $form['paragraphs_items'][$id]['weight']['#attributes']['class'] = array('paragraphs-item-weight');
    $rows[] = array(
      'data' => array(
        array(
          'data' => '',
          'class' => array('field-multiple-drag'),
        ),
        backdrop_render($form['paragraphs_items'][$id]['preview']),
        backdrop_render($form['paragraphs_items'][$id]['weight']),
      ),
      'class' => array('draggable'),
    );
  }
  $header = array(
    array(
      'data' => t('Preview'),
      'colspan' => 2,
      'class' => array('field-label'),
    ),
    t('Weight'),
  );
  $table_id = backdrop_html_id('paragraph-items-sort');
  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array(
      'id' => $table_id,
    ),
  ));
  $form['#children'] = $output;
  $output .= backdrop_render_children($form);

  backdrop_add_tabledrag($table_id, 'order', 'sibling', 'paragraphs-item-weight');
  return $output;
}

/**
 * Page callback: Form constructor for paragraph deletion confirmation form.
 *
 * @see paragraphs_menu()
 * @see paragraphs_modal_admin_remove_confirm_submit()
 */
function paragraphs_modal_admin_remove_confirm($form, &$form_state, $paragraphs_item = NULL) {
  if (empty($paragraphs_item) && !empty($form['paragraphs_item'])) {
    $paragraphs_item = $form['paragraphs_item'];
  }
  $form['item_id'] = array(
    '#type' => 'value',
    '#value' => $paragraphs_item->item_id,
  );
  $bundle = paragraphs_bundle_load($paragraphs_item->bundle);
  $form['description'] = array(
    '#markup' => t('Are you sure you want to remove this !type? (This action cannot be undone.)', array('!type' => $bundle->name)),
  );
  $form['actions'] = array(
    '#type' => 'actions',
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Remove'),
    '#attributes' => array('class' => array('button-danger')),
    '#ajax' => array(
      'callback' => 'paragraphs_modal_admin_remove_confirm_submit',
      'effect' => 'fade',
      // 'wrapper' => $element['#wrapper_id'],
    ),
  );
  return $form;
}

/**
 * Form submission handler for paragraphs_modal_admin_remove_confirm().
 *
 * @see paragraphs_modal_admin_remove_confirm()
 */
function paragraphs_modal_admin_remove_confirm_submit($form, &$form_state) {
  // Close modal dialog upon submit.
  $commands = array(
    '#type' => 'ajax',
    '#commands' => array(),
  );
  $commands['#commands'][] = ajax_command_close_modal_dialog();
  // Delete paragraph item and refresh display.
  if (is_object($form_state['build_info']['args'][0]) && !empty($form_state['build_info']['args'][0]->item_id)) {
    $item_id = $form_state['build_info']['args'][0]->item_id;
    $paragraph = paragraphs_item_load($item_id);
    $paragraph->deleteRevision();
    // Check entity_delete_multiple('paragraphs_item', array($item_id)).
    $commands['#commands'][] = ajax_command_replace('.paragraphs-item-' . $item_id, '<div class=\'messages status paragraphs-modal-admin-message\'>Paragraph has been removed.</div>');
    $commands['#commands'][] = array(
      'command' => 'paragraphs_modal_admin_message_hide',
    );
  }
  return $commands;
}


/**
 * Page callback: Form constructor for modal Paragraph unpublishing.
 *
 * @see paragraphs_menu()
 * @see paragraphs_modal_admin_unpublish_confirm_submit()
 */
function paragraphs_modal_admin_unpublish_confirm($form, &$form_state, $paragraphs_item = NULL) {
  if (empty($paragraphs_item) && !empty($form['paragraphs_item'])) {
    $paragraphs_item = $form['paragraphs_item'];
  }
  $form['item_id'] = array(
    '#type' => 'value',
    '#value' => $paragraphs_item->item_id,
  );
  $bundle = paragraphs_bundle_load($paragraphs_item->bundle);
  if ($bundle->allow_unpublish) {
    $form['description'] = array(
      '#markup' => t('Are you sure you want to unpublish this !type? (You will need to edit the content from the administrative side if you wish to republish it in the future.)', array('!type' => $bundle->name)),
    );
    $form['actions'] = array(
      '#type' => 'actions',
    );
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Unpublish'),
      '#ajax' => array(
        'callback' => 'paragraphs_modal_admin_unpublish_confirm_submit',
        'effect' => 'fade',
      ),
    );
  }
  else {
    $form['description'] = array(
      '#markup' => t('Unpublishing is not configured for this !type. (You configure the Paragraphs type from the administrative side if you wish to allow unpublishing.)', array('!type' => $bundle->name)),
    );
  }
  return $form;
}

/**
 * Form submission handler for paragraphs_modal_admin_unpublish_confirm().
 *
 * @see paragraphs_modal_admin_unpublish_confirm()
 */
function paragraphs_modal_admin_unpublish_confirm_submit($form, &$form_state) {
  // Close modal dialog upon submit.
  $commands = array(
    '#type' => 'ajax',
    '#commands' => array(),
  );
  $commands['#commands'][] = ajax_command_close_modal_dialog();
  // Unpublish paragraph item and refresh display.
  if (is_object($form_state['build_info']['args'][0]) && !empty($form_state['build_info']['args'][0]->item_id)) {
    $item_id = $form_state['build_info']['args'][0]->item_id;
    $paragraph = paragraphs_item_load($item_id);
    $paragraph->status = 0;
    $paragraph->save();
    $commands['#commands'][] = ajax_command_replace('.paragraphs-item-' . $item_id, '<div class=\'messages status paragraphs-modal-admin-message\'>Paragraph has been unpublished.</div>');
    $commands['#commands'][] = array(
      'command' => 'paragraphs_modal_admin_message_hide',
    );
  }
  return $commands;
}

/**
 * Separate paragraphs_item add form.
 *
 * Add a new paragraphs item above the submitted paragraphs item.
 */
function paragraphs_modal_admin_add_form($form, &$form_state, $mode, $paragraphs_bundle, $host_entity_type, $entity_id, $field_name) {
  $new_paragraph_position = 0;

  // If we start from an empty paragraph field.
  if ($mode == 'scratch') {
    // Load the host entity for the new paragraph.
    $host_entity = entity_load($host_entity_type, array($entity_id));
    $host_entity = reset($host_entity);
  }

  // Inserting a paragraph before or after an existing paragraph.
  else {
    // Load the existing paragraphs item.
    $paragraphs_item = paragraphs_item_load_multiple(array($entity_id));
    $paragraphs_item = reset($paragraphs_item);
    $host_entity = $paragraphs_item->hostEntity();
    $field_name = $paragraphs_item->field_name;

    // If we are inserting the new paragraph after an existing one.
    if ($mode == 'after' && $paragraphs_item) {
      // Load the paragraphs field.
      $paragraphs = field_get_items($host_entity_type, $host_entity, $paragraphs_item->field_name);

      // Calculate the position of the new paragraph (delta).
      foreach ($paragraphs as $key => $value) {
        if ($value['value'] === $paragraphs_item->item_id) {
          // After the existing one.
          $new_paragraph_position = $key + 1;
          break;
        }
      }
    }
  }

  // Create the new paragraph item and set the host entity + position.
  $new_paragraph = entity_create('paragraphs_item', array(
    'bundle' => $paragraphs_bundle->bundle,
    'field_name' => $field_name,
  ));
  $new_paragraph->setHostEntity($host_entity_type, $host_entity, $host_entity->langcode, FALSE);
  $new_paragraph->position = $new_paragraph_position;

  // Nice title for the creation page.
  backdrop_set_title(t('Create !bundle paragraph', array('!bundle' => $paragraphs_bundle->name)));

  // Pass variables in form for usage in submit handler.
  $form['paragraphs_bundle'] = array(
    '#type' => 'value',
    '#value' => $paragraphs_bundle,
  );
  $form['paragraphs_item'] = array(
    '#type' => 'value',
    '#value' => $new_paragraph,
  );
  $form['host_entity'] = array(
    '#type' => 'value',
    '#value' => $host_entity,
  );
  $form['host_entity_type'] = array(
    '#type' => 'value',
    '#value' => $host_entity_type,
  );
  $form['mode'] = array(
    '#type' => 'value',
    '#value' => $mode,
  );

  // Attach the form to create the new paragraphs item.
  field_attach_form('paragraphs_item', $new_paragraph, $form, $form_state);

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#weight' => 10000,
    '#value' => t('Save'),
    '#ajax' => array(
      'callback' => 'paragraphs_modal_admin_add_form_close',
      'effect' => 'fade',
    ),
  );

  return $form;
}

/**
 * Submit function for add entity form.
 */
function paragraphs_modal_admin_add_form_submit($form, &$form_state) {
  // Get data from form object.
  $paragraphs_item = $form['paragraphs_item']['#value'];
  $host_entity = $form['host_entity']['#value'];
  $host_entity_type = $form['host_entity_type']['#value'];
  $mode = $form['mode']['#value'];

  // We have to perform a host save if we are adding the first paragraph in the paragraph field.
  $skip_host_save = $mode == 'scratch' ? FALSE : TRUE;

  // Set host entity for new paragraph.
  $paragraphs_item->setHostEntity($host_entity_type, $host_entity, $host_entity->langcode, FALSE);

  // Attach submit handler for paragraph items + save paragraph item.
  field_attach_submit('paragraphs_item', $paragraphs_item, $form, $form_state);
  $paragraphs_item->save($skip_host_save);

  // If a paragraphs item is passed, we want to add the new paragraph after it.
  if ($paragraphs_item && $mode != 'scratch') {
    $new_paragraph = array(
      array(
        'value' => $paragraphs_item->item_id,
        'revision_id' => $paragraphs_item->revision_id,
      ),
    );

    // Load the paragraphs field from the host entity and insert the new paragraph item in the correct position.
    if ($paragraphs = field_get_items($host_entity_type, $host_entity, $paragraphs_item->field_name)) {
      array_splice($paragraphs, $paragraphs_item->position, 0, $new_paragraph);
    }

    // Overwrite the paragraph field + save the host entity.
    $field_name = $paragraphs_item->field_name;
    $host_entity->{$field_name}[LANGUAGE_NONE] = $paragraphs;
    $host_entity->save();
  }

  // Nice confirmation message.
  $paragraphs_bundle = paragraphs_bundle_load($paragraphs_item->bundle);
  backdrop_set_message(t('Paragraph !bundle has been saved.', array('!bundle' => $paragraphs_bundle->name)));
}

/**
 * Close Ajax modal popup for adding a Paragraphs item.
 */
function paragraphs_modal_admin_add_form_close($form, &$form_state) {
  // Close modal dialog upon submit.
  $commands = array(
    '#type' => 'ajax',
    '#commands' => array(),
  );
  $commands['#commands'][] = ajax_command_close_modal_dialog();

  $paragraphs_item = $form_state['values']['paragraphs_item'];
  $host_entity_type = $form_state['values']['host_entity_type'];
  $host_entity_id = $paragraphs_item->hostEntityId();
  $field_name = $paragraphs_item->hostEntityBundle();

  // Refresh display of field that was added.
  if (!empty($host_entity_id) && !empty($host_entity_type)) {
    $field_name = $paragraphs_item->field_name;
    $host_entity = entity_load($host_entity_type, $host_entity_id);
    $fresh_field = field_view_field($host_entity_type, $host_entity, $field_name);
    $new_element = '<div class=\'messages status paragraphs-modal-admin-message\'>' . t('New paragraph has been added.') . '</div>';
    $new_element .= backdrop_render($fresh_field);
    $commands['#commands'][] = ajax_command_html('.paragraphs-items-' . strtr($field_name, '_', '-'), $new_element);
    $commands['#commands'][] = array(
      'command' => 'paragraphs_modal_admin_message_hide',
    );
  }
  return $commands;
}
