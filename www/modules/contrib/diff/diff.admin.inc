<?php

/**
 * @file
 * Administration page callbacks and forms.
 */

/**
 * General configuration form for controlling the diff behaviour.
 */

function diff_admin_settings($form, $form_state) {
  $help_markup = t('The Diff module replaces the normal <em>Revisions</em> node tab.');
  $help_markup .= t(' Diff enhances the listing of revisions with an option to view the differences between any two content revisions.');
  $help_markup .= t(' Access to this feature is controlled with the <em>View revisions</em> permission.');
  $help_markup .= t(' The feature can be enabled/disabled with <em>Compare revisions</em> settings available on the configuration page for each content type.');
  $help_markup .= t(' Diff also provides an optional <em>View changes</em> button while editing a node.');

  $form['help'] = array(
    '#type' => 'help',
    '#markup' => $help_markup,
  );
  $form['diff_theme'] = array(
    '#type' => 'select',
    '#title' => t('CSS options'),
    '#default_value' => config_get('diff.settings','diff_theme'),
    '#options' => array(
      'default' => t('Classic'),
      'boxes' => t('Boxes'),
    ),
    '#empty_option' => t('- None -'),
    '#description' => t('Alter the CSS used when displaying diff results.'),
  );
  $form['diff_radio_behavior'] = array(
    '#type' => 'select',
    '#title' => t('Diff radio behavior'),
    '#default_value' => config_get('diff.settings','diff_radio_behavior'),
    '#options' => array(
      'simple' => t('Simple exclusion'),
      'linear' => t('Linear restrictions'),
    ),
    '#empty_option' => t('- None -'),
    '#description' => t('<em>Simple exclusion</em> means that users will not be able to select the same revision, <em>Linear restrictions</em> means that users can only select older or newer revisions of the current selections.'),
  );

  $options = backdrop_map_assoc(array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
  $form['diff_context_lines_leading'] = array(
    '#type' => 'select',
    '#title' => t('Leading context lines'),
    '#description' => t('This governs the number of unchanged leading context "lines" to preserve.'),
    '#default_value' => config_get('diff.settings','diff_context_lines_leading'),
    '#options' => $options,
  );
  $form['diff_context_lines_trailing'] = array(
    '#type' => 'select',
    '#title' => t('Trailing context lines'),
    '#description' => t('This governs the number of unchanged trailing context "lines" to preserve.'),
    '#default_value' => config_get('diff.settings','diff_context_lines_trailing'),
    '#options' => $options,
  );

  // Add a submit button
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Submit handler for module_settings_form().
 */
function diff_admin_settings_submit($form, &$form_state){
  $config = config('diff.settings');
  $config->set('diff_theme', $form_state['values']['diff_theme']);
  $config->set('diff_radio_behavior', $form_state['values']['diff_radio_behavior']);
  $config->set('diff_context_lines_leading', $form_state['values']['diff_context_lines_leading']);
  $config->set('diff_context_lines_trailing', $form_state['values']['diff_context_lines_trailing']);
  $config->save();
  backdrop_set_message(t('The configuration options have been saved.'));
}

/**
 * Global entity settings.
 */
function diff_admin_global_entity_settings($form, $form_state, $entity_type) {
  $entity_info = entity_get_info($entity_type);
  backdrop_set_title(t('Diff settings for %entity_label entities', array('%entity_label' => $entity_info['label'])), PASS_THROUGH);

  if ($options = module_invoke_all('entity_diff_options', $entity_type)) {
    $form['diff_additional_options_' . $entity_type] = array(
      '#type' => 'checkboxes',
      '#title' => t('Property options'),
      '#default_value' => config_get('diff.settings',"diff_additional_options.$entity_type"),
      '#options' => $options,
    );
  }
  else {
    $form['diff_additional_options_' . $entity_type] = array(
      '#type' => 'value',
      '#value' => array(),
    );
  }

  $form['diff_show_header_' . $entity_type] = array(
    '#type' => 'checkbox',
    '#title' => t('Show entity label row header'),
    '#default_value' => config_get('diff.settings',"diff_show_header.$entity_type"),
  );
  if (user_access('administer permissions')) {
    $admin_link = l(t('View the administration theme'), 'admin/people/permissions', array('fragment' => 'edit-view-the-administration-theme'));
  }
  else {
    $admin_link = t('View the administration theme');
  }
  $form['diff_admin_path_' . $entity_type] = array(
    '#type' => 'checkbox',
    '#title' => t('Use administration theme'),
    '#description' => t('This option will enable users with the <em>!link</em> permission to use the admin theme when doing comparisons.', array('!link' => $admin_link)),
    '#default_value' => config_get('diff.settings',"diff_admin_path.$entity_type"),
  );
  $form['diff_default_state_' . $entity_type] = array(
    '#type' => 'select',
    '#title' => t('Diff default state'),
    '#default_value' => config_get('diff.settings',"diff_default_state.$entity_type"),
    '#options' => array(
      'raw' => t('HTML view'),
      'raw_plain' => t('Plain view'),
    ),
    '#empty_option' => t('- None -'),
    '#description' => t('Default display to show when viewing a diff, html tags in diffed result or as plain text.'),
  );

  // Add a submit button
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Submit handler for global_entity_settings_form().
 */
function diff_admin_global_entity_settings_submit($form, &$form_state){
  $config = config('diff.settings');
  $entity_type = $form_state['build_info']['args'][0];
  $config->set("diff_additional_options.$entity_type", $form_state['values']['diff_additional_options_' . $entity_type]);
  $config->set("diff_show_header.$entity_type", $form_state['values']['diff_show_header_' . $entity_type]);
  $config->set("diff_admin_path.$entity_type", $form_state['values']['diff_admin_path_' . $entity_type]);
  $config->set("diff_default_state.$entity_type", $form_state['values']['diff_default_state_' . $entity_type]);
  $config->save();
  backdrop_set_message(t('The configuration options have been saved.'));
}


/**
 * Menu callback - provides an overview of Diff support and global settings.
 */
function diff_admin_field_overview() {
  $build['info'] = array(
    '#markup' => '<p>' . t('This table provides a summary of the field type support found on the system. It is recommended that you use global settings whenever possible to configure field comparison settings.') . '</p>',
  );

  $header = array(t('Type'), t('Module'), t('Operations'));
  $rows = array();

  // Skip field types which have no widget types.
  $field_types = field_info_field_types();
  $widgets = array();
  foreach (field_info_widget_types() as $name => $widget_type) {
    foreach ($widget_type['field types'] as $widget_field_type) {
      if (isset($field_types[$widget_field_type])) {
        $widgets[$widget_field_type][$name] = $widget_type['label'];
      }
    }
  }

  foreach ($field_types as $field_name => $field_type) {
    if (!empty($widgets[$field_name])) {
      $row = array();
      $row[] = t('@field_label (%field_type)', array(
        '@field_label' => $field_type['label'],
        '%field_type' => $field_name,
      ));
      $row[] = $field_type['module'];
      $row[] = l(t('Global settings'), 'admin/config/content/diff/fields/' . $field_name);
      $rows[] = $row;
    }
  }

  $build['category_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('The system has no configurable fields.'),
  );
  return $build;
}

/**
 * Menu form callback for the field settings.
 */
function diff_admin_global_field_settings($form, $form_state, $type) {
  module_load_include('diff.inc', 'diff');

  $field_types = field_info_field_types();
  if (!isset($field_types[$type])) {
    backdrop_set_message(t('Invalid field type.'), 'error');
    backdrop_goto('admin/config/content/diff/fields');
  }
  $field_type = $field_types[$type];

  // Set the title to give more context to this page.
  backdrop_set_title(t('Global settings for %label fields', array(
    '%label' => $field_type['label'],
  )), PASS_THROUGH);

  $variable_name = "diff_{$field_type['module']}_field_{$type}_default_options";
  $setting_name = substr($variable_name, 5);
  $setting_name = substr($setting_name, 0,- 16);
  $settings = config_get('diff.settings', "diff_field_default_options.$setting_name");
  $settings = _diff_field_default_settings($field_type['module'], $type, $settings);
  $func = $field_type['module'] . '_field_diff_options_form';
  if (function_exists($func) && ($options_form = $func($type, $settings))) {
    $form[$variable_name] = $options_form;
  }
  $form[$variable_name]['#tree'] = TRUE;

  diff_global_settings_form($form[$variable_name], $form_state, $type, $settings);

  // Add a submit button
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Submit handler for global_field_settings_form().
 */
function diff_admin_global_field_settings_submit($form, &$form_state){

  form_state_values_clean($form_state);
	$config = config('diff.settings');
  foreach ($form_state['values'] as $key => $value) {
    if (is_array($value) && isset($form_state['values']['array_filter'])) {
      $value = array_keys(array_filter($value));
    }
    $setting_name = substr($key, 5);
    $setting_name = substr($setting_name, 0,- 16);
    $config->set("diff_field_default_options.$setting_name", $value);
  }
  $config->save();
  backdrop_set_message(t('The configuration options have been saved.'));
}
