<?php
/**
 * @file
 * Paragraphs Type cloning functions.
 */

/**
 * Form constructor for the clone creation.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   Reference to the form state array.
 * @param string $bundle
 *   The machine name of the source paragraph type.
 */
function paragraphs_admin_bundle_clone_create_form(array $form, array &$form_state, $bundle) {

  // Load the paragraph type from machine name.
  $paragraph_type = $bundle;
  // Prepare the new paragraph type machine name prefix.
  $paragraph_type_prefix = $paragraph_type->bundle . '_clone';
  // Get the next occurrence count to use in the names.
  $num = paragraphs_type_clone_get_next_type_number($paragraph_type_prefix);

  // Source fieldset.
  $form['fieldset_source'] = array(
    '#type' => 'fieldset',
    '#title' => t('Paragraphs type source'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  // Source paragraph type name field.
  $form['fieldset_source']['source_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => $paragraph_type->name,
    '#required' => TRUE,
    '#attributes' => array(
      'readonly' => 'readonly',
      'class' => array('paragraphs-type-clone-readonly-field'),
    ),
  );
  // Source paragraph type machine name field.
  $form['fieldset_source']['source_machine_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Machine name'),
    '#default_value' => $paragraph_type->bundle,
    '#required' => TRUE,
    '#attributes' => array(
      'readonly' => 'readonly',
      'class' => array('paragraphs-type-clone-readonly-field'),
    ),
  );
  // Source paragraph type label field.
  $form['fieldset_source']['source_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#default_value' => $paragraph_type->label,
    '#required' => FALSE,
    '#attributes' => array(
      'readonly' => 'readonly',
      'class' => array('paragraphs-type-clone-readonly-field'),
    ),
  );
  // Source paragraph type description field.
  $form['fieldset_source']['source_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => $paragraph_type->description,
    '#attributes' => array(
      'readonly' => 'readonly',
      'class' => array('paragraphs-type-clone-readonly-field'),
    ),
  );
  // Source paragraph type allow_unpublish field.
  $form['fieldset_source']['source_allow_unpublish'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow unpublishing from the admin interface'),
    '#default_value' => $paragraph_type->allow_unpublish,
    '#disabled' => TRUE,
    '#attributes' => array(
      'class' => array('paragraphs-type-clone-readonly-field'),
    ),
  );

  // Target fieldset.
  $form['fieldset_target'] = array(
    '#type' => 'fieldset',
    '#title' => t('Paragraphs type target'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  // Target paragraph type name field.
  $default_value = 'Clone of ' . $paragraph_type->name . ' (' . $num . ')';
  $form['fieldset_target']['target_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => $default_value,
    '#token' => FALSE,
    '#required' => TRUE,
  );
  // Target type machine name field.
  $form['fieldset_target']['target_machine_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Machine name'),
    '#default_value' => $paragraph_type_prefix . '_' . $num,
    '#required' => TRUE,
  );
  // Target paragraph type label field.
  $default_value = 'Clone of ' . $paragraph_type->label . ' (' . $num . ')';
  $form['fieldset_target']['target_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#default_value' => $default_value,
    '#required' => FALSE,
  );
  // Target paragraph type description field.
  $form['fieldset_target']['target_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => $paragraph_type->description,
  );
  // Target paragraph type allow_unpublish field.
  $form['fieldset_target']['target_allow_unpublish'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow unpublishing from the admin interface'),
    '#default_value' => $paragraph_type->allow_unpublish,
  );

  // Submit button.
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Clone Paragraphs type'),
  );

  // Return the form.
  return $form;
}

/**
 * Generate a default item number for a new clone.
 *
 * This function will add the clone number to the machine name and name
 * in the paragraph type creation form.
 *
 * The clone number is based on the number of existing clones
 * for the paragraph type being cloned.
 *
 * @param string $paragraph_type_prefix
 *   The default prefix defined for the new paragraph type.
 */
function paragraphs_type_clone_get_next_type_number($paragraph_type_prefix) {
  // Get paragraph types.
  $paragraph_types = array_keys(paragraphs_bundle_load());

  // Prepare the search pattern.
  $pattern = '/^' . $paragraph_type_prefix . '/';

  // Count existing prefix occurrences.
  $occurrences = count(preg_grep($pattern, $paragraph_types));

  // Return an incremented value.
  return $occurrences + 1;
}

/**
 * Form validation handler for the clone creation form.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   Reference to the form state array.
 */
function paragraphs_admin_bundle_clone_create_form_validate(array $form, array &$form_state) {
  if ($form['#form_id'] == 'paragraphs_admin_bundle_clone_create_form') {
    // Get paragraph types.
    $paragraph_types = array_keys(paragraphs_bundle_load());

    // If the machine name already exists.
    if (in_array($form_state['values']['target_machine_name'], $paragraph_types)) {
      form_set_error('target_machine_name', t('The paragraph type machine name provided already exists'));
    }

    // Add the custom form submit handler.
    $form['#submit'][] = 'paragraphs_admin_bundle_clone_create_form_submit';
  }
}

/**
 * Form submit handler for the clone creation form.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   Reference to the form state array.
 */
function paragraphs_admin_bundle_clone_create_form_submit(array $form, array &$form_state) {
  if ($form['#form_id'] == 'paragraphs_admin_bundle_clone_create_form') {
    $values = $form_state['values'];
    // Execute the function batch function.
    $batch = paragraphs_type_clone_create($values);
    if (empty($batch)) {
      // If $batch is empty (i.e. no fields), skip batch set, set a message and
      // end clone operation.
      backdrop_set_message(
        t('The "@source_name" paragraph type has been cloned successfully to "@target_name".',
          array(
            '@source_name' => $values['source_name'],
            '@target_name' => $values['target_name'],
          )
        )
      );
      // Redirect to the paragraph types list.
      $form_state['redirect'] = 'admin/structure/paragraphs';
    }
    else {
      batch_set($batch);
    }
  }
}

/**
 * Create a paragraph type clone.
 *
 * This function creates the new paragraph type and adds the cloned fields.
 *
 * @param array $values
 *   Form values coming from the paragraph type creation form.
 */
function paragraphs_type_clone_create(array $values) {

  // Create the paragraph type object and populate from the form.
  $bundle = new stdClass();
  $bundle->bundle = $values['target_machine_name'];
  $bundle->locked = 1;
  $bundle->name = trim((string) $values['target_name']);
  $bundle->allow_unpublish = (int) $values['target_allow_unpublish'];
  $bundle->label = (empty($values['target_label'])) ? $bundle->name : trim($values['target_label']);
  $bundle->description = trim((string) $values['target_description']);

  // Save the paragraph type.
  paragraphs_bundle_save($bundle);

  // Prepare the operations array.
  $operations = array();

  // Get paragraph type base fields.
  $base_fields = field_info_instances('paragraphs_item', $values['source_machine_name']);

  // Check to see if fields exist before completing field actions.
  if (!empty($base_fields)) {
    // Get paragraph type extra fields.
    $extra_fields = field_info_extra_fields('paragraphs_item', $values['source_machine_name'], 'form');

    // Merge all fields.
    $all_fields = array_merge($base_fields, $extra_fields);

    // Create new fields.
    foreach ($all_fields as $name => $instance) {
      if (isset($name) && !empty($name) && isset($instance['entity_type']) && !empty($instance['entity_type'])) {
        $operations[] = array(
          'paragraphs_type_clone_set_target_field',
          array($instance, $name, $values),
        );
      }
    }

    // Check if the Field Group module is installed.
    if (module_exists('field_group')) {
      // Get the groups from the form view and all display view modes.
      $groups = field_group_info_groups('paragraphs_item', $values['source_machine_name'], NULL, TRUE);
      // Loop through each display mode including form.
      $view_modes = array_keys($groups);
      foreach ($view_modes as $view_mode) {
        // Loop through each group within the display mode.
        foreach ($groups[$view_mode] as $group) {
          // Set the bundle (paragraph type) to be the new type machine name.
          $group->bundle = $values['target_machine_name'];
          // Save the new group configuration.
          field_group_group_save($group, $new = TRUE);
        }
      }
    }
  }

  // Check if the Paragraphs Type Permissions module is enabled so we can clone
  // permissions if they exist.
  if (module_exists('paragraphs_bundle_permissions')) {
    // Get all the roles defined in the site.
    $roles = user_roles();
    // Define the avaiable permission types for each paragraph type.
    $permission_types = array('view', 'update', 'delete', 'create');
    // Loop through each role on the site and get the existing permissions.
    foreach ($roles as $role_name => $role_label) {
      $role_permissions = user_role_permissions(array($role_name));
      // Reset the list of target permissions for each role.
      $target_permissions = array();
      // Loop through the permission types to check if the source type has
      // permissions for this action on this role.
      foreach ($permission_types as $permission_type) {
        // Define the source paragraph type permission to check for.
        $permission_name = $permission_type . ' paragraph content ' . $values['source_machine_name'];
        // Check if the role has this permission.
        if (in_array($permission_name, $role_permissions)) {
          // Add this to the list of target permissions to add for this role.
          $target_permission_name = $permission_type . ' paragraph content ' . $bundle->bundle;
          $target_permissions[$target_permission_name] = TRUE;
        }
      }
      // Add all target_permissions to this role.
      user_role_change_permissions($role_name, $target_permissions);
    }
  }

  // Define batch operation if fields exist and therefore operations set.
  if (!empty($operations) > 0) {
    $batch = array(
      'title' => t('Clone Paragraphs type'),
      'operations' => $operations,
      'file' => backdrop_get_path('module', 'paragraphs') . '/paragraphs.type_clone.inc',
      'finished' => 'paragraphs_type_clone_finished',
      'init_message' => t('Operation in progress...'),
      'progress_message' => t('Task @current/@total'),
      'error_message' => t('An error has occurred please try again.'),
    );
    return $batch;
  }
  else {
    return FALSE;
  }
}

/**
 * Create a field.
 *
 * This function creates a field for a cloned paragraph type.
 *
 * @param object $instance
 *   The newly created paragraph type.
 * @param string $field_name
 *   The name of the field in the source paragraph type.
 * @param array $values
 *   Form values coming from the paragraph type creation form.
 */
function paragraphs_type_clone_set_target_field($instance, $field_name, array $values, &$context) {
  // Progress context results.
  $context['results'][] = $field_name;

  // Pass the cloned paragraph type name to context.
  $context['results']['source_name'] = $values['source_name'];
  $context['results']['target_name'] = $values['target_name'];

  // Update the field count.
  if (!isset($context['results']['field_count'])) {
    $context['results']['field_count'] = 0;
  }
  $context['results']['field_count']++;

  // Progress context message.
  $context['message'] = t('Cloning field @field',
    array(
      '@field' => $field_name,
    )
  );

  // Verify the field does not already exist.
  if (!_paragraphs_type_clone_exclude_field($field_name)) {
    // If field doesn't exist, create it.
    if (!field_info_field($field_name)) {
      if ($field = field_info_field($field_name)) {
        $field = field_create_field($field);
      }
    }

    // Set the new bundle (paragraph type) name.
    $instance['bundle'] = $values['target_machine_name'];
    $instance['field_name'] = $field_name;

    // Create an instance of the field and bind it to the bundle.
    field_create_instance($instance);
  }
}

/**
 * Checks if a field should be excluded.
 *
 * @param string $field_name
 *   The field name.
 *
 * @return bool
 *   Number of requests.
 */
function _paragraphs_type_clone_exclude_field($field_name) {
  $exclude = array('title', 'body', 'metatags', 'locations');
  return in_array($field_name, $exclude);
}

/**
 * Batch operation finished.
 *
 * This function displays a message after the batch operation ends.
 *
 * @param bool $success
 *   The batch status.
 * @param array $results
 *   The batch results array.
 * @param array $operations
 *   The batch operations array.
 */
function paragraphs_type_clone_finished($success, array $results, array $operations) {
  // Test the results.
  if ($success) {
    backdrop_set_message(
      t('The "@source_name" paragraph type and @field_count fields have been cloned successfully to "@target_name".',
        array(
          '@source_name' => $results['source_name'],
          '@target_name' => $results['target_name'],
          '@field_count' => $results['field_count'],
        )
      )
    );
  }
  else {
    $error_operation = reset($operations);
    backdrop_set_message(
      t('An error occurred while processing @operation with arguments : @args',
        array(
          '@operation' => $error_operation[0],
          // phpcs:ignore Squiz.PHP.DiscouragedFunctions -- This is used widely in debugging.
          '@args' => print_r($error_operation[0], TRUE),
        )
      )
    );
  }

  // Redirect to the paragraph types list.
  backdrop_goto('admin/structure/paragraphs');
}
