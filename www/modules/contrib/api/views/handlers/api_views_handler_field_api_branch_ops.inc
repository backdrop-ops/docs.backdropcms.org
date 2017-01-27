<?php

/**
 * @file
 * Defines a handler for the branch operations for API documentation.
 */

/**
 * Handles the branch ops field for API documentation.
 *
 * Makes edit, reparse, update, and delete links.
 */
class api_views_handler_field_api_branch_ops extends views_handler_field {

  /**
   * Overrides views_handler_field::init().
   *
   * Adds additional database fields that we need.
   */
  function init(&$view, &$options) {
    parent::init($view, $options);
    $this->additional_fields['branch_id'] = array('table' => 'api_branch', 'field' => 'branch_id');
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
    $branch_id = (int) $this->get_value($values, 'branch_id');

    $ops = array();
    $ops[] = l(t('edit'), 'admin/config/development/api/branches/' . $branch_id);
    $ops[] = l(t('reparse'), 'admin/config/development/api/branches/' . $branch_id . '/reparse');
    $ops[] = l(t('update'), 'admin/config/development/api/branches/' . $branch_id . '/update');
    $ops[] = l(t('delete'), 'admin/config/development/api/branches/' . $branch_id . '/delete');

    return implode(' ', $ops);
  }
}
