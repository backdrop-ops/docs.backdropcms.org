<?php
/**
 * @file
 * Definition of draft_workflow_handler_filter_forward_revisions.
 */

/**
 * Filter to nodes that have (or do not have) revisions newer than the current
 * active revision.
 *
 * @ingroup views_filter_handlers
 */
class draft_workflow_handler_filter_forward_revisions extends views_handler_filter_boolean_operator {

  function construct() {
    parent::construct();
    $this->value_value = t('Has forward revisions');
  }

  function get_value_options() {
    $this->value_options = array(
      1 => t('Yes'),
      0 => t('No'),
    );
  }

  function query() {
    if (!empty($this->value)) {
      // Include nodes that have at least one revision newer than the current.
      $this->query->add_where_expression(
        $this->options['group'],
        'EXISTS (SELECT 1 FROM {node_revision} nr WHERE nr.nid = node.nid AND nr.vid > node.vid)'
      );
    }
    else {
      // Include nodes that have no revisions newer than the current.
      $this->query->add_where_expression(
        $this->options['group'],
        'NOT EXISTS (SELECT 1 FROM {node_revision} nr WHERE nr.nid = node.nid AND nr.vid > node.vid)'
      );
    }
  }
}
