<?php

/**
 * @file
 * References Feed File.
 *
 * Mapper that handles any fields added by the References (node_reference,
 * user_reference)
 *
 * Code is effectively a port of the mapper_for_nodereference_field patch to
 * drupal 7, and borrows
 * heavily from the nodereference patch.
 */

/**
 * Implements hook_feeds_parser_sources_alter().
 */
function node_reference_feeds_parser_sources_alter(&$sources, $content_type) {
  if (!empty($content_type)) {
    $fields = field_info_fields();
    foreach ($fields as $field) {
      if ($field['type'] == 'node_reference' && isset($field['bundles']['node']) && in_array($content_type, $field['bundles']['node'])) {
        $sources['parent:node_reference:' . $field['field_name']] = array(
          'name' => t('Feed node: Node Reference (nid): @field_name', array('@field_name' => $field['field_name'])),
          'description' => t('Node References from the parent feed node.'),
          'callback' => 'node_reference_feeds_get_source',
        );
      }
    }
  }
  return $sources;
}

/**
 * Callback for mapping parent node references to child node reference values.
 *
 * @param object $source
 *   A FeedsSource object.
 * @param object $result
 *   FeedsParserResult object.
 * @param string $key
 *   The key as defined in the _feeds_parser_sources_alter() hook defined above.
 *
 * @return array
 *   The node ids that the parent node references.
 *
 * @codingStandardsIgnoreStart
 */
function node_reference_feeds_get_source(FeedsSource $source, FeedsParserResult $result, $key) {
  // @codingStandardsIgnoreEnd
  if ($node = node_load($source->feed_nid)) {
    $results = array();
    $field = substr($key, 22);
    if (!empty($node->{$field}[LANGUAGE_NONE])) {
      foreach ($node->{$field}[LANGUAGE_NONE] as $value) {
        $results[] = $value[LANGUAGE_NONE];
      }
    }
    return $results;
  }
}

/**
 * Implements hook_feeds_processor_targets_alter() for node_reference fields.
 *
 * @see FeedsNodeProcessor::getMappingTargets()
 */
function node_reference_feeds_processor_targets_alter(&$targets, $entity_type, $bundle_name) {
  foreach (field_info_instances($entity_type, $bundle_name) as $name => $instance) {
    $info = field_info_field($name);
    if ($info['type'] == 'node_reference') {

      $targets[$name . ':title'] = array(
        'name' => t('@label (Node reference by node title)', array('@label' => $instance['label'])),
        'callback' => 'references_feeds_set_target',
        'description' => t('The @label field of the node matched by node title.', array('@label' => $instance['label'])),
        'real_target' => $name,
      );

      $targets[$name . ':nid'] = array(
        'name' => t('@label (Node reference by node ID)', array('@label' => $instance['label'])),
        'callback' => 'references_feeds_set_target',
        'description' => t('The @label field of the node matched by node ID.', array('@label' => $instance['label'])),
        'real_target' => $name,
      );

      $targets[$name . ':url'] = array(
        'name' => t('@label (Node reference by Feeds URL)', array('@label' => $instance['label'])),
        'callback' => 'references_feeds_set_target',
        'description' => t('The @label field of the node matched by Feeds URL.', array('@label' => $instance['label'])),
        'real_target' => $name,
      );

      $targets[$name . ':guid'] = array(
        'name' => t('@label (Node reference by Feeds GUID)', array('@label' => $instance['label'])),
        'callback' => 'references_feeds_set_target',
        'description' => t('The @label field of the node matched by Feeds GUID.', array('@label' => $instance['label'])),
        'real_target' => $name,
      );

      $targets[$name . ':title:duplicates'] = array(
        'name' => t('@label (Node reference by node title) -- allow duplicate nodes', array('@label' => $instance['label'])),
        'callback' => 'references_feeds_set_target',
        'description' => t('The @label field of the node matched by node title. This target allows duplicate nodes (nodes can appear more than once in a field).', array('@label' => $instance['label'])),
        'real_target' => $name,
      );

      $targets[$name . ':nid:duplicates'] = array(
        'name' => t('@label (Node reference by node ID) -- allow duplicate nodes', array('@label' => $instance['label'])),
        'callback' => 'references_feeds_set_target',
        'description' => t('The @label field of the node matched by node ID. This target allows duplicate nodes (nodes can appear more than once in a field).', array('@label' => $instance['label'])),
        'real_target' => $name,
      );

      $targets[$name . ':url:duplicates'] = array(
        'name' => t('@label (Node reference by Feeds URL) -- allow duplicate nodes', array('@label' => $instance['label'])),
        'callback' => 'references_feeds_set_target',
        'description' => t('The @label field of the node matched by Feeds URL.', array('@label' => $instance['label'])),
        'real_target' => $name,
      );

      $targets[$name . ':guid:duplicates'] = array(
        'name' => t('@label (Node reference by Feeds GUID) -- allow duplicate nodes', array('@label' => $instance['label'])),
        'callback' => 'references_feeds_set_target',
        'description' => t('The @label field of the node matched by Feeds GUID.', array('@label' => $instance['label'])),
        'real_target' => $name,
      );
    }
  }
}

