<?php

/**
 * Implements hook_preprocess_page().
 */
function api_theme_preprocess_page(&$variables) {
  backdrop_add_js('core/misc/tableheader.js');
}

/**
 * Implements hook_preprocess_layout().
 */
function api_theme_preprocess_layout(&$variables) {
  if ($variables['content']['header']) {
    $variables['content']['header'] = '<div class="l-header-inner">' . $variables['content']['header'] . '</div>';
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function api_theme_form_search_block_form_alter(&$form, $form_state) {
  // Add an autocomplete. See api-theme.js.
  $form['search_block_form']['#attributes']['data-typeahead'] = url('api/search/autocomplete/1');
  $form['search_block_form']['#attributes']['data-typeahead-selected'] = url('api/backdrop/1/search');

  $form['search_block_form']['#attributes']['placeholder'] = t('Search API and Documentation');
  $form['search_block_form']['#type'] = 'search';
}

/**
 * Implements hook_preprocess_search_results().
 */
function api_theme_preprocess_search_results(&$variables) {
  foreach ($variables['search_results'] as $key => $result) {
    if (isset($result['result']['type'])) {
      $variables['search_results'][$key]['info'] = $result['result']['type'];
    }
  }
}
