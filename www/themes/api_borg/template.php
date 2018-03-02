<?php
/**
 * @file
 * Theme function overrides.
 */

/*******************************************************************************
 * Alter functions: modify renderable structures before used.
 ******************************************************************************/

/**
 * Implements hook_form_FORM_ID_alter().
 */
function api_borg_form_search_block_form_alter(&$form, $form_state) {
  // Add an autocomplete. See api-theme.js.
  $form['search_block_form']['#attributes']['data-typeahead'] = url('api/search/autocomplete/1');
  $form['search_block_form']['#attributes']['data-typeahead-selected'] = url('api/backdrop/1/search');

  $form['search_block_form']['#attributes']['placeholder'] = t('Search API and Documentation');
  $form['search_block_form']['#type'] = 'search';
}

/*******************************************************************************
 * Preprocess functions: prepare variables for templates.
 ******************************************************************************/

/**
 * Prepares variables for page templates.
 * @see page.tpl.php
 */
function api_borg_preprocess_page(&$variables) {
  backdrop_add_js('core/misc/tableheader.js');
  if (arg(0) == 'node' && arg(1) == '41642' && !arg(2)) {
    $path = backdrop_get_path('theme', 'api_borg');
    backdrop_add_css($path . '/css/page-form-api.css');
  }
}

/**
 * Prepares variables for layout templates.
 * @see layout.tpl.php
 */
function api_borg_preprocess_layout(&$variables) {
  if ($variables['content']['header']) {
    $variables['content']['header'] = '<div class="l-header-inner">' . $variables['content']['header'] . '</div>';
  }
}

/**
 * Prepares variables for search result templates.
 * @see search-results.tpl.php
 */
function api_borg_preprocess_search_results(&$variables) {
  foreach ($variables['search_results'] as $key => $result) {
    if (isset($result['result']['type'])) {
      $variables['search_results'][$key]['info'] = $result['result']['type'];
    }
  }
}
