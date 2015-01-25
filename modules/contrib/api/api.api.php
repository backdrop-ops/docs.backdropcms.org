<?php

/**
 * @file
 * Hooks for the API module.
 */

/**
 * React to API documentation being updated.
 *
 * Note that when API documentation is first added, a node will be created,
 * so you can use the node hooks to react to that (the node type will be
 * 'api'). Similarly, when API documentation is removed (because the function,
 * class, etc. no longer exists), the node will be deleted, so you can use node
 * hooks to react. In between creation and deletion, this hook can be used to
 * detect that the documentation has been updated.
 *
 * @param int[] $dids
 *   Array of the documentation IDs of documentation that has been updated.
 */
function hook_api_updated($dids) {
  foreach ($dids as $did) {
    mymodule_mark_api_as_updated($did);
  }
}

/**
 * Tell the API module to ignore node access for a query.
 *
 * Normally, the API module modifies all node listing queries to exclude API
 * nodes. You can use this hook to turn off that exclusion for a query.
 *
 * @param $query
 *   The query that is being altered.
 *
 * @return bool
 *   TRUE if the API module should skip its normal alteration of the query.
 */
function hook_api_ignore_node_access($query) {
  // Check something on the query, such as that it is a query on the
  // search_index table.
  $tables = $query->getTables();
  $first = reset($tables);
  if (is_array($first) && isset($first['table']) && $first['table'] == 'search_index') {
    return TRUE;
  }

  return FALSE;
}


