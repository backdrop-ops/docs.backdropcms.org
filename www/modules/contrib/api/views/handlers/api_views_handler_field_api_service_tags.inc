<?php

/**
 * @file
 * Defines a handler for service tags for API documentation.
 */

/**
 * Handles service tags for API documentation.
 *
 * Similar to the "All taxonomy terms" field.
 */
class api_views_handler_field_api_service_tags extends views_handler_field_prerender_list {

  function init(&$view, &$options) {
    parent::init($view, $options);
    $this->additional_fields['did'] = array('table' => 'api_documentation', 'field' => 'did');
  }

  function query() {
    $this->add_additional_fields();
  }

  function pre_render(&$values) {
    // This is mostly borrowed from the taxonomy handler.
    $this->field_alias = $this->aliases['did'];
    $dids = array();
    foreach ($values as $result) {
      if (!empty($result->{$this->aliases['did']})) {
        $dids[] = $result->{$this->aliases['did']};
      }
    }

    if ($dids) {
      $query = db_select('api_reference_storage', 'ars');
      $query->condition('from_did', $dids);
      $query->condition('object_type', 'service_tag');
      $query->fields('ars', array('object_name', 'from_did'));
      $result = $query->execute();

      foreach ($result as $tag) {
        $this->items[$tag->from_did][$tag->object_name]['name'] = $tag->object_name;
      }
    }
  }

  function render_item($count, $item) {
    return $item['name'];
  }
}