/**
 * Implements hook_feeds_processor_targets_alter() for user_reference fields.
 *
 * @see FeedsNodeProcessor::getMappingTargets()
 */
function user_reference_feeds_processor_targets_alter(&$targets, $entity_type, $bundle_name) {
  foreach (field_info_instances($entity_type, $bundle_name) as $name => $instance) {
    $info = field_info_field($name);
    if ($info['type'] == 'user_reference') {

      $targets[$name . ':name'] = array(
        'name' => t('@label (User reference by user name)', array('@label' => $instance['label'])),
        'callback' => 'references_feeds_set_target',
        'description' => t('The @label field of the user matched by user name.', array('@label' => $instance['label'])),
        'real_target' => $name,
      );

      $targets[$name . ':uid'] = array(
        'name' => t('@label (User reference by user ID)', array('@label' => $instance['label'])),
        'callback' => 'references_feeds_set_target',
        'description' => t('The @label field of the user matched by user ID.', array('@label' => $instance['label'])),
        'real_target' => $name,
      );

      $targets[$name . ':url'] = array(
        'name' => t('@label (User reference by Feeds URL)', array('@label' => $instance['label'])),
        'callback' => 'references_feeds_set_target',
        'description' => t('The @label field of the node matched by Feeds URL.', array('@label' => $instance['label'])),
        'real_target' => $name,
      );

      $targets[$name . ':guid'] = array(
        'name' => t('@label (User reference by Feeds GUID)', array('@label' => $instance['label'])),
        'callback' => 'references_feeds_set_target',
        'description' => t('The @label field of the node matched by Feeds GUID.', array('@label' => $instance['label'])),
        'real_target' => $name,
      );

      $targets[$name . ':name:duplicates'] = array(
        'name' => t('@label (User reference by user name) -- allow duplicate users', array('label' => $instance['label'])),
        'callback' => 'references_feeds_set_target',
        'description' => t('The @label field of the user matched by user name. This target allows duplicate users (users can appear more than once in a field).', array('@label' => $instance['label'])),
        'real_target' => $name,
      );

      $targets[$name . ':uid:duplicates'] = array(
        'name' => t('@label (User reference by user ID) -- allow duplicate users', array('@label' => $instance['label'])),
        'callback' => 'references_feeds_set_target',
        'description' => t('The @label field of the user matched by user ID. This target allows duplicate users (users can appear more than once in a field).', array('@label' => $instance['label'])),
        'real_target' => $name,
      );

      $targets[$name . ':url:duplicates'] = array(
        'name' => t('@label (User reference by Feeds URL) -- allow duplicate users', array('@label' => $instance['label'])),
        'callback' => 'references_feeds_set_target',
        'description' => t('The @label field of the node matched by Feeds URL.', array('@label' => $instance['label'])),
        'real_target' => $name,
      );

      $targets[$name . ':guid:duplicates'] = array(
        'name' => t('@label (User reference by Feeds GUID) -- allow duplicate users', array('@label' => $instance['label'])),
        'callback' => 'references_feeds_set_target',
        'description' => t('The @label field of the node matched by Feeds GUID.', array('@label' => $instance['label'])),
        'real_target' => $name,
      );
    }
  }
}

