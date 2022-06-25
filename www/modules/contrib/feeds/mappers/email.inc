<?php
/**
 * @file
 * On behalf implementation of Feeds mapping API for email.module.
 */

/**
 * Implements hook_feeds_processor_targets().
 *
 * @see FeedsNodeProcessor::getMappingTargets().
 */
function email_feeds_processor_targets($entity_type, $bundle_name) {
  $targets = array();

  foreach (field_info_instances($entity_type, $bundle_name) as $name => $instance) {
    $info = field_info_field($name);

    if (in_array($info['type'], array('email'))) {
      $targets[$name] = array(
        'name' => check_plain($instance['label']),
        'callback' => 'email_feeds_set_target',
        'description' => t('The @label field of the entity.', array('@label' => $instance['label'])),
      );
    }
  }

  return $targets;
}

/**
 * Callback function for mapping email field.
 *
 * @param $target
 *   the name of the field the user has decided to map to.
 * @param $value
 *   the value of the feed item element the user has picked as a source.
 */
function email_feeds_set_target(FeedsSource $source, $entity, $target, array $values, array $mapping) {
  $language = $mapping['language'];

  $info = field_info_field($target);

  // Iterate over all values.
  $field = isset($entity->$target) ? $entity->$target : array($language => array());
  $delta = 0;

  foreach ($values as $value) {

    if (is_object($value) && ($value instanceof FeedsElement)) {
      $value = $value->getValue();
    }

    if (is_scalar($value)) {
      $field[$language][$delta]['email'] = (string) $value;
    }

    if ($info['cardinality'] == 1) {
      break;
    }
    $delta++;
  }

  $entity->{$target} = $field;
}
