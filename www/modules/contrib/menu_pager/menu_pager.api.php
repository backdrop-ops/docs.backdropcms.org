<?php
/**
 * @file
 * Hooks provided by the Menu Pager module.
 */

/**
 * Returns a list of paths to NOT include in menu pagers.
 */
function hook_menu_pager_ignore_paths($menu_name) {
  // Don't include link to home page.
  return array('<front>');
}

/**
 * Alters list of paths not included in menu pagers.
 */
function hook_menu_pager_ignore_paths_alter(&$paths, $menu_name) {
  // Don't include link to home page.
  $paths[] = '<front>';
}
