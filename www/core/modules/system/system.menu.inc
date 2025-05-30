<?php

/**
 * @file
 * Menu block configuration form and display.
 */

/**
 * Implements hook_block_configure().
 */
function _system_block_configure($delta = '', $settings = array()) {
  // Merge in the default configuration.
  $settings += system_menu_block_defaults($delta);
  return system_menu_block_form($settings);
}

/**
 * The default menu block configuration.
 *
 * @return array
 */
function system_menu_block_defaults($menu_name) {
  $defaults = array(
    'level' => 1,
    'menu_name' => $menu_name,
    'style' => NULL,
    'expand_all' => FALSE,
    'clickdown' => FALSE,
    'collapse' => 'default',
    'toggle_text' => 'Menu',
  );
  if ($menu_name == 'main-menu') {
    $defaults['depth'] = 1;
    $defaults['toggle'] = TRUE;
  }
  else {
    $defaults['depth'] = 0;
    $defaults['toggle'] = FALSE;
  }
  return $defaults;
}

/**
 * Returns the configuration form for a menu tree.
 *
 * @param $config
 *   An associated array of settings passed in by hook_block_configure().
 *   If none are given, default configuration is assumed.
 * @return
 *   The form array. This is a form fragment and not in full Form API format.
 */
function system_menu_block_form($config) {
  $form['menu_name'] = array(
    '#type' => 'value',
    '#value' => $config['menu_name'],
  );

  $form['style'] = array(
    '#type' => 'select',
    '#title' => t('Menu style'),
    '#options' => array(
      'top_only' => t('Top level only'),
      'dropdown' => t('Dropdown menu'),
      'tree' => t('Hierarchical tree'),
    ),
    '#default_value' => empty($config['style']) ? 'tree' : $config['style'],
  );

  $form['level'] = array(
    '#type' => 'select',
    '#title' => t('Starting level'),
    '#default_value' => $config['level'],
    '#options' => array(
      '1' => t('1st level (primary)'),
      '2' => t('2nd level (secondary)'),
      '3' => t('3rd level (tertiary)'),
      '4' => t('4th level'),
      '5' => t('5th level'),
      '6' => t('6th level'),
      '7' => t('7th level'),
      '8' => t('8th level'),
      '9' => t('9th level'),
    ),
    '#description' => t('The block will be visible only if the current page has its menu item at or below the level set here.'),
    '#states' => array(
      'visible' => array(
        'select[name="block_settings[style]"]' => array('value' => 'tree'),
      ),
    ),
  );
  $form['depth'] = array(
    '#type' => 'select',
    '#title' => t('Maximum depth'),
    '#default_value' => $config['depth'],
    '#options' => array(
      '1' => '1',
      '2' => '2',
      '3' => '3',
      '4' => '4',
      '5' => '5',
      '6' => '6',
      '7' => '7',
      '8' => '8',
      '9' => '9',
      '0' => t('Unlimited'),
    ),
    '#description' => t('The maximum depth of the menu tree from the starting level (if available).'),
    '#states' => array(
      'invisible' => array(
        'select[name="block_settings[style]"]' => array('value' => 'top_only'),
      ),
    ),
  );
  $form['expand_all'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show all menu links'),
    '#default_value' => $config['expand_all'],
    '#description' => t('When unchecked, only expanded or active trail menu links will be displayed.'),
    '#states' => array(
      'visible' => array(
        'select[name="block_settings[style]"]' => array('value' => 'tree'),
      ),
    ),
  );
  $form['clickdown'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use a click to open, instead of a hover'),
    '#default_value' => $config['clickdown'],
    '#description' => t('Menus that don\'t appear or disappear on hover can be better for usability in some cases.'),
    '#states' => array(
      'visible' => array(
        'select[name="block_settings[style]"]' => array('value' => 'dropdown'),
      ),
    ),
  );
  $link = t('menu settings');
  if (user_access('administer menu')) {
    $link = l($link, 'admin/structure/menu/settings', array(
      'attributes' => array('target' => '_blank'),
    ));
  }
  $form['toggle'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display menu toggle button on small screens'),
    '#default_value' => $config['toggle'],
    '#description' => t('On devices with small screens, this option reduces the menu to a toggle button, commonly known as <a href="https://en.wikipedia.org/wiki/Hamburger_button" target="_blank"><em>hamburger button</em></a>. Clicking the icon reveals or hides the menu, saving screen space. The breakpoint for small screens can be configured in the global !menu_settings.', array(
      '!menu_settings' => $link,
    )),
  );
  $toggle_text_default = system_menu_block_defaults($config['menu_name'])['toggle_text'];
  $form['toggle_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Toggle button text'),
    '#default_value' => $config['toggle_text'],
    '#description' => t('Text to display beside the toggle button. Default: %toggle_text.', array(
      '%toggle_text' => $toggle_text_default,
    )),
    '#indentation' => 1,
    '#states' => array(
      'visible' => array(
        'input[name="block_settings[toggle]"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['collapse'] = array(
    '#type' => 'radios',
    '#title' => t('Collapsible behavior'),
    '#default_value' => (!empty($config['collapse'])) ? $config['collapse'] : 'default',
    '#options' => array(
      'default' => t('Default'),
      'toggle' => t('Toggle'),
      'link' => t('Link'),
    ),
    '#description' => t('Determines how parent menu items behave when the menu is shown on small screens.'),
    '#states' => array(
      'visible' => array(
        'select[name="block_settings[style]"]' => array('value' => 'dropdown'),
      ),
    ),
  );
  $form['collapse']['default']['#description'] = t('Parent expands on first tap, acts as a link on second tap.');
  $form['collapse']['toggle']['#description'] = t('Parent expands or collapses on each tap and does not act as a link.');
  $form['collapse']['link']['#description'] = t('Parent text acts as a link and the +/- element expands or collapses.');

  $form['accordion'] = array('#type' => 'checkbox',
    '#title' => t('Accordion-style'),
    '#default_value' => (!empty($config['accordion'])) ? $config['accordion'] : FALSE,
    '#description' => t('Close all sub-menus when closing parent menu items.'),
    '#states' => array(
      'visible' => array(
        'select[name="block_settings[style]"]' => array('value' => 'dropdown'),
      ),
    ),
  );

  return $form;
}

/**
 * Build a menu tree based on the provided configuration.
 *
 * @param array $config
 *   An array of configuration options that specifies how to build the
 *   menu tree and its title.
 *   - menu_name: (string) The machine name of the requested menu. Can also be
 *     set to MENU_TREE__CURRENT_PAGE_MENU to use the menu selected by the page.
 *   - style: (string) The menu style used to render this menu.
 *   - level: (int) The starting level of the tree. Some styles may ignore this
 *     setting.
 *   - depth: (int) The maximum depth the tree should contain, relative to the
 *     starting level. Some styles may ignore this setting.
 *
 * @return array
 *   An associative array containing several pieces of data.
 *   - content: The tree as a renderable array.
 *   - subject: The title rendered as HTML.
 *   - subject_array: The title as a renderable array.
 */
function system_menu_block_build(array $config) {
  if (module_exists('menu')) {
    $menu_names = menu_get_menus();
  }
  else {
    $menu_names = menu_list_system_menus();
  }

  // Set defaults.
  $menu_name = str_replace('_', '-', $config['menu_name']);
  $config += system_menu_block_defaults($menu_name);
  if (empty($config['style'])) {
    $config['style'] = 'tree';
  }

  // Always expand dropdown style output.
  if ($config['style'] === 'dropdown') {
    $config['expand_all'] = TRUE;
  }

  // Get the default block name.
  backdrop_static_reset('menu_block_set_title');
  system_menu_block_set_title($menu_names[$menu_name]);

  // Get the raw menu tree data.
  $tree = system_menu_tree_block_data($config);
  $title = system_menu_block_get_title();

  $data = array();
  $data['subject'] = $title;
  $data['content'] = array();
  // Create a renderable tree.
  if (!empty($tree) && $output = menu_tree_output($tree)) {
    $data['content'] = $output;
    // Add any menu style (currently always "dropdown" if any).
    if (!empty($config['style'])) {
      $data['content']['#wrapper_attributes']['class'][] = 'menu-' . str_replace('_', '-', $config['style']);
      $data['content']['#wrapper_attributes']['data-menu-style'] = $config['style'];
      $data['content']['#wrapper_attributes']['data-clickdown'] = $config['clickdown'];
      if (!empty($config['accordion'])) {
        $collapsible_behavior = (empty($config['collapse']) || $config['collapse'] == 'default') ? 'accordion' : 'accordion-' . $config['collapse'];
      }
      else {
        $collapsible_behavior = (empty($config['collapse'])) ? 'default' : $config['collapse'];
      }
      $data['content']['#wrapper_attributes']['data-collapse'] = $collapsible_behavior;
      $data['content']['#attached']['library'][] = array(
        'system',
        'backdrop.menus',
      );
      if ($config['style'] === 'dropdown') {
        $data['content']['#attached']['library'][] = array(
          'system',
          'smartmenus',
        );
      }
    }
    if (!empty($config['toggle']) && $config['toggle'] == TRUE) {
      $id = backdrop_html_id('menu-toggle-state');
      $data['content']['#wrapper_attributes']['data-menu-toggle-id'] = $id;
      $data['content']['#prefix'] = theme('menu_toggle', array(
        'enabled' => $config['toggle'],
        'id' => $id,
        // Allow the user-entered menu toggle text to be translated.
        'text' => t(trim($config['toggle_text'])),
      ));
      $data['content']['#attached']['library'][] = array(
        'system',
        'backdrop.menu-toggle',
      );
    }
    if (!empty($config['style_settings'])) {
      $data['content']['#wrapper_attributes']['data-menu-style'] = backdrop_json_encode($config['style_settings']);
    }
  }

  return $data;
}

/**
 * Gets the data structure representing a menu tree for the given configuration.
 *
 * @param array $config
 *   See the $config param of menu_tree_build().
 *
 * @return array
 */
function system_menu_tree_block_data(array &$config) {
  // @todo: Anything that makes adjustments based on style should come from
  // a menu style plugin.
  // See https://github.com/backdrop/backdrop-issues/issues/1457
  if ($config['style'] === 'top_only') {
    $config['level'] = 1;
    $config['depth'] = 1;
  }
  elseif ($config['style'] === 'dropdown') {
    $config['level'] = 1;
  }

  // Determine the max depth based on level and depth setting.
  $max_depth = ($config['depth'] == 0) ? NULL : min($config['level'] + $config['depth'] - 1, MENU_MAX_DEPTH);

  // Set the expand all flag if the style requires it.
  $expand_all = FALSE;
  if ($config['depth'] != 1) {
    if (isset($config['expand_all'])) {
      $expand_all = (bool) $config['expand_all'];
    }
    // Right now we assume if any style is set, the whole menu should be expanded.
    elseif ($config['style'] !== 'tree') {
      $expand_all = TRUE;
    }
  }

  // Get the menu tree based on the configuration options using the central
  // menu system cache.
  $tree = menu_tree_page_data($config['menu_name'], $max_depth, FALSE, $expand_all);

  // Allow alteration of the tree and config before we begin operations on it.
  backdrop_alter('menu_block_tree', $tree, $config);

  // Prune the tree along the active trail to the specified level.
  if ($config['level'] > 1) {
    system_menu_tree_prune_tree($tree, $config['level']);
  }

  // Trim the tree to the active path.
  if (!$expand_all) {
    system_menu_tree_trim_active_path($tree);
  }

  // Trim the branches that extend beyond the specified depth.
  if ($config['depth'] > 0) {
    system_menu_tree_depth_trim($tree, $config['depth']);
  }

  return $tree;
}

/**
 * Prune a tree so that it begins at the specified level.
 *
 * This function will follow the active menu trail to the specified level.
 *
 * @param array $tree
 *   The menu tree to prune.
 * @param int $level
 *   The level of the original tree that will start the pruned tree.
 * @param array|null $parent_item
 *   The menu item that should be used as the root of the tree.
 */
function system_menu_tree_prune_tree(array &$tree, $level, ?array $parent_item = NULL) {
  if (!empty($parent_item)) {
    // Prune the tree along the path to the menu item.
    for ($i = 1; $i <= MENU_MAX_DEPTH && $parent_item["p$i"] != '0'; $i++) {
      $plid = $parent_item["p$i"];
      $found_active_trail = FALSE;
      // Examine each element at this level for the ancestor.
      foreach ($tree as $key => &$value) {
        if ($tree[$key]['link']['mlid'] == $plid) {
          system_menu_block_set_title($tree[$key]['link']);
          // Prune the tree to the children of this ancestor.
          $tree = $tree[$key]['below'] ? $tree[$key]['below'] : array();
          $found_active_trail = TRUE;
          break;
        }
      }
      // If we don't find the ancestor, bail out.
      if (!$found_active_trail) {
        $tree = array();
        break;
      }
    }
  }

  $is_front_page = backdrop_is_front_page();
  // Trim the upper levels down to the one desired.
  for ($i = 1; $i < $level; $i++) {
    $found_active_trail = FALSE;
    // Examine each element at this level for the active trail.
    foreach ($tree as $key => &$value) {
      // Also include the children of the home page.
      if ($tree[$key]['link']['in_active_trail'] || ($tree[$key]['link']['link_path'] == '<front>' && $is_front_page)) {
        // Get the title for the pruned tree.
        system_menu_block_set_title($tree[$key]['link']);
        // Prune the tree to the children of the item in the active trail.
        $tree = $tree[$key]['below'] ? $tree[$key]['below'] : array();
        $found_active_trail = TRUE;
        break;
      }
    }
    // If we don't find the active trail, the active item isn't in the tree we want.
    if (!$found_active_trail) {
      $tree = array();
      break;
    }
  }
}

/**
 * Trim everything but the active trail in the tree.
 *
 * @param array $tree
 *   The menu tree to trim.
 */
function system_menu_tree_trim_active_path(array &$tree) {
  foreach ($tree as $key => &$value) {
    if (($tree[$key]['link']['in_active_trail'] || $tree[$key]['link']['expanded']) && $tree[$key]['below']) {
      // Continue in the subtree, if it exists.
      system_menu_tree_trim_active_path($tree[$key]['below']);
    }
    else {
      // Trim anything not expanded or along the active trail.
      $tree[$key]['below'] = FALSE;
    }
  }
}

/**
 * Prune a tree so it does not extend beyond the specified depth limit.
 *
 * @param array $tree
 *   The menu tree to prune.
 * @param int $depth_limit
 *   The maximum depth of the returned tree; must be a positive integer.
 */
function system_menu_tree_depth_trim(array &$tree, $depth_limit) {
  // Prevent invalid input from returning a trimmed tree.
  if ($depth_limit < 1) {
    return;
  }

  // Examine each element at this level to find any possible children.
  foreach ($tree as $key => &$value) {
    if ($tree[$key]['below']) {
      if ($depth_limit > 1) {
        system_menu_tree_depth_trim($tree[$key]['below'], $depth_limit - 1);
      }
      else {
        // Remove the children items.
        $tree[$key]['below'] = FALSE;
      }
    }
  }
}


/**
 * Sets the menu item to use for the tree's title.
 *
 * @param array|string $item
 *   The menu item (an array) or the menu item's title as a string.
 *
 * @return array|string
 *   The saved title value.
 */
function system_menu_block_set_title($item = NULL) {
  $menu_item = &backdrop_static(__FUNCTION__, '');
  // Save the menu item.
  if (!is_null($item)) {
    $menu_item = $item;
  }

  return $menu_item;
}

/**
 * Retrieves the menu item to use for the tree's title.
 *
 * @return array|string
 *   A render array or string containing the tree's title.
 */
function system_menu_block_get_title() {
  $menu_item = system_menu_block_set_title();

  if (is_string($menu_item)) {
    // The tree's title is a menu title, a normal string.
    $title = check_plain($menu_item);
  }
  // The tree's title is a menu item.
  else {
    $title = check_plain($menu_item['title']);
  }
  return $title;
}
