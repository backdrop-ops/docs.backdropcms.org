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
    t('Machine Name'),
    t('Operations'),
  );
  $rows = array();
  $operations = array();
  foreach ($bundles as $bundle) {

    $type_url_str = strtr($bundle->bundle, array('_' => '-'));
    $row = array(
      array(
        'data' => $bundle->name,
      ),
      array(
        'data' => $bundle->bundle,
      ),
    );

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
 * Returns HTML for a paragraphs bundle description.
 *
 * @param array $variables
 *   An array with one item, containing the paragraphs bundle object.
 *   $variables['bundle'] is an object with the following properties:
 *   - name: The human-readable name of the bundle.
 *   - bundle: The machine name of the bundle.
 *   - label: The bundle's label when displayed in edit forms.
 *   - description: A text description of the bundle.
 *
 * @see paragraphs_admin_bundle_overview()
 * @ingroup themeable
 */
function theme_paragraphs_admin_overview(array $variables) {
  $bundle = $variables['bundle'];

  $output = check_plain($bundle->name);
  $output .= ($bundle->name != $bundle->label) ? ' [<em>' . check_plain($bundle->label) . '</em>]' : '';
  $output .= ' <small>' . t('(Machine name: @bundle)', array('@bundle' => $bundle->bundle)) . '</small>';
  $output .= !empty($bundle->description) ? '<div class="description">' . filter_xss_admin($bundle->description) . '</div>' : '';

  return $output;
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
  }
  else {
    if (!$bundle) {
      backdrop_set_message(t('Could not load Paragraph type'), 'error');
      backdrop_goto('admin/structure/paragraphs');
    }
  }

  $form['#paragraphs_bundle'] = $bundle;

  $form['name'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $bundle->name,
    '#description' => t('Label for the Paragraph type.'),
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
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $bundle->label,
    '#description' => t('The label for this bundle as it will appear to users on edit forms. Defaults to bundle name if left empty.'),
    '#size' => 30,
  );

  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textarea',
    '#default_value' => $bundle->description,
    '#description' => t('Describe this bundle. The text will be displayed on the Paragraphs admin overview page.'),
  );

  $form['locked'] = array(
    '#type' => 'value',
    '#value' => $bundle->locked,
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
