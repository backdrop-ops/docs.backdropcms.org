<?php
/**
 * @file
 * drupalapi_core.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function drupalapi_core_user_default_permissions() {
  $permissions = array();

  // Exported permission: 'access API reference'.
  $permissions['access API reference'] = array(
    'name' => 'access API reference',
    'roles' => array(
      'anonymous user' => 'anonymous user',
    ),
    'module' => 'api',
  );

  return $permissions;
}
