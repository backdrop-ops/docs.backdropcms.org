<?php

/**
 * @file
 * Version agnostic parts of feeds_tamper.module.
 */


/**
 * @defgroup feeds_tamper_api Feeds Tamper API
 * @{
 * API functions for dealing with plugins and plugin instances.
 */

/**
 * Load all plugin instances.
 *
 * @param bool $disabled
 *   (optional) If TRUE, load disabled plugin instances. Defaults to FALSE.
 *
 * @return array
 *   An associative array of plugin instances keyed to their id.
 *
 * @todo Remove. Only used in test so don't bother with it here.
 */
// function feeds_tamper_load_all_instances($disabled = FALSE) {
//   $instances = array();

//   $configs = feeds_tamper_load_tamper_object();
//   foreach ($configs as $instance_id => $config) {
//     if (!empty($config->id) && ($disabled || empty($config->disabled))) {
//       $instances[$instance_id] = $config;
//     }
//   }
//   return $instances;
// }

/**
 * Load a plugin instance by id.
 *
 * @param FeedsImporter $importer
 *
 * @param string $instance_id
 *   The id of the plugin instance.
 *
 * @return stdClass
 *   A plugin instance object.
 */
function feeds_tamper_load_instance(FeedsImporter $importer, $instance_id) {
  $mappings = $importer->processor->config['mappings'];
  $instance = array();
  foreach ($mappings as $mapping) {
    if (isset($mapping['tampers'][$instance_id])) {
      $instance = $mapping['tampers'][$instance_id];
      // Load source along with instance to provide some context.
      $instance['source'] = $mapping['source'];
    }
  }

  return (object) $instance;
}

/**
 * Save a plugin instance.
 *
 * @param FeedsImporter $importer
 *
 * @param stdClass $instance
 *   A plugin instance object.
 *
 * @param string $source_id
 *
 * @return mixed
 *   If the plugin save failed, returns FALSE. If it succeeded, returns
 *   SAVED_NEW or SAVED_UPDATED, depending on the operation performed.
 */
function feeds_tamper_save_instance(FeedsImporter $importer, $instance, $source_id) {
  $importer_config = config('feeds.feeds_importer.' . $importer->__get('id'));

  $mappings = $importer_config->get('config.processor.config.mappings');
  foreach ($mappings as $key => $mapping) {
    if (!isset($mapping['tampers'])) {
      $mapping['tampers'] = array();
      $importer_config->set('config.processor.config.mappings.' . $key . '.tampers', array());
    }

    if ($mapping['source'] == $source_id ) {
      // It's a new instance, give it the heaviest weight.
      if (!isset($instance->weight)) {
        $weight = 0;
        foreach ($mapping['tampers'] as $i) {
          if ($i['weight'] >= $weight) {
            $weight = $i['weight'] + 1;
          }
        }
        $instance->weight = $weight;
      }

      if (!isset($instance->settings)) {
        $instance->settings = array();
      }

      $instance->disabled = !empty($instance->disabled) ? TRUE : FALSE;

      $importer_config->set('config.processor.config.mappings.' . $key . '.tampers.' . $instance->id, $instance);
    }
  }

  $importer_config->save();
}

/**
 * Delete a single plugin instance.
 *
 * @param FeedsImporter $importer
 *
 * @param object $instance
 *   A plugin instance object.
 */
function feeds_tamper_delete_instance(FeedsImporter $importer, stdClass $instance) {
  $importer_config = config('feeds.feeds_importer.' . $importer->__get('id'));
  $mappings = $importer_config->get('config.processor.config.mappings');
  foreach ($mappings as $key => $mapping) {
    $importer_config->clear('config.processor.config.mappings.' . $key . '.tampers.' . $instance->id);
  }

  $importer_config->save();
}

/**
 * Load plugin instances by importer id.
 *
 * @param FeedsImporter $importer
 *   The importer id, or object to reference.
 * @param bool $disabled
 *   (optional) If TRUE load disabled plugin instances. Defaults to FALSE.
 *
 * @return array
 *   An associative array of plugin instances, keyed by source.
 */
function feeds_tamper_load_by_importer(FeedsImporter $importer, $disabled = FALSE) {

  $importer_config = config('feeds.feeds_importer.' . $importer->__get('id'));
  $mappings = $importer_config->get('config.processor.config.mappings');
  $return = array();
  foreach ($mappings as $mapping) {
    if (!isset($mapping['tampers'])) {
      continue;
    }
    foreach ($mapping['tampers'] as $tamper) {
      $tamper = (object) $tamper;
      if ($disabled || !$tamper->disabled)
      $return[$mapping['source']][$tamper->id] = $tamper;
    }
  }

  foreach ($return as &$instance_list) {
    usort($instance_list, '_feeds_tamper_cmp');
  }

  return $return;
}

/**
 * Returns a unique list of sources in order.
 *
 * @param FeedsImporter $importer
 *   The importer.
 *
 * @param boolean $lower
 *
 * @return array
 *   A list of sources.
 */
function feeds_tamper_get_unique_source_list(FeedsImporter $importer, $lower = TRUE) {
  $sources = array();
  $is_csv = $lower && ($importer->parser instanceof FeedsCSVParser);

  foreach ($importer->processor->getMappings() as $mapping) {
    if ($is_csv) {
      $sources[] = backdrop_strtolower($mapping['source']);
    }
    else {
      $sources[] = $mapping['source'];
    }
  }

  return array_unique($sources);
}

/**
 * Comparison callback that sorts by weight, then alphabetically by id.
 */
function _feeds_tamper_cmp($a, $b) {
  // If weights are equal compare id's.
  if ($a->weight == $b->weight) {
    $tmp = array($a->id, $b->id);
    sort($tmp);
    if ($tmp[0] == $a->id) {
      return -1;
    }
    return 1;
  }
  return ($a->weight < $b->weight) ? -1 : 1;
}

/**
 * Get all available plugins.
 *
 * @return array
 *   An associative array where the keys are the plugin keys and the values are
 *   the plugin info arrays as defined in a plugin include file.
 */
function feeds_tamper_get_plugins() {
  $plugins = module_invoke_all('feeds_tamper_plugins');

  return $plugins;
}

/**
 * Get a single plugin.
 *
 * @param string $id
 *   The id of a plugin.
 *
 * @return array
 *   An array of config for one plugin.
 */
function feeds_tamper_get_plugin($id) {
  $plugins = module_invoke_all('feeds_tamper_plugins');

  return $plugins[$id];
}

/**
 * Return a machine name safe version of a string.
 *
 * @param string $string
 *   String to get machine nameized.
 *
 * @return string
 *   A lowercase string with all values not in [a-zA-Z0-9] replaced with an
 *   underscore and shortened to 128 characters.
 */
function feeds_tamper_make_machine($string) {
  return backdrop_substr(preg_replace('/[^a-z0-9-]/u', '_', backdrop_strtolower($string)), 0, 127);
}

/**
 * @} End of "feeds_tamper_api".
 */

/**
 * Menu access callback.
 *
 * @param FeedsImporter $importer
 *   The importer or importer id being tampered with.
 *
 * @return bool
 *   TRUE if the user has access, FALSE if not.
 */
function feeds_tamper_access(FeedsImporter $importer) {
  if (user_access('administer feeds_tamper')) {
    return TRUE;
  }

  // Check for permissions, otherwise return FALSE.
  if (user_access('tamper ' . $importer->__get('id'))) {
    return TRUE;
  }

  return FALSE;
}
