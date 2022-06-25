<?php
/**
 * @file
 * Integration with feeds to support unique targets in mappers.
 */

/**
 * Implements hook_feeds_processor_targets_alter().
 *
 * @see field_feeds_processor_targets_alter().
 */
function field_validation_feeds_processor_targets_alter(&$targets, $entity_type, $bundle) {
  $prefix = 'field_validation.rule.' . $entity_type . '.' . $bundle;
  $configs = config_get_names_with_prefix($prefix);

  foreach ($configs as $name) {
    $config = config($name);
    $validator = $config->get('validator');

    if ($validator == 'field_validation_unique_validator') {
      if (!empty($config->get('disabled'))) {
        continue;
      }
      $field_name = $config->get('field_name');
      $targets[$field_name]['optional_unique'] = TRUE;
      $targets[$field_name]['unique_callbacks'][] = 'field_validation_feeds_existing_entity_id';
    }
  }
}

/**
 * Callback for Feeds processor unique entity ID.
 *
 * @param FeedsSource $source
 *   The Feed source.
 * @param string $entity_type
 *   Entity type for the entity to be processed.
 * @param string $bundle
 *   Bundle name for the entity to be processed.
 * @param string $target
 *   A string identifying the unique target on the entity.
 * @param array $values
 *   The unique values to be checked.
 *
 * @return int
 *   The existing entity id, or 0 if not found.
 *
 * @see field_validation_feeds_processor_targets_alter()
 * @see FeedsProcessor::existingEntityId()
 */
function field_validation_feeds_existing_entity_id(FeedsSource $source, $entity_type, $bundle, $target, array $values) {

  $prefix = 'field_validation.rule.' . $entity_type . '.' . $bundle . '.' . $target;
  $configs = config_get_names_with_prefix($prefix);

  foreach ($configs as $name) {
    $config = config($name);
    $validator = $config->get('validator');

    if ($validator == 'field_validation_unique_validator') {
      $config_values = $config->get();
      if (!empty($config_values)) {
        // Get unique entity ID from unique field value. Note that we are not using
        // field_validation_unique_validator::validate() because we don't want to
        // fire set_error().
        $query = new EntityFieldQuery();
        $result = $query
          ->entityCondition('entity_type', $entity_type)
          ->entityCondition('bundle', $bundle)
          ->fieldCondition($target, $config_values['col'], $values)
          // Run the query as user 1.
          ->addMetaData('account', user_load(1))
          ->execute();

        if (isset($result[$entity_type])) {
          return key($result[$entity_type]);
        }
      }
    }
  }
}
