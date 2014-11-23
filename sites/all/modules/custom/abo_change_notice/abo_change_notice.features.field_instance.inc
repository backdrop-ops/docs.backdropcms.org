<?php
/**
 * @file
 * abo_change_notice.features.field_instance.inc
 */

/**
 * Implements hook_field_default_field_instances().
 */
function abo_change_notice_field_default_field_instances() {
  $field_instances = array();

  // Exported field_instance: 'node-changenotice-body'
  $field_instances['node-changenotice-body'] = array(
    'bundle' => 'changenotice',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'Describe the change, and its impact on site users, site builders, themers, and module developers. If appropriate, give before/after code samples. Good examples: 6/7 module update guide section, 6/7 theme update guide section (though it is not necessary to link to the issue).',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(
          'trim_length' => 600,
        ),
        'type' => 'text_summary_or_trimmed',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'body',
    'label' => 'Description',
    'required' => 0,
    'settings' => array(
      'display_summary' => 0,
      'text_processing' => 1,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'rows' => 5,
        'summary_rows' => 5,
      ),
      'type' => 'text_textarea_with_summary',
      'weight' => 5,
    ),
  );

  // Exported field_instance: 'node-changenotice-field_details'
  $field_instances['node-changenotice-field_details'] = array(
    'bundle' => 'changenotice',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'Enter details of any needed updates (URLs of specific pages to be updated, what the coder routines or examples should contain, etc.).',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 7,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_details',
    'label' => 'Details',
    'required' => 0,
    'settings' => array(
      'text_processing' => 1,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'rows' => 5,
      ),
      'type' => 'text_textarea',
      'weight' => 8,
    ),
  );

  // Exported field_instance: 'node-changenotice-field_file_attachments'
  $field_instances['node-changenotice-field_file_attachments'] = array(
    'bundle' => 'changenotice',
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'file',
        'settings' => array(),
        'type' => 'file_default',
        'weight' => 5,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_file_attachments',
    'label' => 'File Attachments',
    'required' => 0,
    'settings' => array(
      'description_field' => 0,
      'file_directory' => 'changenotice',
      'file_extensions' => 'jpg jpeg gif png txt xls pdf ppt pps odt ods odp gz tgz pat ch diff zip test info po pot psd',
      'max_filesize' => '1MB',
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'file',
      'settings' => array(
        'progress_indicator' => 'bar',
      ),
      'type' => 'file_generic',
      'weight' => 6,
    ),
  );

  // Exported field_instance: 'node-changenotice-field_impacts'
  $field_instances['node-changenotice-field_impacts'] = array(
    'bundle' => 'changenotice',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'list',
        'settings' => array(),
        'type' => 'list_default',
        'weight' => 9,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_impacts',
    'label' => 'Impacts',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(),
      'type' => 'options_buttons',
      'weight' => 10,
    ),
  );

  // Exported field_instance: 'node-changenotice-field_introduced_in_branch'
  $field_instances['node-changenotice-field_introduced_in_branch'] = array(
    'bundle' => 'changenotice',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'Enter the branch this change was first applied in. Examples: 1.x, 2.x-3.x',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 2,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_introduced_in_branch',
    'label' => 'Introduced in branch',
    'required' => 0,
    'settings' => array(
      'text_processing' => 0,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'size' => 60,
      ),
      'type' => 'text_textfield',
      'weight' => 2,
    ),
  );

  // Exported field_instance: 'node-changenotice-field_introduced_in_version'
  $field_instances['node-changenotice-field_introduced_in_version'] = array(
    'bundle' => 'changenotice',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'Enter the version this change was first applied in. Examples: 2.0-ALPHA3, 1.5, 7.x-3.2',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 3,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_introduced_in_version',
    'label' => 'Introduced in version',
    'required' => 0,
    'settings' => array(
      'text_processing' => 0,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'size' => 60,
      ),
      'type' => 'text_textfield',
      'weight' => 3,
    ),
  );

  // Exported field_instance: 'node-changenotice-field_progress'
  $field_instances['node-changenotice-field_progress'] = array(
    'bundle' => 'changenotice',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'Use this field to record progress on making the needed updates',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 8,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_progress',
    'label' => 'Progress',
    'required' => 0,
    'settings' => array(
      'text_processing' => 1,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'rows' => 5,
      ),
      'type' => 'text_textarea',
      'weight' => 9,
    ),
  );

  // Exported field_instance: 'node-changenotice-field_project'
  $field_instances['node-changenotice-field_project'] = array(
    'bundle' => 'changenotice',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 1,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_project',
    'label' => 'Project',
    'required' => 1,
    'settings' => array(
      'text_processing' => 0,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'size' => 60,
      ),
      'type' => 'text_textfield',
      'weight' => 1,
    ),
  );

  // Exported field_instance: 'node-changenotice-field_term_issues'
  $field_instances['node-changenotice-field_term_issues'] = array(
    'bundle' => 'changenotice',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'Enter all issues related to this change.
',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 4,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_term_issues',
    'label' => 'Issues',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'taxonomy',
      'settings' => array(
        'autocomplete_path' => 'taxonomy/autocomplete',
        'size' => 60,
      ),
      'type' => 'taxonomy_autocomplete',
      'weight' => 4,
    ),
  );

  // Exported field_instance: 'node-changenotice-field_updates_done'
  $field_instances['node-changenotice-field_updates_done'] = array(
    'bundle' => 'changenotice',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'list',
        'settings' => array(),
        'type' => 'list_default',
        'weight' => 6,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_updates_done',
    'label' => 'Updates Done',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(),
      'type' => 'options_buttons',
      'weight' => 7,
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Describe the change, and its impact on site users, site builders, themers, and module developers. If appropriate, give before/after code samples. Good examples: 6/7 module update guide section, 6/7 theme update guide section (though it is not necessary to link to the issue).');
  t('Description');
  t('Details');
  t('Enter all issues related to this change.
');
  t('Enter details of any needed updates (URLs of specific pages to be updated, what the coder routines or examples should contain, etc.).');
  t('Enter the branch this change was first applied in. Examples: 1.x, 2.x-3.x');
  t('Enter the version this change was first applied in. Examples: 2.0-ALPHA3, 1.5, 7.x-3.2');
  t('File Attachments');
  t('Impacts');
  t('Introduced in branch');
  t('Introduced in version');
  t('Issues');
  t('Progress');
  t('Project');
  t('Updates Done');
  t('Use this field to record progress on making the needed updates');

  return $field_instances;
}
