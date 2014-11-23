<?php

/**
 * @file
 * Theme-related functions for the API module.
 */

/**
 * Preprocesses theme('api_defined').
 *
 * Sets up variables for the file link and summary line.
 */
function api_preprocess_api_defined(&$variables) {
  $object = $variables['object'];
  $branch = $variables['branch'];
  $variables['file_link'] = api_file_link($object);
  $file_info = api_object_load((int) $object->file_did, $branch, 'file');

  $variables['file_summary'] = '';
  if (isset($file_info)) {
    $variables['file_summary'] = $file_info->summary;
  }
}

/**
 * Preprocesses theme('api_class_section').
 *
 * Sets up variables for the class link and summary line.
 */
function api_preprocess_api_class_section(&$variables) {
  $class = $variables['class'];
  $branch = $variables['branch'];
  $variables['class_link'] = l($class->title, api_url($class));
  $variables['class_info'] = $class->summary;
}

/**
 * Preprocesses a documentation item page.
 */
function api_preprocess_api_object_page(&$variables) {
  // Note: branch is not defined for File pages, so don't try to create the
  // 'defined' section of the page.
  if (isset($variables['branch'])) {
    $variables['defined'] = theme('api_defined', array('branch' => $variables['branch'], 'object' => $variables['object']));
  }

  $object = $variables['object'];

  // Find the namespace.
  if (isset($object->namespace) && strlen($object->namespace)) {
    $variables['namespace'] = l($object->namespace, api_namespace_url($object->branch_id, $object->namespace));
  }
  else {
    $variables['namespace'] = '';
  }

  // Find alternative versions in different files and branches.
  $branches = api_get_branches();
  $branch = api_get_branch_by_id($object->branch_id);
  $alternatives = array(
    '#prefix' => '<ol id="api-alternatives">',
    '#suffix' => '</ol>',
  );
  // Try to find objects of the same type with the same object name. But for
  // files, the object name includes the path, so the matching needs to go on
  // the title instead of the object_name. Limit these to the same project.
  if ($object->object_type == 'file') {
    $query = db_select('api_documentation', 'ad')
      ->fields('ad', array('branch_id', 'file_name', 'object_type', 'object_name'))
      ->condition('ad.object_type', $variables['object']->object_type)
      ->condition('ad.title', $variables['object']->title);
    $query->leftJoin('api_branch', 'b', 'ad.branch_id = b.branch_id');
    $query->condition('b.project', $branch->project);
    $result = $query->execute();
  }
  else {
    $query = db_select('api_documentation', 'ad')
      ->fields('ad', array('branch_id', 'file_name', 'object_type', 'object_name'))
      ->condition('ad.object_type', $variables['object']->object_type)
      ->condition('ad.object_name', $variables['object']->object_name);
    $query->leftJoin('api_branch', 'b', 'ad.branch_id = b.branch_id');
    $query->condition('b.project', $branch->project);
    $result = $query->execute();
  }

  $count = 0;
  foreach ($result as $alternative) {
    // Construct link label.
    $label = $branches[$alternative->branch_id]->branch_name . ' ' . $alternative->file_name;
    $suffix = ($object->object_type == 'file') ? '' : ' ' . $variables['object']->object_name;
    $alternatives[$branches[$alternative->branch_id]->project][] = array(
      '#prefix' => '<li>',
      '#markup' => l($label, api_url($alternative)) . $suffix,
      '#weight' => $branches[$alternative->branch_id]->weight,
      '#suffix' => '</li>',
    );
    $count++;
  }
  if ($count > 1) {
    $variables['alternatives'] = drupal_render($alternatives);
  }
  else {
    $variables['alternatives'] = '';
  }

  // See if this is an override of another method/etc.
  $variables['override'] = '';
  if (!empty($object->overrides_did)) {
    $overrides = api_object_load((int) $object->overrides_did, $branches[$object->branch_id], array('function', 'property', 'constant'));
    if (isset($overrides)) {
      $variables['override'] = '<p class="api-override">' . t('Overrides !link', array('!link' => l($overrides->title, api_url($overrides)))) . '</p>';
    }
  }
}

/**
 * Returns HTML for a reference link on a function or file page.
 *
 * This theme hook is used for the "N functions call function()" and "N
 * functions implement hook()" links on function pages.
 *
 * @param $variables
 *   Associative array of variables to use, with the following elements:
 *   - type: The type of reference link, one of:
 *     - 'calls': Function calls.
 *     - 'implementations': Hook implementations.
 *     - 'theme_invokes': Theme calls to a function or file.
 *     - 'invokes': Calls to module_invokes and similar functions.
 *     - 'references': String references to a function name.
 *     - 'hierarchy': Class hierarchy.
 *   - count: The number of referenced items.
 *   - function: The function, class, or file object being referenced.
 *   - override_text: Text to override the link text.
 *
 * @ingroup themeable
 */
function theme_api_function_reference_link($variables) {
  module_load_include('inc', 'api', 'api.pages');
  $type = $variables['type'];
  $count = $variables['count'];
  $function = $variables['function'];
  if (!isset($function->branch_id)) {
    return '';
  }

  $link_title = (isset($variables['override_text']) && !empty($variables['override_text'])) ? $variables['override_text'] : api_reference_text($type, $count, $function);

  // Create the link path.
  $processed_file_name = str_replace(API_FILEPATH_SEPARATOR, API_FILEPATH_SEPARATOR_REPLACEMENT, $function->file_name);
  $branch = api_get_branch_by_id($function->branch_id);

  if ($function->object_type == 'file') {
    $link_path = 'api/' . $branch->project . '/' . $processed_file_name . '/' . $type . '/' . $branch->branch_name;
  }
  elseif ($function->object_type == 'class') {
    $link_path = 'api/' . $branch->project . '/' . $processed_file_name . '/class/' . $type . '/' . $function->object_name . '/' . $branch->branch_name;
  }
  elseif ($function->object_type == 'interface') {
    $link_path = 'api/' . $branch->project . '/' . $processed_file_name . '/interface/' . $type . '/' . $function->object_name . '/' . $branch->branch_name;
  }
  else {
    $link_path = 'api/' . $branch->project . '/' . $processed_file_name . '/function/' . $type . '/' . $function->object_name . '/' . $branch->branch_name;
  }

  return l($link_title, $link_path);
}

/**
 * Preprocesses variables for api-views-view-dl-list.tpl.php.
 */
function api_preprocess_views_view_dl_list(&$variables) {
  module_load_include('views', 'theme/theme', 'inc');

  $handler  = $variables['view']->style_plugin;

  $class = explode(' ', $handler->options['class']);
  $class = array_map('views_clean_css_identifier', $class);

  $wrapper_class = explode(' ', $handler->options['wrapper_class']);
  $wrapper_class = array_map('views_clean_css_identifier', $wrapper_class);

  $variables['list_class'] = implode(' ', $class);
  $variables['wrapper_class'] = implode(' ', $wrapper_class);
  $variables['wrapper_prefix'] = '';
  $variables['wrapper_suffix'] = '';
  if ($variables['wrapper_class']) {
    $variables['wrapper_prefix'] = '<div class="' . $variables['wrapper_class'] . '">';
    $variables['wrapper_suffix'] = '</div>';
  }

  template_preprocess_views_view_unformatted($variables);
}
