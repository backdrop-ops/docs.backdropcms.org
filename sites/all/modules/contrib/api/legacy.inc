<?php

/**
 * @file
 * Redirect deprecated API links.
 */

/**
 * Redirects legacy URLs for files.
 *
 * api/file/{directory}/{file.php}[/{branch}] ->
 * api/{project}/{directory}{separator}{file.php}/{branch}
 *
 * @param ...
 *   One or more menu parts that follow(s) the old style api/file/ path.
 */
function api_file_redirect() {
  $branches = api_get_branches();

  $args = func_get_args();
  if (count($args) == 1) {
    $args = explode('/', $args[0]);
  }

  $suffix = '';
  $file = new stdClass();
  $file->object_type = 'file';
  $file->branch_id = variable_get('api_default_branch', NULL);
  $default_project = $branches[variable_get('api_default_branch', NULL)]->project;

  $end = count($args) - 1;
  if (in_array($args[$end], array('source', 'documentation'))) {
    $suffix = '/' . $args[$end];
    $end -= 1;
  }
  foreach ($branches as $b) {
    if ($b->branch_name === $args[$end] && $b->project === $default_project) {
      $file->branch_id = $b->branch_id;
      $end -= 1;
      break;
    }
  }
  $file->object_name = implode('/', array_slice($args, 0, $end + 1));

  drupal_goto(api_url($file) . $suffix);
}

/**
 * Redirects old search URL.
 */
function api_legacy_search_redirect($branch, $search_term) {
  drupal_goto('api/' . $branch->project . '/' . $branch->branch_name . '/search/' . $search_term);
}
