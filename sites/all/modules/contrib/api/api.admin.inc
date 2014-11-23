<?php

/**
 * @file
 * Administrative pages for the API module.
 */

/**
 * Builds the overview page for API module administration.
 *
 * @return
 *   Render array for the page.
 */
function api_page_admin_overview() {
  // Turn on cron for the API module.
  variable_set('api_do_cron', 1);

  $output = array();

  // Display settings form.
  $output['admin_form'] = drupal_get_form('api_page_admin_form');

  $output['status'] = array(
    '#type' => 'markup',
    '#markup' => '<h3>' . t('Status') . '</h3>',
  );

  // Display library status.
  if (api_libraries_loaded(FALSE)) {
    $output['library_status'] = array(
      '#type' => 'markup',
      '#markup' => '<p>' . t('Required parsing libraries are present.') . '</p>',
    );
  }
  else {
    $output['library_status'] = array(
      '#type' => 'markup',
      '#markup' => '<p>' . t('The Grammar Parser library is not present. You will not be able to parse API files until you install it. See README.txt for a link to installation instructions for this version of the API module.') . '</p>',
    );
  }

  // Display parsing status.
  $header = array(
    t('Queue'),
    t('Number of jobs'),
    t('Operations'),
  );
  $rows = array();

  $queue = DrupalQueue::get('api_parse');
  $num = $queue->numberOfItems();
  $rows[] = array(
    t('File parse'),
    $num,
    l(t('reset'), 'admin/config/development/api/reset_queue', array('attributes' => array('title' => t('Reset file parse queue')))),
  );

  $queue = DrupalQueue::get('api_branch_update');
  $num = $queue->numberOfItems();
  $rows[] = array(
    t('Branch update'),
    $num,
    ''
  );

  $queue = DrupalQueue::get('api_node_delete');
  $num = $queue->numberOfItems();
  $rows[] = array(
    t('Node cleanup'),
    $num,
    ''
  );

  $output['queue_status'] = array(
    '#type' => 'markup',
    '#markup' => theme('table', array('rows' => $rows, 'header' => $header)),
  );

  return $output;
}

/**
 * Form constructor for the API overview form.
 *
 * @see api_page_admin_form_submit()
 * @ingroup forms
 */
function api_page_admin_form($form, &$form_state) {
  $form['api_default_core_compatibility'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('api_default_core_compatibility', '7.x'),
    '#title' => t('Default core compatibility'),
    '#required' => TRUE,
    '#description' => t('The core compatibility of the default branch/version to display if none is given in a URL'),
  );

  $form['api_default_project'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('api_default_project', 'drupal'),
    '#title' => t('Default project'),
    '#required' => TRUE,
    '#autocomplete_path' => 'api/project/autocomplete',
    '#description' => t('Name of the default project to display if none is given in a URL'),
  );

  $form = system_settings_form($form);
  $form['#submit'][] = 'api_reset_branches';

  return $form;
}

/**
 * Builds a list of PHP branches.
 *
 * @return
 *   Render array for the page.
 */
function api_php_branch_list() {
  variable_set('api_do_cron', 1);

  $output = array();

  $output['add_new'] = array(
    '#type' => 'markup',
    '#markup' => '<p>' . l(t('Add new PHP reference branch'), 'admin/config/development/api/php_branches/new') . '</p>',
  );

  $output['add_new_api'] = array(
    '#type' => 'markup',
    '#markup' => '<p>' . l(t('Add new API reference branch'), 'admin/config/development/api/php_branches/new_api') . '</p>',
  );

  // Build a list of PHP branches.
  $branches = api_get_php_branches();

  $rows = array();
  $header = array(
    t('Name'),
    t('Type'),
    t('Operations'),
  );

  foreach ($branches as $branch) {
    $row = array();

    $row[] = check_plain($branch->title);
    $row[] = check_plain($branch->reference_type);
    $ops = array();

    if ($branch->reference_type == 'php') {
      $ops[] = l(t('edit'), 'admin/config/development/api/php_branches/' . $branch->branch_id);
    } elseif ($branch->reference_type == 'api') {
      $ops[] = l(t('edit'), 'admin/config/development/api/api_branches/' . $branch->branch_id);
    }

    $ops[] = l(t('delete'), 'admin/config/development/api/php_branches/' . $branch->branch_id . '/delete');

    $ops[] = l(t('update'), 'admin/config/development/api/php_branches/' . $branch->branch_id . '/update');
    $row[] = implode(' ', $ops);

    $rows[] = $row;
  }

  $output['branch_list'] = array(
    '#type' => 'markup',
    '#markup' => theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'empty' => t('There are no reference branches set up. You can <a href="@url">add a PHP reference branch</a> or <a href="@apiurl">add an API reference branch</a>.', array('@url' => url('admin/config/development/api/php_branches/new'), '@apiurl' => url('admin/config/development/api/api_branches/new'))),
  )));

  return $output;
}

