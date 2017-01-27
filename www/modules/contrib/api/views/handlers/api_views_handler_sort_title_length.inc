<?php

/**
 * @file
 * Defines a handler for sorting by the length of {api_documentation}.title.
 */

/**
 * Handles sorting by the length of {api_documentation}.title.
 */
class api_views_handler_sort_title_length extends views_handler_sort {

  /**
   * Called to add the sort to a query.
   */
  function query() {
    $this->ensure_my_table();
    // Add the field.
    $this->query->add_orderby(NULL, 'LENGTH(' . $this->table_alias . '.title)', $this->options['order'], 'title_length');
  }

}