/**
 * Callback for mapping both node reference and user_reference fields.
 *
 * Implementation of hook_feeds_set_target().
 *
 * @param object $source
 *   A FeedsSource object.
 * @param object $entity
 *   The entity to map to.
 * @param string $target
 *   The target key on $entity to map to.
 * @param mixed $value
 *   The value to map. Can be an array or a string.
 */
function references_feeds_set_target($source, $entity, $target, $value) {
  if (empty($value) || empty($value[key($value)])) {
    return;
  }

  // Handle comma delimited or non-multiple values.
  if (!is_array($value)) {
    $value = array($value);
  }

  // Determine the field we are matching against, and whether duplicates are
  // allowed.
  $target_info = explode(':', $target, 3);
  if (count($target_info) == 3) {
    list($target, $match_key, $duplicates) = $target_info;
  }
  else {
    list($target, $match_key) = $target_info;
  }

  // Load field definition.
  $info = field_info_field($target);
  // Parameters to handle differences between node references and user
  // references.
  if ($info['type'] == 'user_reference') {
    $idname = 'uid';
    $typename = 'user';
    $validate_function = 'user_reference_potential_references';
  }
  else {
    $idname = 'nid';
    $typename = 'node';
    $validate_function = 'node_reference_potential_references';
  }

  $field = isset($entity->$target) ? $entity->$target : array();
  if (!isset($field[LANGUAGE_NONE])) {
    $field[LANGUAGE_NONE] = array();
  }

  // Match values against nodes and add to field.
  foreach ($value as $v) {
    // Create options.
    $options = array(
      'string' => $v,
      'match' => 'equals',
      'ids' => array(),
      'limit' => 1,
    );

    switch ($match_key) {
      case 'title':
      case 'name':
        // Validate node title or user name.
        if ((is_string($options['string']) && $options['string'] != '') || is_numeric($options['string'])) {
          // Lookup potential exact matches for the value (limit to one result).
          $matches = $validate_function($info, $options);
          // Use the first element of the potential matches.
          $options['ids'] = key($matches);
        }

        // Alert if no match is found.
        if (empty($options['ids'])) {
          backdrop_set_message(t('%title does not match an existing @typename', array('%title' => $options['string'], '@typename' => $typename)));
        }
        break;

      case 'nid':
      case 'uid':
        // Make sure it is a positive integer.
        if ((is_int($options['string']) || ctype_digit($options['string'])) && $options['string'] > 0 && $options['string'] !== '') {
          // Make sure it is a valid node id or user id for this field.
          $matches = $validate_function($info, array($options['string']));
          foreach ($matches as $k => $v) {
            if ($options['string'] == $k) {
              $options['ids'] = $k;
            }
          }
        }

        // Alert if no match is found.
        if (empty($options['ids'])) {
          backdrop_set_message(t('%id is not a valid @typename id for this field.', array('%id' => $options['string'], '@typename' => $typename)));
        }
        break;

      case 'guid':
      case 'url':
        // Get the value from table feeds-item.
        $result = db_query('SELECT f.entity_id FROM {feeds_item} f WHERE f.' . $match_key . ' = :v', array(':v' => $v));
        $options['ids'] = $result->fetchField();

        // Alert if no match is found.
        if (empty($options['ids'])) {
          backdrop_set_message(t('%id is not a valid @typename id for this field.', array('%id' => $v, '@typename' => $typename)));
        }
        break;
    }

    if (!empty($options['ids'])) {
      $reference = array($idname => $options['ids']);
      if (!empty($duplicates)) {
        // Add the reference, ignoring duplicates.
        $field[LANGUAGE_NONE][] = $reference;
      }
      elseif (!in_array($reference, $field[LANGUAGE_NONE])) {
        // Add the reference only if it doesn't already exist.
        $field[LANGUAGE_NONE][] = $reference;
      }

      if ($info['cardinality'] == 1) {
        break;
      }
    }
  }
  $entity->{$target} = $field;
}