/**
 * Generates the projects admin page.
 */
function api_admin_project_list() {
  variable_set('api_do_cron', 1);

  $out = array();

  $out['add_new'] = array(
    '#type' => 'markup',
    '#markup' => l(t('Add new project'), 'admin/config/development/api/projects/new'),
  );

  $out['project_list'] = array(
    '#type' => 'markup',
    '#markup' => api_display_view('api_projects', 'block_project_admin', array()),
  );

  return $out;
}

/**
 * Generates the branch admin page.
 */
function api_admin_branch_list() {
  variable_set('api_do_cron', 1);

  $out = array();

  $out['add_new'] = array(
    '#type' => 'markup',
    '#markup' => l(t('Add new branch'), 'admin/config/development/api/branches/new'),
  );

  $out['branch_list'] = array(
    '#type' => 'markup',
    '#markup' => api_display_view('api_branches', 'block_branch_admin', array()),
  );

  return $out;
}

/**
 * Form constructor for resetting the API parse queue.
 *
 * @see api_reset_queue_form_submit()
 * @ingroup forms
 */
function api_reset_queue_form($form, &$form_state) {

  $form = array();

  return confirm_form($form, t('Are you sure you want to reset the parse queue?'), 'admin/config/development/api', t('Files will be added back to the queue as necessary at the next cron run.'));
}

/**
 * Form submission handler for api_reset_queue_form().
 *
 * Resets the parsing queue and redirects to the API admin page.
 */
function api_reset_queue_form_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    api_reset_parse_queue();
  }
  $form_state['redirect'] = 'admin/config/development/api';
}

/**
 * Form constructor for the edit form for a branch.
 *
 * @param $branch_id
 *   ID of the branch to edit.
 *
 * @see api_branch_edit_form_validate()
 * @see api_check_directories()
 * @see api_branch_edit_form_submit()
 * @ingroup forms
 */
