<?php

/**
 * @file
 * Definition of api_views_handler_filter_project_name.
 */

/**
 * Filter handler for project names.
 */
class api_views_handler_filter_project_name extends views_handler_filter_string {
  function value_form(&$form, &$form_state) {
    $default_value = $this->value;

    $form['value'] = array(
      '#type' => 'textfield',
      '#title' => t('Project'),
      '#default_value' => $default_value,
      '#autocomplete_path' => 'api/project/autocomplete',
    );
  }
}

