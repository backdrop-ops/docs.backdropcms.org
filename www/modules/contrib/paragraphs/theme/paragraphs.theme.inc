<?php
/**
 * @file
 * Default theme functions for paragraphs.
 */

/**
 * Process variables for paragraphs-items.tpl.php
 */
function template_preprocess_paragraphs_items(&$variables, $hook) {
  $variables['view_mode'] = $variables['element']['#view_mode'];
  $variables['field_name'] = $variables['element']['#field_name'];

  $variables['content'] = $variables['element']['#children'];

  $variables['classes_array'][] = backdrop_html_class('paragraphs-items-' . $variables['element']['#field_name']);
  $variables['classes_array'][] = backdrop_html_class('paragraphs-items-' . $variables['element']['#field_name'] . '-' . $variables['view_mode']);
  $variables['classes_array'][] = backdrop_html_class('paragraphs-items-' . $variables['view_mode']);

  $variables['theme_hook_suggestions'][] = 'paragraphs_items__' . $variables['element']['#field_name'];
  $variables['theme_hook_suggestions'][] = 'paragraphs_items__' . $variables['element']['#field_name'] . '__' . $variables['view_mode'];
}

/**
 * Process variables for paragraphs-item.tpl.php
 */
function template_preprocess_paragraphs_item(&$variables, $hook) {
  $key = key($variables['elements']['entity']['paragraphs_item']);
  $variables['view_mode'] = $variables['elements']['entity']['paragraphs_item'][$key]['#view_mode'];
  $variables['bundle'] = $variables['elements']['entity']['paragraphs_item'][$key]['#bundle'];

  $variables['content'] = $variables['elements']['#children'];

  $variables['classes'][] = backdrop_html_class('paragraphs-item-' . $variables['bundle']);
  $variables['classes'][] = backdrop_html_class('paragraphs-item-' . $variables['bundle'] . '-' . $variables['view_mode']);
  $variables['classes'][] = backdrop_html_class('paragraphs-item-' . $variables['view_mode']);

  $variables['theme_hook_suggestions'][] = 'paragraphs_item__' . $variables['bundle'];
  $variables['theme_hook_suggestions'][] = 'paragraphs_item__' . $variables['bundle'] . '__' . $variables['view_mode'];
}
