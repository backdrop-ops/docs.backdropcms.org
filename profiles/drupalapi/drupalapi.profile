<?php
/**
 * @file
 * Install profile for DrupalAPI.
 */

/**
 * Implements hook_form_FORM_ID_alter().
 */
function drupalapi_form_install_configure_form_alter(&$form, $form_state) {
  $form['site_information']['site_name']['#default_value'] = t('DrupalAPI');
}

/**
 * Implements hook_install_tasks().
 */
function drupalapi_install_tasks() {
  $tasks = array();

  $tasks['drupalapi_api_setup'] = array(
    'display_name' => st('Index API'),
    'type' => 'batch',
    'run' => php_sapi_name() !== 'cli' ? INSTALL_TASK_RUN_IF_NOT_COMPLETED : INSTALL_TASK_SKIP,
  );

  return $tasks;
}

/**
 * DrupaAPI API setup installation task callback.
 */
function drupalapi_api_setup() {
  // Ensure that projects and branches only get automatically setup on install.
  $branches = api_get_branches();
  if (empty($branches)) {
    $projects = drupalapi_core_get_projects();
    foreach ($projects as $project) {
      // Setup API project.
      $api_project = new stdClass();
      $api_project->project_name = $project['name'];
      $api_project->project_title = $project['title'];
      $api_project->project_type = $project['type'];
      api_save_project($api_project);

      // Setup API branches.
      foreach ($project['branches'] as $version => $branch) {
        $api_branch = new stdClass();
        $api_branch->branch_name = $version;
        $api_branch->core_compatibility = $branch['core_compatibility'];
        $api_branch->preferred = $branch['preferred'];
        $api_branch->project = $project['name'];
        $api_branch->data['directories'] = '';
        foreach ($branch['directories'] as $dir) {
          $api_branch->data['directories'] .= $dir . "\n";
        }
        $api_branch->title = $project['title'] . ' ' . $version;
        api_save_branch($api_branch);
      }
    }
  }

  // Return Batch array.
  return array(
    'title' => t('Indexing DrupalAPI reference'),
    'operations' => array(
      array('drupalapi_api_batch', array())
    ),
    'finished' => 'drupalapi_api_batch_finished',
  );
}

/**
 * Operation callback for DrupalAPI Indexing batch.
 */
function drupalapi_api_batch(&$context) {
  // Try to increase the maximum execution time if it is too low.
  if (ini_get('max_execution_time') < 240 && !ini_get('safe_mode')) {
    set_time_limit(240);
  }

  // Initialize batch.
  if (!isset($context['sandbox']['progress'])) {
    // Update all branches.
    module_load_include('inc', 'api', 'parser');
    api_update_all_branches();

    // Run API update queue items.
    $context['sandbox']['api_cron_queue_info'] = api_cron_queue_info();
    $function = $context['sandbox']['api_cron_queue_info']['api_branch_update']['worker callback'];
    $queue = DrupalQueue::get('api_branch_update');
    while ($item = $queue->claimItem()) {
      $function($item->data);
      $queue->deleteItem($item);
    }

    // Determine how many files need parsing.
    $queue = DrupalQueue::get('api_parse');
    $context['sandbox']['max'] = $queue->numberOfItems();
    $context['sandbox']['progress'] = 0;
  }

  // Process abtch.
  else {
    // Run 'API Parse' queue items.
    $function = $context['sandbox']['api_cron_queue_info']['api_parse']['worker callback'];
    $end = time() + (isset($info['api_parse']['time']) ? $info['api_parse']['time'] : 15);
    $queue = DrupalQueue::get('api_parse');
    while (time() < $end && ($item = $queue->claimItem())) {
      $function($item->data);
      $queue->deleteItem($item);
    }
  }

  // Determine how many files still need parsing.
  $queue = DrupalQueue::get('api_parse');
  $context['sandbox']['progress'] = $context['sandbox']['max'] - $queue->numberOfItems();

  // Provide status and progress information.
  $function = function_exists('st') ? 'st' : 't';
  $context['message'] = $function('!progress of !max files indexed.', array(
    '!progress' => $context['sandbox']['progress'],
    '!max' => $context['sandbox']['max'],
  ));
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * Finished callback for DrupalAPI Indexing batch.
 */
function drupalapi_api_batch_finished($success, $results) {
  variable_set('drupalapi_indexed', TRUE);
}