function api_branch_edit_form($form, $form_state, $branch_id = NULL) {
  if (is_numeric($branch_id)) {
    $branch = api_get_branch_by_id($branch_id);
  }
  else {
    $branch = new stdClass();
    $branch->branch_id = 0;
    $branch->branch_name = '';
    $branch->title = '';
    $branch->core_compatibility = '';
    $branch->preferred = 1;
    $branch->project = '';
    $branch->directories = '';
    $branch->excluded_directories = '';
    $branch->exclude_files_regexp = '';
    $branch->update_frequency = 1;
  }

  $form = array(
    '#branch' => $branch,
  );

  $form['project'] = array(
    '#title' => t('Project'),
    '#type' => 'textfield',
    '#default_value' => $branch->project,
    '#description' => t('The identifier of the project this branch is part of.'),
    '#autocomplete_path' => 'api/project/autocomplete',
    '#required' => TRUE,
  );

  $form['core_compatibility'] = array(
    '#title' => t('Core compatibility'),
    '#type' => 'textfield',
    '#default_value' => $branch->core_compatibility,
    '#description' => t('Which core version this branch is compatible with (for search/link grouping).'),
    '#required' => TRUE,
  );

  $form['preferred'] = array(
    '#title' => t('Preferred'),
    '#type' => 'radios',
    '#options' => array(
      0 => 'No',
      1 => 'Yes',
    ),
    '#default_value' => $branch->preferred,
    '#description' => t('Whether this is the preferred branch for this project within this core compatibility'),
    '#required' => TRUE,
  );

  $form['branch_name'] = array(
    '#title' => t('Branch identifier'),
    '#type' => 'textfield',
    '#default_value' => $branch->branch_name,
    '#description' => t('Unique identifier for this branch within this project. Used as the URL suffix for documentation pages for this branch.'),
    '#required' => TRUE,
  );

  $form['title'] = array(
    '#title' => t('Branch title'),
    '#type' => 'textfield',
    '#default_value' => $branch->title,
    '#description' => t('Used as the version identifier for item versions. The title of the first branch within the project is also used as the link text in the Other Projects link sections (these links appear at the bottom of API pages such as the functions list and the home page for each project, if you have more than one project).'),
    '#required' => TRUE,
  );
  $form['data'] = array('#tree' => TRUE);

  $form['data']['directories'] = array(
    '#title' => t('Directories'),
    '#type' => 'textarea',
    '#default_value' => $branch->directories,
    '#rows' => 3,
    '#description' => t('Absolute paths to index, one per line.'),
    '#required' => TRUE,
    '#element_validate' => array('api_check_directories'),
  );
  $form['data']['excluded_directories'] = array(
    '#title' => t('Excluded directories'),
    '#type' => 'textarea',
    '#default_value' => $branch->excluded_directories,
    '#rows' => 3,
    '#description' => t('Absolute paths to exclude from the index, one per line.'),
    '#element_validate' => array('api_check_directories'),
  );
  $form['data']['exclude_files_regexp'] = array(
    '#title' => t('Exclude files by regular expressions'),
    '#type' => 'textarea',
    '#default_value' => $branch->exclude_files_regexp,
    '#rows' => 3,
    '#description' => t('Regular expressions: all matching files and directories will be excluded, one per line. Include delimeters.'),
    '#element_validate' => array('api_check_regexps'),
  );

  $form['update_frequency'] = array(
    '#type' => 'select',
    '#title' => t('Branch update frequency'),
    '#description' => t('During cron runs, this branch will be checked for updated files. This sets a minimum time to wait before checking.'),
    '#default_value' => isset($branch->update_frequency) ? $branch->update_frequency : 1,
    '#options' => drupal_map_assoc(array(1, 3600, 10800, 21600, 43200, 86400, 604800), 'format_interval'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save branch'),
  );

  if (is_numeric($branch_id)) {
    $form[] = array(
      '#type' => 'markup',
      '#markup' => l(t('Delete'), 'admin/config/development/api/branches/' . $branch->branch_id . '/delete'),
    );
  }

  return $form;
}

/**
 * Form validation handler for api_branch_edit_form().
 *
 * @see api_branch_edit_form_submit()
 */
function api_branch_edit_form_validate($form, &$form_state) {
  $project = $form_state['values']['project'];
  $branch_name = $form_state['values']['branch_name'];

  // Check for bad characters in branch names.
  if (preg_match('/[^A-Za-z0-9-_.]/', $branch_name)) {
    form_set_error('branch_name', t("Only letters, numbers, '.', '-' and '_' are allowed in the branch name, as it is used as an URL suffix."));
  }

  // Check for duplicate branch names.
  if (db_query("SELECT 1 FROM {api_branch} WHERE branch_id <> :branch_id AND project = :project AND branch_name = :branch_name", array(':branch_id' => $form['#branch']->branch_id, ':project' => $project, ':branch_name' => $branch_name))->fetchField()) {
    form_set_error('branch_name', t('%project and %branch_name is already used by another branch.', array('%project' => $project, '%branch_name' => $branch_name)));
  }

  // Check that the project exists.
  if (!api_project_exists($project)) {
    form_set_error('branch_name', t('%project does not exist.', array('%project' => $project)));
  }
}

/**
 * Verifies that all directories exist and are actually directories.
 *
 * Element validation callback for the directories elements of
 * api_branch_edit_form().
 */
function api_check_directories($element, &$form_state) {
  // Check for valid directories.
  foreach (explode("\n", $element['#value']) as $directory) {
    $directory = trim($directory);
    if (!empty($directory)) {
      if (!is_dir($directory)) {
        form_set_error(implode('][', $element['#parents']), t('%directory is not a directory.', array('%directory' => $directory)));
      }
      elseif (!is_readable($directory)) {
        form_set_error(implode('][', $element['#parents']), t('%directory is not readable by PHP.', array('%directory' => $directory)));
      }
    }
  }
  form_set_value($element, api_clean_directory_list($element['#value']), $form_state);
}

/**
 * Verifies that all regular expressions are legal.
 *
 * Element validation callback for the exclude regexp element of
 * api_branch_edit_form().
 */
function api_check_regexps($element, &$form_state) {
  $msg = array();
  $regexps = explode("\n", $element['#value']);
  $regexps = array_filter(array_map('trim', $regexps));

  foreach ($regexps as $regexp) {
    // Validate by calling preg_match() and checking for error.
    if (@preg_match(trim($regexp), 'a') === FALSE) {
      $msg[] = t('%regexp is not a valid regular expression', array('%regexp' => $regexp));
    }
  }
  if (count($msg)) {
    form_set_error(implode('][', $element['#parents']), implode('; ', $msg));
  }
}

/**
 * Trims whitespace, and removes trailing directory separators and blank lines.
 *
 * @param $list
 *   Array of directories.
 *
 * @return
 *   String with cleaned-up directories, separated by newlines.
 */
function api_clean_directory_list($list) {
  $array = explode("\n", $list);
  foreach ($array as $key => $directory) {
    $array[$key] = rtrim(trim($directory), DIRECTORY_SEPARATOR);
    if (empty($array[$key])) {
      unset($array[$key]);
    }
  }
  return implode("\n", $array);
}

/**
 * Form submission handler for api_branch_edit_form().
 *
 * @see api_branch_edit_form_validate()
 */
function api_branch_edit_form_submit($form, &$form_state) {
  $branch = $form['#branch'];
  $branch->project = $form_state['values']['project'];
  $branch->branch_name = $form_state['values']['branch_name'];
  $branch->title = $form_state['values']['title'];
  $branch->data = $form_state['values']['data'];
  $branch->core_compatibility = $form_state['values']['core_compatibility'];
  $branch->preferred = $form_state['values']['preferred'];
  $branch->update_frequency = $form_state['values']['update_frequency'];

  api_save_branch($branch);

  drupal_set_message(t('Saved %branch_name.', array('%branch_name' => $branch->branch_name)));
  $form_state['redirect'] = 'admin/config/development/api/branches';
}

/**
 * Form constructor for the edit form for a PHP reference branch.
 *
 * @param $branch_id
 *   ID of the branch to edit.
 *
 * @see api_php_branch_edit_form_submit()
 * @ingroup forms
 */
function api_php_branch_edit_form($form, $form_state, $branch_id = NULL) {
  if (is_numeric($branch_id)) {
    $branches = api_get_php_branches();
    $branch = isset($branches[$branch_id]) ? $branches[$branch_id] : NULL;
  }

  if (!isset($branch)) {
    $branch = new stdClass();
    $branch->branch_id = 0;
    $branch->title = '';
    $branch->reference_type = 'php';
    $branch->summary = '';
    $branch->path = '';
    $branch->update_frequency = 1;
  }

  $form = array(
    '#branch' => $branch,
  );

  $form['title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#default_value' => $branch->title,
    '#description' => t('Used in admin screens only.'),
    '#required' => TRUE,
  );

  // This form is only for PHP-type branches.
  $form['reference_type'] = array(
    '#type' => 'hidden',
    '#value' => 'php',
  );

  $form['data'] = array('#tree' => TRUE);

  $form['data']['summary'] = array(
    '#type' => 'textfield',
    '#title' => t('Function list'),
    '#default_value' => $branch->summary,
    '#required' => TRUE,
    '#description' => t('The URL of the PHP function summary list.'),
  );

  $form['data']['path'] = array(
    '#type' => 'textfield',
    '#title' => t('Function URL'),
    '#default_value' => $branch->path,
    '#required' => TRUE,
    '#description' => t('The URL format used to build the link to PHP functions.  Use the variable <em>!function</em> in place of the function name.'),
  );

  $form['update_frequency'] = array(
    '#type' => 'select',
    '#title' => t('Branch update frequency'),
    '#description' => t('During cron runs, this branch will be checked for updates in the function list. This sets a minimum time to wait before checking.'),
    '#default_value' => isset($branch->update_frequency) ? $branch->update_frequency : 1,
    '#options' => drupal_map_assoc(array(1, 3600, 10800, 21600, 43200, 86400, 604800), 'format_interval'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save branch'),
  );

  if (is_numeric($branch_id)) {
    $form[] = array(
      '#type' => 'markup',
      '#markup' => l(t('Delete'), 'admin/config/development/api/php_branches/' . $branch->branch_id . '/delete'),
    );
  }

  return $form;
}

/**
 * Form submission handler for api_php_branch_edit_form().
 *
 * Also used by api_api_branch_edit_form().
 */
function api_php_branch_edit_form_submit($form, &$form_state) {
  $branch = $form['#branch'];
  $branch->title = $form_state['values']['title'];
  $branch->data = $form_state['values']['data'];
  $branch->reference_type = $form_state['values']['reference_type'];
  $branch->update_frequency = $form_state['values']['update_frequency'];

  api_save_php_branch($branch);

  drupal_set_message(t('Saved %branch_name.', array('%branch_name' => $branch->title)));
  $form_state['redirect'] = 'admin/config/development/api/php_branches';
}

/**
 * Form constructor for the edit form for an API reference branch.
 *
 * @param $branch_id
 *   ID of the branch to edit.
 *
 * @see api_php_branch_edit_form_submit()
 * @ingroup forms
 */
function api_api_branch_edit_form($form, $form_state, $branch_id = NULL) {
  if (is_numeric($branch_id)) {
    $branches = api_get_php_branches();
    $branch = isset($branches[$branch_id]) ? $branches[$branch_id] : NULL;
  }

  if (!isset($branch)) {
    $branch = new stdClass();
    $branch->branch_id = 0;
    $branch->title = '';
    $branch->reference_type = 'api';
    $branch->url = '';
    $branch->core_compatibility = '7.x';
    $branch->project_type = 'core';
    $branch->update_frequency = 1;
    $branch->search_url = '';
  }

  $form = array(
    '#branch' => $branch,
  );

  $form['title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#default_value' => $branch->title,
    '#description' => t('Used in admin screens only.'),
    '#required' => TRUE,
  );

  // This form is for the API type of reference branch.
  $form['reference_type'] = array(
    '#type' => 'hidden',
    '#value' => 'api',
  );

  $form['data'] = array('#tree' => TRUE);

  $form['data']['url'] = array(
    '#type' => 'textfield',
    '#title' => t('Branch full_list URL'),
    '#default_value' => $branch->url,
    '#required' => TRUE,
    '#description' => t('The URL of the API documentation dump, such as http://api.drupal.org/api/drupal/full_list/7'),
  );

  $form['data']['search_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Branch search URL'),
    '#default_value' => $branch->url,
    '#required' => TRUE,
    '#description' => t('The URL to use for searching by appending the search term, such as http://api.drupal.org/api/drupal/7/search/'),
  );

  $form['data']['core_compatibility'] = array(
    '#type' => 'textfield',
    '#default_value' => $branch->core_compatibility,
    '#title' => t('Core compatibility'),
    '#required' => TRUE,
  );

  $form['data']['project_type'] = array(
    '#title' => t('Type'),
    '#type' => 'textfield',
    '#default_value' => $branch->project_type,
    '#description' => t('Type of project (core, module, theme, etc.).'),
    '#required' => TRUE,
  );

  $form['update_frequency'] = array(
    '#type' => 'select',
    '#title' => t('Branch update frequency'),
    '#description' => t('During cron runs, this branch will be checked for updates in the information. This sets a minimum time to wait before checking.'),
    '#default_value' => isset($branch->update_frequency) ? $branch->update_frequency : 1,
    '#options' => drupal_map_assoc(array(1, 3600, 10800, 21600, 43200, 86400, 604800), 'format_interval'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save branch'),
  );

  if (is_numeric($branch_id)) {
    $form[] = array(
      '#type' => 'markup',
      '#markup' => l(t('Delete'), 'admin/config/development/api/php_branches/' . $branch->branch_id . '/delete'),
    );
  }

  // Use the submit handler for the PHP branch edit form, which is generic
  // enough to work for this type too.
  $form['#submit'] = array('api_php_branch_edit_form_submit');

  return $form;
}

/**
 * Form constructor for the edit form for a project.
 *
 * @param $project_name
 *   Name of the project to edit.
 *
 * @see api_project_edit_form_submit()
 * @ingroup forms
 */
function api_project_edit_form($form, $form_state, $project_name = NULL) {
  if (is_string($project_name)) {
    $project = db_select('api_project', 'p')
      ->fields('p')
      ->condition('project_name', $project_name)
      ->execute()
      ->fetchObject();
  }
  else {
    $project = new stdClass();
    $project->project_name = '';
    $project->project_title = '';
    $project->project_type = '';
  }

  $form = array(
    '#project' => $project,
  );

  $form['project_name'] = array(
    '#title' => t('Identifier'),
    '#type' => 'textfield',
    '#default_value' => $project->project_name,
    '#description' => t('Used in URLs to identify the project, must be unique.'),
    '#required' => TRUE,
  );

  $form['project_title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#default_value' => $project->project_title,
    '#description' => t('Used to identify the project in lists.'),
    '#required' => TRUE,
  );

  $form['project_type'] = array(
    '#title' => t('Type'),
    '#type' => 'textfield',
    '#default_value' => $project->project_type,
    '#description' => t('Type of project (core, module, theme, etc.).'),
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save project'),
  );

  if (strlen($project_name) > 0) {
    $form[] = array(
      '#type' => 'markup',
      '#markup' => l(t('Delete'), 'admin/config/development/api/projects/' . $project->project_name . '/delete'),
    );
  }

  return $form;
}

/**
 * Form submission handler for api_project_edit_form().
 */
function api_project_edit_form_submit($form, &$form_state) {
  $project = $form['#project'];
  $old_name = $project->project_name;

  $project->project_name = $form_state['values']['project_name'];
  $project->project_title = $form_state['values']['project_title'];
  $project->project_type = $form_state['values']['project_type'];

  api_save_project($project, $old_name);

  drupal_set_message(t('Saved %project_name.', array('%project_name' => $project->project_name)));
  $form_state['redirect'] = 'admin/config/development/api/projects';
}

/**
 * Form constructor for the confirmation form for deleting a branch.
 *
 * @param $branch_id
 *   ID of the branch to delete.
 *
 * @see api_branch_delete_form_submit()
 */
function api_branch_delete_form($form, $form_state, $branch_id) {
  $branch = api_get_branch_by_id($branch_id);
  if (!isset($branch->branch_name)) {
    drupal_set_message(t('Invalid branch supplied'));
    drupal_goto('admin/config/development/api/branches');
  }

  return confirm_form(array('#branch' => $branch), t('Are you sure you want to delete branch %branch_name and all its documentation? This operation cannot be undone.', array('%branch_name' => $branch->branch_name)), 'admin/config/development/api/branches', NULL, t('Delete'));
}

/**
 * Form submission handler for api_branch_delete_form().
 */
function api_branch_delete_form_submit($form, &$form_state) {
  api_delete_branch($form['#branch']->branch_id);
  $form_state['redirect'] = 'admin/config/development/api/branches';
}

/**
 * Form constructor for the confirmation form for deleting a PHP branch.
 *
 * @param $branch_id
 *   ID of the branch to delete.
 *
 * @see api_php_branch_delete_form_submit()
 */
function api_php_branch_delete_form($form, $form_state, $branch_id) {
  $branches = api_get_php_branches();
  $branch = isset($branches[$branch_id]) ? $branches[$branch_id] : NULL;

  if (!isset($branch->title)) {
    drupal_set_message(t('Invalid branch supplied'));
    drupal_goto('admin/config/development/api/php_branches');
  }

  return confirm_form(array('#branch' => $branch), t('Are you sure you want to delete branch %branch_name? This operation cannot be undone.', array('%branch_name' => $branch->title)), 'admin/config/development/api/php_branches', NULL, t('Delete'));
}

/**
 * Form submission handler for api_php_branch_delete_form().
 */
function api_php_branch_delete_form_submit($form, &$form_state) {
  api_delete_php_branch($form['#branch']->branch_id);
  $form_state['redirect'] = 'admin/config/development/api/php_branches';
}

/**
 * Form constructor for the confirmation form for deleting a project.
 *
 * @param $project_name
 *   Name of the project to delete.
 *
 * @see api_project_delete_form_submit()
 */
function api_project_delete_form($form, $form_state, $project_name) {
  return confirm_form(array('#project' => $project_name), t('Are you sure you want to delete project %project and all its branches and documentation? This operation cannot be undone.', array('%project' => $project_name)), 'admin/config/development/api/projects', NULL, t('Delete'));
}

/**
 * Form submission handler for api_project_delete_form().
 */
function api_project_delete_form_submit($form, &$form_state) {
  api_delete_project($form['#project']);
  $form_state['redirect'] = 'admin/config/development/api/projects';
}

/**
 * Form constructor for the settings form for API comments.
 *
 * @see api_comments_settings_form_submit()
 */
function api_comments_settings_form($form, &$form_state) {
  $type = node_type_set_defaults(array('type' => 'api', 'module' => 'api'));
  $form['#node_type'] = $type;

  // Let the Comment module add comment settings to the form.
  if (module_exists('comment')) {
    $form['type'] = 1;
    comment_form_node_type_form_alter($form, $form_state);
    unset($form['type']);
  }

  // Comment module puts the settings in a fieldset, but we don't need that.
  unset($form['comment']['#type']);

  // Add a submit button.
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save comment settings'),
  );

  // Add a link to apply the current settings to new nodes.
  if (module_exists('comment')) {
    $form['apply_link'] = array(
      '#type' => 'markup',
      '#markup' => '<p>' . l(t('Apply current default comment settings to existing API pages'), 'admin/config/development/api/comments/apply') . '</p>',
    );
  }

  return $form;
}

/**
 * Form submission handler for api_comments_settings_form().
 */
function api_comments_settings_form_submit($form, &$form_state) {
  foreach ($form_state['values'] as $key => $value) {
    if (strpos($key, 'comment') === 0) {
      variable_set($key . '_api', $value);
    }
  }
  drupal_set_message(t('Saved API comment settings.'));
}

/**
 * Applies current default comment settings to existing API nodes.
 */
function api_apply_comment_settings() {
  if (module_exists('comment')) {
    $setting = variable_get('comment_api', 2);
    db_update('node')
      ->condition('type', 'api')
      ->fields(array('comment' => $setting))
      ->execute();
    // This is a query using a WHERE EXISTS, and there is no apparent way
    // to write it using the database API, so use ANSI syntax.
    db_query("UPDATE {node_revision} SET comment = :comment_setting WHERE EXISTS ( SELECT * FROM {node} n WHERE n.type = 'api')", array(':comment_setting' => $setting));
    // Clear the cache so node_load doesn't load old versions.
    entity_get_controller('node')->resetCache();

    // Might as well also make sure that the body field exists for comments.
    if ($setting != 0) {
      $info = (object) array('type' => 'api');
      _comment_body_field_create($info);
    }

    drupal_set_message(t('Applied default comment settings to existing API pages'));
  }
  else {
    drupal_set_message(t('The Comment module is not enabled. No settings changed'));
  }

  drupal_goto('admin/config/development/api/comments');
}

/**
 * Page callback: Marks a branch for reparsing and redirects to branches page.
 *
 * @param $branch_id
 *   ID of the branch to mark for reparsing.
 *
 * @see api_menu()
 */
function api_admin_reparse($branch_id) {
  $branch = api_get_branch_by_id($branch_id);
  if (is_null($branch)) {
    drupal_set_message(t('Unknown branch requested for reparse'), array(), 'error');
    drupal_goto('admin/config/development/api/branches');
    return;
  }

  // Mark the files for reparsing.
  $num = api_mark_for_reparse($branch_id);
  $type = ($num > 0) ? 'status' : 'error';
  // Mark branch for update.
  $branch->last_updated = 0;
  drupal_write_record('api_branch', $branch, array('branch_id'));

  drupal_set_message(t('@num files in branch @branch set to reparse, starting at next cron run', array('@num' => $num, '@branch' => $branch->branch_name)), $type);
  drupal_goto('admin/config/development/api/branches');
}

/**
 * Page callback: Marks a branch for update and redirects to branches page.
 *
 * @param $type
 *   'branch' for a regular branch, and 'php_branch' for a PHP reference
 *   branch.
 * @param $branch_id
 *   ID of the branch to mark for update.
 *
 * @see api_menu()
 */
function api_admin_update($type, $branch_id) {
  if ($type == 'branch') {
    $branch = api_get_branch_by_id($branch_id);
  }
  else {
    $branches = api_get_php_branches();
    $branch = isset($branches[$branch_id]) ? $branches[$branch_id] : NULL;
  }

  if (is_null($branch)) {
    drupal_set_message(t('Unknown branch requested for update'), array(), 'error');
  }
  else {
    $branch->last_updated = 0;
    if ($type == 'branch') {
      drupal_write_record('api_branch', $branch, 'branch_id');
    }
    else {
      drupal_write_record('api_php_branch', $branch, 'branch_id');
    }

    drupal_set_message(t('Branch @branch set to update at next cron run', array('@branch' => $branch->title)));
  }

  if ($type == 'branch') {
    drupal_goto('admin/config/development/api/branches');
  }
  else {
    drupal_goto('admin/config/development/api/php_branches');
  }
}
