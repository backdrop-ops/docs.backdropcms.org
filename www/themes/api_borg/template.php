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
  if (arg(0) == 'form_api' || (arg(0) == 'node' && arg(1) == '41642' && !arg(2))) {
    $path = backdrop_get_path('theme', 'api_borg');
    backdrop_add_css($path . '/css/page-form-api.css', array('group' => CSS_THEME));
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

/**
 * Prepares variables for unformatted views templates.
 * @see views-view-unformatted.tpl.php
 */
function api_borg_preprocess_views_view_unformatted(&$variables) {
  if ($variables['view']->name == 'form_api') {
    // Re-word the title (content type).
    if (!empty($variables['title'])) {
      if ($variables['title'] == 'fapi_element') {
        $variables['title'] = t('Elements');
      }
      elseif ($variables['title'] == 'fapi_property') {
        $variables['title'] = t('Properties');
      }
    }
  }
}

/**
 * Prepares variables for paragraph items.
 * @see paragraphs-item.tpl.php
 */
function api_borg_preprocess_paragraphs_item(&$variables, $hook) {
  $content = $variables['content'];

  if ($variables['bundle'] == 'fapi_properties') {
    $property = $content['field_fapi_property'][0]['#markup'];
    $id_suffix = in_array($property, backdropapi_form_api_elements()) ? '_property' : '';

    $recommended = !empty($content['field_recommended'][0]['#markup']);
    if (isset($content['field_default_value'])) {
      $default = $content['field_default_value'][0]['#markup'];
    }

    $text = '#' . $property;
    if ($recommended) {
      $text = '<strong>' . $text . '</strong>';
    }

    $variables['fapi_property'] = l($text, '', array('fragment' => $property . $id_suffix, 'external' => TRUE, 'html' => TRUE));
    if (!empty($default)) {
      $variables['fapi_property'] .= t(' (default: @value)', array('@value' => $default));
    }
  }
}

/*******************************************************************************
 * Theme functions: override the output of theme functions.
 ******************************************************************************/

/**
 * Overrides theme_views_view_field().
 */
function api_borg_views_view_field($variables) {
  $view = $variables['view'];
  $field = $variables['field'];
  $row = $variables['row'];

  if ($view->name == 'form_api' && $view->current_display == 'block_1') {
    // Add a wrapper H3 tag with an ID, and add a '#' to property names.
    if ($field->field == 'title') {
      $text = $variables['output'];
      $id_suffix = '';
      if ($row->node_type == 'fapi_property') {
        if (in_array($text, backdropapi_form_api_elements())) {
          $id_suffix = '_property';
        }
        $text = '#' . $text;
      }

      $variables['output'] = '<h3 id="' . $variables['output'] . $id_suffix . '">' . $text . '</h3>';
    }
    // Display a list of all FAPI Elements for the 'type' property.
    elseif (($row->node_type == 'fapi_property') && ($row->_field_data['nid']['entity']->title == 'type') && ($field->field == 'field_values')) {
      $output = array();
      $results = views_get_view_result('form_api', 'attachment_5');

      foreach ($results as $result) {
        $output[] = l($result->node_title, '', array('fragment' => $result->node_title, 'external' => TRUE));
      }

      $variables['output'] = implode(', ', $output);
    }
  }

  return $variables['output'];
}
