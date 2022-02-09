<?php

/**
 * @file
 * Default theme functions for paragraphs.
 */

/**
 * Process variables for paragraphs-items.tpl.php.
 */
function template_preprocess_paragraphs_items(&$variables, $hook) {
  $view_mode = isset($variables['element']['#paragraphs_items_view_mode']) ? $variables['element']['#paragraphs_items_view_mode'] : isset($variables['element']['#view_mode']);
  $field_name = $variables['element']['#field_name'];

  $variables['view_mode'] = $view_mode;
  $variables['field_name'] = $field_name;
  $variables['content'] = $variables['element']['#children'];

  $variables['classes'][] = backdrop_html_class('paragraphs-items-' . $field_name);
  $variables['classes'][] = backdrop_html_class('paragraphs-items-' . $field_name . '-' . $view_mode);
  $variables['classes'][] = backdrop_html_class('paragraphs-items-' . $view_mode);

  $variables['theme_hook_suggestions'][] = 'paragraphs_items__' . $field_name;
  $variables['theme_hook_suggestions'][] = 'paragraphs_items__' . $field_name . '__' . $view_mode;
}

/**
 * Process variables for paragraphs-item.tpl.php.
 */
function template_preprocess_paragraphs_item(&$variables, $hook) {
  $variables['view_mode'] = $variables['elements']['#view_mode'];
  $paragraphs_item_type = $variables['elements']['#entity_plus_type'];
  $variables['entity_type'] = $paragraphs_item_type;
  $paragraphs_item = $variables['elements']['#entity'];
  $variables[$paragraphs_item_type] = $paragraphs_item;

  list(, , $bundle) = entity_extract_ids($paragraphs_item_type, $paragraphs_item);
  $variables['bundle'] = $bundle;

  $paragraphs_item_id = entity_plus_id($paragraphs_item_type, $paragraphs_item);

  $variables['title'] = check_plain(entity_label($paragraphs_item_type, $paragraphs_item));

  $uri = entity_uri($paragraphs_item_type, $paragraphs_item);
  $variables['url'] = $uri && !empty($uri['path']) ? url($uri['path'], $uri['options']) : FALSE;

  if (isset($variables['elements']['#page'])) {
    // If set by the caller, respect the page property.
    $variables['page'] = $variables['elements']['#page'];
  }
  else {
    // Else, try to automatically detect it.
    $variables['page'] = $uri && !empty($uri['path']) && $uri['path'] == $_GET['q'];
  }

  $variables['content'] = array();
  foreach (element_children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }
  // Make the field variables available with the appropriate language.
  field_attach_preprocess($paragraphs_item_type, $paragraphs_item, $variables['content'], $variables);

  // Gather css classes.
  $variables['classes_array'][] = backdrop_html_class($paragraphs_item_type);
  $variables['classes_array'][] = backdrop_html_class($paragraphs_item_type . '-' . $bundle);
  $variables['classes_array'][] = backdrop_html_class('paragraphs-item-' . $bundle);
  $variables['classes_array'][] = backdrop_html_class('paragraphs-item-' . $variables['elements']['#view_mode']);
  $variables['classes_array'][] = backdrop_html_class('paragraphs-item-' . $paragraphs_item_id);

  // Add suggestions.
  $variables['theme_hook_suggestions'][] = $paragraphs_item_type;
  $variables['theme_hook_suggestions'][] = $paragraphs_item_type . '__' . $bundle;
  $variables['theme_hook_suggestions'][] = $paragraphs_item_type . '__' . $bundle . '__' . $variables['view_mode'];
  $variables['theme_hook_suggestions'][] = $paragraphs_item_type . '__' . $paragraphs_item_id;

  // Add the modal css class if enabled.
  if (!paragraphs_item_access('delete', $paragraphs_item)
    || !paragraphs_item_access('create', $paragraphs_item)
    || !paragraphs_item_access('update', $paragraphs_item)
    || empty($variables['elements']['#modal_admin'])) {
    return;
  }
  $variables['classes_array'][] = backdrop_html_class('paragraphs-item-modal-admin');
  if (paragraphs_item_access('delete', $paragraphs_item)) {
    $variables['admin_links'][$paragraphs_item_id] = paragraphs_modal_admin_links($paragraphs_item);
  }
}
