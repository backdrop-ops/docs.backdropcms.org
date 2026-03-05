<?php
/**
 * @file
 * Definition of draft_workflow_handler_field_latest_revision_log.
 */

/**
 * Field handler to display the log message from the most recent draft revision
 * (i.e. the newest revision with a vid greater than the current active vid).
 *
 * @ingroup views_field_handlers
 */
class draft_workflow_handler_field_latest_revision_log extends views_handler_field {

  function query() {
    $formula = '(SELECT log FROM {node_revision} WHERE nid = node.nid AND vid > node.vid ORDER BY vid DESC LIMIT 1)';
    $this->field_alias = $this->query->add_field(NULL, $formula, 'draft_workflow_latest_log');
  }

  function render($values) {
    $value = $values->{$this->field_alias};
    return $value ? filter_xss($value) : '';
  }
}
