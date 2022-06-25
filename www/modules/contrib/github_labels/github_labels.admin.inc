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

  $form['help'] = array(
    '#type' => 'help',
    '#markup' => t('Enter one or more GitHub repositories to create tokens and CSS for GitHub labels. The labels will be formatted to match their appearance in GitHub. If you specify multiple repositories that define the same label name, the formatting used will be the repository that appears first in the list below.'),
  );

  $form['repositories'] = array(
    '#type' => 'textarea',
    '#title' => t('Repositories'),
    '#description' => t('Enter the GitHub repositories in the form user/repository, one per line.'),
    '#default_value' => config_get('github_labels.settings', 'repositories'),
  );

  $form = system_settings_form($form);
  $form['#validate'][] = 'github_labels_settings_form_validate';
  $form['#submit'][] = 'github_labels_settings_form_submit';
  return $form;
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
 * Post-processing submission function for settings forms.
 */
function github_labels_settings_form_submit($form, &$form_state) {
  github_labels_update_state(TRUE);
}

/**
 * Returns the report page of all GitHub labels.
 */
function github_labels_report_page() {
  $build = array();
  if (!module_exists('token')) {
    $build['help'] = array(
      '#type' => 'help',
      '#markup' => t('You will need to install and enable the <a href="@url">Token Filter module</a> to use the tokens within text fields.', array('@url' => 'https://backdropcms.org/project/token_filter')),
    );
  }

  $labels_info = state_get('github_labels_labels_info');
  if (!empty($labels_info)) {
    foreach ($labels_info as $repository => $repository_data) {
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
  }

  return $build;
}
