<?php

/**
 * @file
 * Defines a handler for the project operations for API documentation.
 */

/**
 * Handles the project ops field for API documentation.
 *
 * Makes edit and delete links.
 */
class api_views_handler_field_api_project_ops extends views_handler_field {

  /**
   * Overrides views_handler_field::init().
   *
   * Adds additional database fields that we need.
   */
  function init(&$view, &$options) {
    parent::init($view, $options);
    $this->additional_fields['project_name'] = array('table' => 'api_project', 'field' => 'project_name');
  }

  /**
   * Overrides views_handler_field::query().
   *
   * This is a fake field, so don't add it to the query.
   */
  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  /**
   * Overrides views_handler_field::render().
   */
  function render($values) {
    $project_name = check_plain($this->get_value($values, 'project_name'));
    $ops = array();
    $ops[] = l(t('edit'), 'admin/config/development/api/projects/' . $project_name);
    $ops[] = l(t('delete'), 'admin/config/development/api/projects/' . $project_name . '/delete');
    return implode(' ', $ops);
  }
}
