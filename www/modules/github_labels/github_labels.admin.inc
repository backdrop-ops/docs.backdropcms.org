<?php
/**
 * @file
 * Contains admin callbacks for the Role help module.
 */


/**
 * Return the settings form for GitHub Labels.
 */
function github_labels_settings_form($form, &$form_state) {
  $form['#config'] = 'github_labels.settings';

  $form[] = array(
    '#markup' => t('Enter one or more GitHub repositories to create tokens and CSS for GitHub labels. The labels will be formatted to match their appearance in GitHub. If you specify multiple repositories that define the same label name, the formatting used will be the repository that appears first in the list below.'),
    '#prefix' => '<p>',
    '#suffix' => '</p>',
  );

  $form['repositories'] = array(
    '#type' => 'textarea',
    '#title' => t('Repositories'),
    '#description' => t('Enter the GitHub repositories in the form user/repository, one per line.'),
    '#default_value' => config_get('github_labels.settings', 'repositories'),
  );
  $form['#validate'][] = 'github_labels_settings_form_validate';
  $form['#submit'][] = 'github_labels_settings_form_submit';

  return system_settings_form($form);
}

/**
 * Validation function for settings form.
 */
function github_labels_settings_form_validate($form, &$form_state) {
  $repositories = explode("\n", trim($form_state['values']['repositories']));
  foreach ($repositories as $repository) {
    if (!empty(trim($repository))) {
      $parts = explode('/', trim($repository));
      if (count($parts) != 2) {
        form_set_error('repositories', t('%repository is not in the form user/repository.', array('%repository' => $repository)));
      }
    }
  }
}

/**
 * Submission function for settings forms.
 */
function github_labels_settings_form_submit($form, &$form_state) {
  cache_clear_all('github_labels_info', 'cache');
  cache_clear_all('github_labels_css', 'cache');
}

/**
 * Returns the report page of all GitHub labels.
 */
function github_labels_report_page() {
  $build = array();
  if (!module_exists('token')) {
    $link = l(t('install and enable the Token Filter module'), 'https://backdropcms.org/project/token_filter');
    $build[] = array(
      '#markup' => t('You will need to !link to use the tokens within text fields.', array('!link' => $link)),
      '#prefix' => '<p>',
      '#suffix' => '</p>',
    );
  }
  $labels_info = github_labels_info();
  foreach ($labels_info['repositories'] as $repository => $repository_data) {
    $build[$repository] = array(
      '#type' => 'fieldset',
      '#title' => l(check_plain($repository), 'https://github.com/' . $repository . '/labels'),
    );
    $header = array(
      array('data' => t('Label')),
      array('data' => t('Tokens')),
      array('data' => t('Label HTML')),
    );
    $rows = array();
    foreach ($repository_data as $data) {
      $rows[] = array(
        array('data' => $data['html']),
        array('data' => '[gh-label:' . $data['clean_name'] . ']' . '<br>' . '[gh-label-link:' . $data['clean_name'] . ']'),
        array('data' => '<code>' . check_plain($data['html']) . '</code>'),
      );
    }
    $build[$repository]['table'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
    );
  }
  return $build;
}
