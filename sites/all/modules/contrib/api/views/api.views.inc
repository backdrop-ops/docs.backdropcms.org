<?php
/**
 * Views integration for the API module.
 */

/**
 * Implements hook_views_data().
 *
 * Defines the base data table for making Views of API documentation items.
 */
function api_views_data() {
  $data = array();

  $data['api_documentation'] = array(
    'table' => array(
      'group' => t('API documentation'),
      'base' => array(
        'field' => 'did',
        'title' => t('API documentation'),
        'help' => t('Documentation objects from the API module (representing functions, classes, etc.)'),
        'weight' => 20,
      ),
    ),

    'did' => array(
      'title' => t('Documentation ID'),
      'help' => t('Numeric ID of the documentation item'),
      'relationship' => array(
        'base' => 'comment',
        'base field' => 'nid',
        'handler' => 'views_handler_relationship',
        'label' => t('Comments'),
        'title' => t('All comments'),
        'help' => t('All comments on this documentation object. This will create duplicate records, so you probably want a comment view instead.'),
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
    ),

    'namespace_did' => array(
      'title' => t('Namespace dummy field'),
      'relationship' => array(
        'base' => 'api_namespace',
        'base field' => 'did',
        'field' => 'did',
        'handler' => 'views_handler_relationship',
        'label' => t('All namespace information for file'),
        'title' => t('All namespace information for file'),
        'help' => t('All information on namespaces and use aliases for this file. This will create duplicate records, unless you filter.'),
      ),
    ),

    'member_did' => array(
      'title' => t('Member dummy field'),
      'relationship' => array(
        'base' => 'api_members',
        'base field' => 'class_did',
        'field' => 'did',
        'handler' => 'views_handler_relationship',
        'label' => t('All members of this class'),
        'title' => t('All members of this class'),
        'help' => t('All methods, properties, and constants in this class. This will create duplicate records, unless you filter.'),
      ),
    ),

    'override_did' => array(
      'title' => t('Override dummy field'),
      'relationship' => array(
        'base' => 'api_overrides',
        'base field' => 'did',
        'field' => 'did',
        'handler' => 'views_handler_relationship',
        'label' => t('Overrides information'),
        'title' => t('Overrides information'),
        'help' => t('Information about whether this class member overrides or implements a parent member'),
      ),
    ),

    'references_did' => array(
      'title' => t('References dummy field'),
      'relationship' => array(
        'base' => 'api_reference_storage',
        'base field' => 'from_did',
        'field' => 'did',
        'handler' => 'views_handler_relationship',
        'label' => t('All references to this item'),
        'title' => t('All references to this item'),
        'help' => t('All references to this item. This will create duplicate records, unless you filter.'),
      ),
    ),

    'branch_id' => array(
      'title' => t('Branch'),
      'help' => t('Numeric ID of the branch on documentation item'),
      'relationship' => array(
        'base' => 'api_branch',
        'handler' => 'views_handler_relationship',
        'label' => t('Branch'),
        'title' => t('Branch'),
        'help' => t('The branch this documentation is in'),
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
    ),

    'object_name' => array(
      'title' => t('Object name'),
      'help' => t('Name of this object'),
      'field' => array(
        'handler' => 'api_views_handler_field_api_linkable',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'argument' => array(
        'handler' => 'api_views_handler_argument_partial_match',
      ),
    ),

    'title' => array(
      'title' => t('Title'),
      'help' => t('Title of this object'),
      'field' => array(
        'handler' => 'api_views_handler_field_api_linkable',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'argument' => array(
        'handler' => 'api_views_handler_argument_partial_match',
      ),
    ),

    'member_name' => array(
      'title' => t('Member name'),
      'help' => t('For class members, the name without the Class:: prefix; for others, blank.'),
      'field' => array(
        'handler' => 'api_views_handler_field_api_linkable',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
    ),

    'class_did' => array(
      'title' => t('Class documentation ID'),
      'help' => t('Class ID'),
      'relationship' => array(
        'base' => 'api_documentation',
        'base field' => 'did',
        'handler' => 'views_handler_relationship',
        'label' => t('Class object'),
        'title' => t('Class object'),
        'help' => t('The documentation object for the class this object is a member of'),
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
    ),

    'summary' => array(
      'title' => t('Summary'),
      'help' => t('One-line summary of documentation'),
      'field' => array(
        'handler' => 'api_views_handler_field_api_docs',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'argument' => array(
        'handler' => 'api_views_handler_argument_partial_match',
      ),
    ),

    'object_type' => array(
      'title' => t('Object type'),
      'help' => t('Type of this object (file, function, etc.)'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
    ),

    'file_name' => array(
      'title' => t('File name'),
      'help' => t('File this object is in, relative to branch directory'),
      'relationship' => array(
        'base' => 'api_documentation',
        'base field' => 'file_name',
        'handler' => 'views_handler_relationship',
        'join_handler' => 'api_views_join_extras',
        'extra' => array(
          array('field' => 'object_type', 'value' => 'file'),
          array('field' => 'branch_id = %alias.branch_id', 'prepend_left' => TRUE),
        ),
        'label' => t('File object'),
        'title' => t('File object'),
        'help' => t('The documentation object for the file this object is in'),
      ),
      'field' => array(
        'handler' => 'api_views_handler_field_api_linkable',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
    ),

    'start_line' => array(
      'title' => t('Start line'),
      'help' => t('Starting line of the code within the file'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
      ),
    ),

    'namespace' => array(
      'title' => t('Namespace'),
      'help' => t('Namespace this object is in'),
      'field' => array(
        'handler' => 'api_views_handler_field_api_namespace',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
    ),

    'modifiers' => array(
      'title' => t('Modifiers'),
      'help' => t('Modifiers such as abstract, static, etc.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
    ),

    'deprecated' => array(
      'title' => t('Deprecated'),
      'help' => t('Whether this item is deprecated or not'),
      'filter' => array(
        'handler' => 'views_handler_filter_boolean_operator_string',
      ),
    ),

  );

  $data['api_branch'] = array(
    'table' => array(
      'group' => t('API documentation'),
      'base' => array(
        'field' => 'branch_id',
        'title' => t('API branch'),
        'help' => t('Branches from the API module'),
        'weight' => 21,
      ),
    ),

    'branch_id' => array(
      'title' => t('Branch ID'),
      'help' => t('Numeric ID of the branch on branch item'),
      'relationship' => array(
        'base' => 'api_documentation',
        'handler' => 'views_handler_relationship',
        'label' => t('Documentation objects'),
        'title' => t('All documentation objects'),
        'help' => t('All documentation objects in this branch. This will create duplicate records, so you probably want a documentation view instead.'),
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
    ),

    'branch_name' => array(
      'title' => t('Branch name'),
      'help' => t('Short name of this branch, within the project'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
    ),

    'title' => array(
      'title' => t('Branch title'),
      'help' => t('Displayed name of this branch, including the project'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
    ),

    'core_compatibility' => array(
      'title' => t('Core compatibility'),
      'help' => t('Which core version this branch is compatible with'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
    ),

    'project' => array(
      'title' => t('Project'),
      'help' => t('Short name of the project'),
      'relationship' => array(
        'base' => 'api_project',
        'handler' => 'views_handler_relationship',
        'label' => t('Project'),
        'title' => t('Project'),
        'help' => t('The project this branch is part of'),
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
    ),

    'branch_ops' => array(
      'title' => t('Branch operations'),
      'help' => t('Administrative operations for branches'),
      'field' => array(
        'handler' => 'api_views_handler_field_api_branch_ops',
      ),
    ),
  );

  $data['api_project'] = array(
    'table' => array(
      'group' => t('API project'),
      'base' => array(
        'field' => 'project_name',
        'title' => t('API project'),
        'help' => t('Projects from the API module'),
        'weight' => 22,
      ),
    ),

    'project_name' => array(
      'title' => t('Project name'),
      'help' => t('Short name of the project'),
      'relationship' => array(
        'base' => 'api_branch',
        'handler' => 'views_handler_relationship',
        'label' => t('Documentation branches'),
        'title' => t('All documentation branches'),
        'help' => t('All documentation branches in this project. This will create duplicate records, so you probably want a documentation or branch view instead.'),
      ),
      'field' => array(
        'handler' => 'api_views_handler_field_api_project',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'api_views_handler_filter_project_name',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
    ),

    'project_title' => array(
      'title' => t('Project title'),
      'help' => t('Long, human-readable name of the project'),
      'field' => array(
        'handler' => 'api_views_handler_field_api_project',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
    ),

    'project_type' => array(
      'title' => t('Project type'),
      'help' => t('Type of project: module, theme, core, etc.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_in_operator',
        'options callback' => 'api_list_project_type_options',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
    ),

    'project_ops' => array(
      'title' => t('Project operations'),
      'help' => t('Administrative operations for projects'),
      'field' => array(
        'handler' => 'api_views_handler_field_api_project_ops',
      ),
    ),
  );

  $data['api_namespace'] = array(
    'table' => array(
      'group' => t('API documentation'),
      'base' => array(
        'field' => 'did',
        'title' => t('API namespace'),
        'help' => t('Namespace and use information for files in the API module'),
        'weight' => 40,
      ),
    ),

    'did' => array(
      'title' => t('Documentation ID of file'),
      'help' => t('Numeric ID of file object'),
      'relationship' => array(
        'base' => 'api_documentation',
        'handler' => 'views_handler_relationship',
        'label' => t('File object for namespace'),
        'title' => t('File object for namespace'),
        'help' => t('Documentation object for the file that this namespace record pertains to'),
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
    ),

    'object_type' => array(
      'title' => t('Namespace information type'),
      'help' => t('Type of namespace information (namespace or use_alias)'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
    ),

    'class_alias' => array(
      'title' => t('Class alias'),
      'help' => t('Alias for the class in this file from a use statement. Blank for namespaces.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
    ),

    'class_name' => array(
      'title' => t('Full class or namespace'),
      'help' => t('Full name of the class for a use alias, or full namespace for a namespace'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
    ),

  );

  $data['api_members'] = array(
    'table' => array(
      'group' => t('API documentation'),
      'base' => array(
        'field' => 'class_did',
        'title' => t('API class members'),
        'help' => t('Member functions, constants, and variables of classes'),
        'weight' => 50,
      ),
    ),

    'class_did' => array(
      'title' => t('Documentation ID of class for members'),
      'help' => t('Numeric ID of the class'),
      'relationship' => array(
        'base' => 'api_documentation',
        'base field' => 'did',
        'handler' => 'views_handler_relationship',
        'label' => t('Documentation object of class'),
        'title' => t('Documentation object of class'),
        'help' => t('Documentation information of the class'),
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
    ),

    'did' => array(
      'title' => t('Documentation ID of member'),
      'help' => t('Numeric ID of the member item'),
      'relationship' => array(
        'base' => 'api_documentation',
        'base field' => 'did',
        'handler' => 'views_handler_relationship',
        'label' => t('Documentation object of member item'),
        'title' => t('Documentation object of member item'),
        'help' => t('Documentation information of member item'),
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
    ),
  );

  $data['api_overrides'] = array(
    'table' => array(
      'group' => t('API documentation'),
      'base' => array(
        'field' => 'did',
        'title' => t('API member overrides'),
        'help' => t('Class member overrides'),
        'weight' => 60,
      ),
    ),

    'did' => array(
      'title' => t('Documentation ID of member for overrides'),
      'help' => t('Numeric ID of the member for overrides'),
      'relationship' => array(
        'base' => 'api_documentation',
        'base field' => 'did',
        'handler' => 'views_handler_relationship',
        'label' => t('Documentation object of member for overrides'),
        'title' => t('Documentation object of member for overrides'),
        'help' => t('Documentation information of the member'),
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
    ),

    'overrides_did' => array(
      'title' => t('Documentation ID of overridden member'),
      'help' => t('Numeric ID of the member item being overridden'),
      'relationship' => array(
        'base' => 'api_documentation',
        'base field' => 'did',
        'handler' => 'views_handler_relationship',
        'label' => t('Documentation object of member being overridden'),
        'title' => t('Documentation object of member being overridden'),
        'help' => t('Documentation information of member being overridden'),
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
    ),
  );

  $data['api_reference_storage'] = array(
    'table' => array(
      'group' => t('API references'),
      'base' => array(
        'field' => 'from_did',
        'title' => t('API references'),
        'help' => t('References in API documentation items to various things'),
        'weight' => 60,
      ),
    ),

    'from_did' => array(
      'title' => t('Documentation ID of item where reference was found'),
      'help' => t('Numeric ID of the documentation item'),
      'relationship' => array(
        'base' => 'api_documentation',
        'base field' => 'did',
        'handler' => 'views_handler_relationship',
        'label' => t('Documentation object where reference was found'),
        'title' => t('Documentation object where reference was found'),
        'help' => t('Documentation information of the item where reference was found'),
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
    ),

    'branch_id' => array(
      'title' => t('Branch on reference'),
      'help' => t('Numeric ID of the branch on reference'),
      'relationship' => array(
        'base' => 'api_branch',
        'handler' => 'views_handler_relationship',
        'label' => t('Branch on reference'),
        'title' => t('Branch on reference'),
        'help' => t('The branch this reference is in'),
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
    ),

    'object_name' => array(
      'title' => t('Reference name'),
      'help' => t('Function name, text found, etc. for a reference'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'argument' => array(
        'handler' => 'api_views_handler_argument_partial_match',
      ),
    ),

    'related_topic' => array(
      'title' => t('Related topic dummy field'),
      'relationship' => array(
        'base' => 'api_documentation',
        'base field' => 'object_name',
        'field' => 'object_name',
        'handler' => 'views_handler_relationship',
        'join_handler' => 'api_views_join_extras',
        'extra' => array(
          array('field' => 'object_type', 'value' => 'group'),
          array('field' => 'branch_id = %alias.branch_id', 'prepend_left' => TRUE),
        ),
        'label' => t('Related topic object'),
        'title' => t('Related topic object'),
        'help' => t('If this reference is of type "group", the documentation object for the group/topic.'),
      ),
    ),

    'object_type' => array(
      'title' => t('Reference type'),
      'help' => t('Reference type: function, group, potential hook, etc.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
    ),
  );

  return $data;
}

/**
 * Implements hook_views_data_alter().
 *
 * Adds a relationship to the Comment table and the comment statistics.
 */
function api_views_data_alter(&$data) {
  $data['comment']['did'] = array(
    'title' => t('Documentation ID'),
    'help' => t('The ID of the documentation object the comment is a reply to.'),
    'relationship' => array(
      'base' => 'api_documentation',
      'base field' => 'did',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('API documentation object'),
      'title' => t('API documentation object'),
      'help' => t('The ID of the documentation object the comment is a reply to.'),
    ),
  );

  $data['node_comment_statistics']['table']['join']['api_documentation'] = array(
    'type' => 'INNER',
    'left_field' => 'did',
    'field' => 'nid',
  );
}

/**
 * Implements hook_views_plugins().
 */
function api_views_plugins() {
  $plugins = array();
  $plugins['style']['api_dl_list'] = array(
    'title' => t('HTML DL list'),
    'help' => t('Displays rows as an HTML definition list.'),
    'handler' => 'api_views_plugin_style_dl_list',
    'theme' => 'api_views_view_dl_list',
    'theme path' => drupal_get_path('module', 'api') . '/templates',
    'theme file' => 'api.theme.inc',
    'register theme' => FALSE,
    'uses row plugin' => TRUE,
    'uses row class' => TRUE,
    'uses options' => TRUE,
    'type' => 'normal',
    'path' => drupal_get_path('module', 'api') . '/views',
  );

  return $plugins;
}

/**
 * Returns a list of existing project types.
 *
 * Options callback for filtering by project type.
 */
function api_list_project_type_options() {
  return drupal_map_assoc(db_select('api_project', 'p')
    ->fields('p', array('project_type'))
    ->groupBy('p.project_type')
    ->orderBy('p.project_type')
    ->execute()
    ->fetchCol());
}

/**
 * Views join handler that does extras better.
 *
 * The class views_join is supposed to let you put extra conditions on the
 * join, but it doesn't actually work very well for conditions involving both
 * tables' fields. This class overrides so that you can just put a 'field' in
 * for the extra (eliminating the value part) and it will be treated as a
 * formula without operator or placeholder.
 */
class api_views_join_extras extends views_join {

  /**
   * Overrides views_join::build_join().
   *
   * This is exactly the same as the base class, except where indicated
   * with an API comment.
   */
  function build_join($select_query, $table, $view_query) {
    if (empty($this->definition['table formula'])) {
      $right_table = $this->table;
    }
    else {
      $right_table = $this->definition['table formula'];
    }

    if ($this->left_table) {
      $left = $view_query->get_table_info($this->left_table);
      $left_field = "$left[alias].$this->left_field";
    }
    else {
      // This can be used if left_field is a formula or something. It should be used only *very* rarely.
      $left_field = $this->left_field;
    }

    $condition = "$left_field = $table[alias].$this->field";
    $arguments = array();

    // Tack on the extra.
    if (isset($this->extra)) {
      if (is_array($this->extra)) {
        $extras = array();
        foreach ($this->extra as $info) {
          $extra = '';
          // Figure out the table name. Remember, only use aliases provided
          // if at all possible.
          $join_table = '';
          if (!array_key_exists('table', $info)) {
            $join_table = $table['alias'] . '.';
          }
          elseif (isset($info['table'])) {
            // If we're aware of a table alias for this table, use the table
            // alias instead of the table name.
            if (isset($left) && $left['table'] == $info['table']) {
              $join_table = $left['alias'] . '.';
            }
            else {
              $join_table = $info['table'] . '.';
            }
          }

          // Convert a single-valued array of values to the single-value case,
          // and transform from IN() notation to = notation

          // API: Allow value to be omitted.
          if (!isset($info['value'])) {
            if (isset($info['prepend_left']) && $info['prepend_left']) {
              $left = $view_query->get_table_info($this->left_table);
              $extras[] = "$left[alias].$info[field]";
            }
            else {
              $extras[] = "$join_table$info[field]";
            }
          }
          else {
            if (is_array($info['value']) && count($info['value']) == 1) {
              if (empty($info['operator'])) {
                $operator = '=';
              }
              else {
                $operator = $info['operator'] == 'NOT IN' ? '!=' : '=';
              }
              $info['value'] = array_shift($info['value']);
            }

            if (is_array($info['value'])) {
              // With an array of values, we need multiple placeholders and the
              // 'IN' operator is implicit.
              foreach ($info['value'] as $value) {
                $placeholder_i = ':views_join_condition_' . $select_query->nextPlaceholder();
                $arguments[$placeholder_i] = $value;
              }

              $operator = !empty($info['operator']) ? $info['operator'] : 'IN';
              $placeholder = '( ' . implode(', ', array_keys($arguments)) . ' )';
            }
            else {
              // With a single value, the '=' operator is implicit.
              $operator = !empty($info['operator']) ? $info['operator'] : '=';
              $placeholder = ':views_join_condition_' . $select_query->nextPlaceholder();
              $arguments[$placeholder] = $info['value'];
            }

            $extras[] = "$join_table$info[field] $operator $placeholder";
          }
        }
        if ($extras) {
          if (count($extras) == 1) {
            $condition .= ' AND ' . array_shift($extras);
          }
          else {
            $condition .= ' AND (' . implode(' ' . $this->extra_type . ' ', $extras) . ')';
          }
        }
      }
      elseif ($this->extra && is_string($this->extra)) {
        $condition .= " AND ($this->extra)";
      }
    }

    $select_query->addJoin($this->type, $right_table, $table['alias'], $condition, $arguments);
  }

}
