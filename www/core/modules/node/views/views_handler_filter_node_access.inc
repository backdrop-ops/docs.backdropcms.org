<?php
/**
 * @file
 * Definition of views_handler_filter_node_access.
 */

/**
 * Filter by node_access records.
 *
 * @ingroup views_filter_handlers
 */
class views_handler_filter_node_access extends views_handler_filter {
  function admin_summary() { }
  function operator_form(&$form, &$form_state) { }
  function can_expose() {
    return FALSE;
  }

  /**
   * See _node_access_where_sql() for a non-views query based implementation.
   */
  function query() {
    if (!user_access('administer nodes') && module_implements('node_grants')) {
      $table = $this->ensure_my_table();
      $grants = node_add_node_grants_to_query(node_access_grants('view'));

      $this->query->add_where('AND', $grants);
      $this->query->add_where('AND', $table . '.grant_view', 1, '>=');
    }
  }
}
