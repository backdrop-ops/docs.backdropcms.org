<?php

/**
 * @file
 * Provides info about the taxonomy entity.
 */

/**
 * Implements hook_entity_property_info() on top of taxonomy module.
 *
 * @see entity_plus_entity_property_info()
 */
function entity_plus_metadata_taxonomy_entity_property_info() {
  $info = array();
  // Add meta-data about the basic taxonomy properties.
  $properties = &$info['taxonomy_term']['properties'];

  $properties['tid'] = array(
    'label' => t("Term ID"),
    'description' => t("The unique ID of the taxonomy term."),
    'type' => 'integer',
    'schema field' => 'tid',
  );
  $properties['name'] = array(
    'label' => t("Name"),
    'description' => t("The name of the taxonomy term."),
    'setter callback' => 'entity_plus_property_verbatim_set',
    'required' => TRUE,
    'schema field' => 'name',
  );
  $properties['description'] = array(
    'label' => t("Description"),
    'description' => t("The optional description of the taxonomy term."),
    'sanitized' => TRUE,
    'raw getter callback' => 'entity_plus_property_verbatim_get',
    'getter callback' => 'entity_plus_metadata_taxonomy_term_get_properties',
    'setter callback' => 'entity_plus_property_verbatim_set',
    'schema field' => 'description',
  );
  $properties['weight'] = array(
    'label' => t("Weight"),
    'type' => 'integer',
    'description' => t('The weight of the term, which is used for ordering terms during display.'),
    'setter callback' => 'entity_plus_property_verbatim_set',
    'schema field' => 'weight',
  );
  $properties['node_count'] = array(
    'label' => t("Node count"),
    'type' => 'integer',
    'description' => t("The number of nodes tagged with the taxonomy term."),
    'getter callback' => 'entity_plus_metadata_taxonomy_term_get_properties',
    'computed' => TRUE,
  );
  $properties['url'] = array(
    'label' => t("URL"),
    'description' => t("The URL of the taxonomy term."),
    'getter callback' => 'entity_plus_metadata_entity_plus_get_properties',
    'type' => 'uri',
    'computed' => TRUE,
  );
  $properties['vocabulary'] = array(
    'label' => t("Vocabulary"),
    'description' => t("The vocabulary the taxonomy term belongs to."),
    'setter callback' => 'entity_plus_metadata_taxonomy_term_setter',
    'getter callback' => 'entity_plus_metadata_taxonomy_term_get_properties',
    'raw getter callback' => 'entity_plus_property_verbatim_get',
    'type' => 'struct',
    'property info' => array(
      'name' => array(
        'label' => t("Name"),
        'description' => t("The name of the taxonomy vocabulary."),
        'setter callback' => 'entity_plus_property_verbatim_set',
        'required' => TRUE,
      ),
      'machine_name' => array(
        'label' => t("Machine name"),
        'type' => 'token',
        'description' => t("The machine name of the taxonomy vocabulary."),
        'setter callback' => 'entity_plus_property_verbatim_set',
        'required' => TRUE,
      ),
      'description' => array(
        'label' => t("Description"),
        'description' => t("The optional description of the taxonomy vocabulary."),
        'setter callback' => 'entity_plus_property_verbatim_set',
        'sanitize' => 'filter_xss',
      ),
      'term_count' => array(
        'label' => t("Term count"),
        'type' => 'integer',
        'description' => t("The number of terms belonging to the taxonomy vocabulary."),
        'getter callback' => 'entity_plus_metadata_taxonomy_vocabulary_get_properties',
        'computed' => TRUE,
      ),
    ),
  );
  $properties['parent'] = array(
    'label' => t("Parent terms"),
    'description' => t("The parent terms of the taxonomy term."),
    'getter callback' => 'entity_plus_metadata_taxonomy_term_get_properties',
    'setter callback' => 'entity_plus_metadata_taxonomy_term_setter',
    'type' => 'list<taxonomy_term>',
  );
  $properties['parents_all'] = array(
    'label' => t("All parent terms"),
    'description' => t("Ancestors of the term, i.e. parent of all above hierarchy levels."),
    'getter callback' => 'entity_plus_metadata_taxonomy_term_get_properties',
    'type' => 'list<taxonomy_term>',
    'computed' => TRUE,
  );

  return $info;
}
