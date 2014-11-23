<?php

/**
 * @file
 * Drush commands for the API module.
 */

/**
 * Implements hook_drush_command().
 */
function api_drush_command() {
  return array(
    'api-reparse' => array(
      'description' => 'Mark API files to reparse',
      'aliases' => array('apirp'),
      'arguments' => array(
        'branch_or_file' => 'Optional branch ID number or file path relative to the common directory of its branch; omitted means mark everything',
      ),
    ),
    'api-list-branches' => array(
      'aliases' => array('apilb'),
      'description' => 'List API branches',
    ),
    'api-reset-queue' => array(
      'aliases' => array('apirq'),
      'description' => 'Reset the API parsing queue',
    ),
    'api-count-queues' => array(
      'aliases' => array('apicq'),
      'description' => 'Count contents of API queues',
    ),
    'api-ensure-branch' => array(
      'aliases' => array('apieb'),
      'description' => 'Add or update an API branch and project',
      'required-arguments' => TRUE,
      'arguments' => array(
        'project' => 'Project name',
        'project_title' => 'Project title',
        'project_type' => 'Project type (core or module)',
        'branch' => 'Branch name',
        'branch_title' => 'Branch title',
        'directory' => 'File directory',
        'core_compatibility' => 'Core compatibility',
        'update_frequency' => 'Minimum update frequency, in seconds',
      ),
    ),
  );
}

/**
 * Flags a branch, or all branches, to be reparsed on the next cron run.
 *
 * @param $branch_or_file
 *   (optional) Identifier of the branch to reparse, or name of a single file to
 *   reparse. If omitted all branches will be reparsed. File name must include
 *   the path relative to the common path to the directories indexed by this
 *   branch.
 */
function drush_api_reparse($branch_or_file = NULL) {
  $num = api_mark_for_reparse($branch_or_file);

  drush_log(dt('Marked @number files for reparsing.', array('@number' => $num)), 'ok');
}

/**
 * Resets the parsing queue.
 */
function drush_api_reset_queue() {
  api_reset_parse_queue();

  drush_log(dt('API parse queue reset. Necessary jobs will be added in the next cron run'), 'ok');
}

/**
 * Counts the API queues.
 */
function drush_api_count_queues() {
  $rows = array();

  $queue = DrupalQueue::get('api_parse');
  $num = $queue->numberOfItems();
  $rows[] = array(dt('Parse files'), $num);

  $queue = DrupalQueue::get('api_branch_update');
  $num = $queue->numberOfItems();
  $rows[] = array(dt('Branch update'), $num);

  $queue = DrupalQueue::get('api_node_delete');
  $num = $queue->numberOfItems();
  $rows[] = array(dt('Node cleanup'), $num);

  drush_print_table($rows, TRUE);
}

/**
 * Lists all API branches.
 */
function drush_api_list_branches() {
  $branches = api_get_branches();
  $rows = array(array(dt('ID'), dt('Project'), dt('Name'), dt('Location')));
  foreach ($branches as $branch) {
    $rows[] = array(
      $branch->branch_id,
      $branch->project,
      $branch->branch_name,
      $branch->directories,
    );
  }
  drush_print_table($rows, TRUE);
}

/**
 * Adds or updates an API branch, and project if it doesn't already exist.
 */
function drush_api_ensure_branch($project, $project_title, $project_type, $branch_name, $branch_title, $directory, $core_compatibility, $update_frequency) {

  // Update the project.
  api_save_project((object) array('project_name' => $project, 'project_title' => $project_title, 'project_type' => $project_type));

  // Try to load the branch if possible.
  $branch = api_get_branch_by_name($project, $branch_name);
  if ($branch) {
    if (!empty($branch->data)) {
      $branch->data = unserialize($branch->data);
    }
    else {
      $branch->data = array();
    }
  }
  else {
    // This is a new branch.
    $branch = new stdClass();
    $branch->branch_id = 0;
    $branch->project = $project;
    $branch->branch_name = $branch_name;
  }

  // Update with the provided information.
  $branch->title = $branch_title;
  $branch->data['directories'] = $directory;
  $branch->core_compatibility = $core_compatibility;
  $branch->update_frequency = $update_frequency;

  api_save_branch($branch);
  drush_log(dt('Saved branch and project.'), 'ok');
}
