<?php

/**
 * @file
 * Definition of api_views_handler_filter_service_name.
 */

/**
 * Filter handler for service tags.
 */
class api_views_handler_filter_service_tag extends views_handler_filter_string {

  function query() {
    $this->ensure_my_table();

    // Join to api_reference_storage.
    $def = array(
      'table' => 'api_reference_storage',
      'field' => 'from_did',
      'left_table' => $this->table_alias,
      'left_field' => 'did',
    );
    $join = new views_join();
    $join->definition = $def;
    $join->construct();
    $join->adjusted = TRUE;
    $this->table_alias = $this->query->add_table('api_reference_storage', $this->relationship, $join);
    $this->real_field = 'object_name';

    parent::query();
  }
}

