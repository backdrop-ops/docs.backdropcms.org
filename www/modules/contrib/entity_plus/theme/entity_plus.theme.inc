<?php

/**
 * @file
 * Holds the Entity Plus module's theme functions.
 */

/**
 * Theme process function for theme_entity_plus_property().
 *
 * Taken over from template_process_field()
 *
 * @see theme_entity_plus_property()
 */
function template_process_entity_plus_property(&$variables, $hook) {
  $element = $variables['elements'];
  // The default theme implementation is a function, so template_process() does
  // not automatically run, so we need to flatten the classes and attributes
  // here. For best performance, only call backdrop_attributes() when needed, and
  // note that template_preprocess_field() does not initialize the
  // *_attributes_array variables.
  $variables['attributes'] = empty($variables['attributes_array']) ? '' : backdrop_attributes($variables['attributes_array']);
  $variables['title_attributes'] = empty($variables['title_attributes_array']) ? '' : backdrop_attributes($variables['title_attributes_array']);
  $variables['content_attributes'] = empty($variables['content_attributes_array']) ? '' : backdrop_attributes($variables['content_attributes_array']);
}

/**
 * Process variables for entity_plus.tpl.php.
 */
function template_preprocess_entity_plus(&$variables) {
  $variables['view_mode'] = $variables['elements']['#view_mode'];
  $entity_type = $variables['elements']['#entity_plus_type'];
  $variables['entity_type'] = $entity_type;
  $entity = $variables['elements']['#entity'];
  $variables[$entity_type] = $entity;
  $info = entity_get_info($entity_type);

  $variables['title'] = check_plain(entity_label($entity_type, $entity));

  $uri = entity_uri($entity_type, $entity);
  $variables['url'] = $uri && !empty($uri['path']) ? url($uri['path'], $uri['options']) : FALSE;

  if (isset($variables['elements']['#page'])) {
    // If set by the caller, respect the page property.
    $variables['page'] = $variables['elements']['#page'];
  }
  else {
    // Else, try to automatically detect it.
    $variables['page'] = $uri && !empty($uri['path']) && $uri['path'] == $_GET['q'];
  }

  // Helpful $content variable for templates.
  $variables['content'] = array();
  foreach (element_children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }

  if (!empty($info['fieldable'])) {
    // Make the field variables available with the appropriate language.
    field_attach_preprocess($entity_type, $entity, $variables['content'], $variables);
  }
  list(, , $bundle) = entity_extract_ids($entity_type, $entity);

  // Gather css classes.
  $variables['classes_array'][] = backdrop_html_class('entity-' . $entity_type);
  $variables['classes_array'][] = backdrop_html_class($entity_type . '-' . $bundle);

  // Add RDF type and about URI.
  if (module_exists('rdf')) {
    $variables['attributes_array']['about'] = empty($uri['path']) ? NULL: url($uri['path']);
    $variables['attributes_array']['typeof'] = empty($entity->rdf_mapping['rdftype']) ? NULL : $entity->rdf_mapping['rdftype'];
  }

  // Add suggestions.
  $variables['theme_hook_suggestions'][] = $entity_type;
  $variables['theme_hook_suggestions'][] = $entity_type . '__' . $bundle;
  $variables['theme_hook_suggestions'][] = $entity_type . '__' . $bundle . '__' . $variables['view_mode'];
  if ($id = entity_plus_id($entity_type, $entity)) {
    $variables['theme_hook_suggestions'][] = $entity_type . '__' . $id;
  }
}
