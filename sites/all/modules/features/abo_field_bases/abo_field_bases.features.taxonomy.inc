<?php
/**
 * @file
 * abo_field_bases.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function abo_field_bases_taxonomy_default_vocabularies() {
  return array(
    'issues' => array(
      'name' => 'Issues',
      'machine_name' => 'issues',
      'description' => 'Required for Changenotice',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
  );
}
