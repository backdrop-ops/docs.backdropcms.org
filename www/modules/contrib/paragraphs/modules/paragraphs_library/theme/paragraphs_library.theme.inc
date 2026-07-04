<?php
/**
 * @file
 * Paragraphs library module theme functions.
 */

/**
 * Process variables for paragraphs-library-item.tpl.php.
 */
function template_preprocess_paragraphs_library_item(&$variables) {
  $variables['view_mode'] = $variables['element']['#view_mode'];
  $library_item_type = $variables['element']['#entity_plus_type'];
  $variables['entity_type'] = $library_item_type;
  $library_item = $variables['element']['#entity'];
  $variables[$library_item_type] = $library_item;

  list(, , $bundle) = entity_extract_ids($library_item_type, $library_item);
  $variables['bundle'] = $bundle;

  $variables['title'] = ($variables['view_mode'] != 'paragraphs_library_item') ? check_plain(entity_label($library_item_type, $library_item)) : '';

  $uri = entity_uri($library_item_type, $library_item);
  $variables['url'] = $uri && !empty($uri['path']) ? url($uri['path'], $uri['options']) : FALSE;

  if (isset($variables['element']['#page'])) {
    // If set by the caller, respect the page property.
    $variables['page'] = $variables['element']['#page'];
  }
  else {
    // Else, try to automatically detect it.
    $variables['page'] = $uri && !empty($uri['path']) && $uri['path'] == $_GET['q'];
  }

  $variables['content'] = [];
  $library_item_properties = [
    'title',
    'created',
    'changed',
    'uid',
    'revision_id',
  ];
  $fullpage = ($uri['path'] == current_path());
  foreach (element_children($variables['element']) as $key) {
    $element_type = (in_array($key, $library_item_properties)) ? 'property' : 'content';
    if ($element_type == 'property' && !$fullpage) {
      // Skip properties if not on the full page.
      continue;
    }
    $variables[$element_type][$key] = $variables['element'][$key];
  }
  if ($variables['view_mode'] == 'paragraphs_library_item') {
    $variables['url'] = FALSE;
  }
  // Make the field variables available with the appropriate language.
  field_attach_preprocess($library_item_type, $library_item, $variables['content'], $variables);
}
