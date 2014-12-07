<?php

/**
 * @file
 * Editor integration functions for CKEditor.
 */

/**
 * Plugin implementation of hook_editor().
 */
function ckeditor_ckeditor_extended_editor() {
  $editor['ckeditor_extended'] = array(
    'title' => 'CKEditor (extended integration)',
    'vendor url' => 'http://ckeditor.com',
    'download url' => 'http://ckeditor.com/download',
    'libraries' => array(
      '' => array(
        'title' => 'Default',
        'files' => array(
          'ckeditor.js' => array('preprocess' => FALSE),
        ),
      ),
    ),
    'editor path' => drupal_get_path('module', 'ckeditor') . '/lib/ckeditor',
    'library path' => drupal_get_path('module', 'ckeditor') . '/lib/ckeditor',
    'js path' => drupal_get_path('module', 'ckeditor') . '/js',
    'css path' => drupal_get_path('module', 'ckeditor') . '/css',
    'version callback' => 'ckeditor_wysiwyg_version',
    'settings callback' => 'ckeditor_wysiwyg_settings',
    'plugin callback' => 'ckeditor_wysiwyg_plugins',
    'plugin settings callback' => 'ckeditor_wysiwyg_plugin_settings',
    'proxy plugin' => array(
      'drupal' => array(
        'load' => TRUE,
        'proxy' => TRUE,
      ),
    ),
    'proxy plugin settings callback' => 'ckeditor_wysiwyg_proxy_plugin_settings',
    'versions' => array(
      '4.0' => array(
        'js files' => array('ckeditor_extended.js'),
      ),
    ),
  );
  return $editor;
}

/**
 * Detect editor version.
 *
 * @param $editor
 *   An array containing editor properties as returned from hook_editor().
 *
 * @return
 *   The installed editor version.
 */
function ckeditor_wysiwyg_version($editor) {
  return '4.0';
}

/**
 * Return JavaScript settings that should be passed to the WYSIWYG editor.
 */
function ckeditor_wysiwyg_settings($editor, $config, $theme) {
  // Merge in defaults.
  $ckeditor_editor_info = ckeditor_editor_info();
  $config = (array) $config + $ckeditor_editor_info['ckeditor']['default settings'];

  // Modify parameters to match Drupal 8 'js settings callback' as closely as
  // possible. WYSIWYG doesn't pass us the format information.
  $editor = (object) $editor;
  $editor->settings = $config;
  $format = (object) array();
  $existing_settings = array();
  return ckeditor_add_settings($editor, $format, $existing_settings);
}

/**
 * Build a JS settings array of native external plugins that need to be loaded separately.
 */
function ckeditor_wysiwyg_plugin_settings($editor, $profile, $plugins) {
  $settings = array();

  // Pull the list of required plugins from the CKEditor JS settings.
  // TODO: Abstract the list of enabled plugins so we can get at this
  // information more easily?
  $editor_js_settings = ckeditor_wysiwyg_settings($editor, $profile->settings, NULL);
  $plugins = explode(',', $editor_js_settings['extraPlugins']);
  $plugin_info = ckeditor_plugins();

  foreach ($plugins as $name) {
    // Skip plugins that are no longer available.
    if (!isset($plugin_info[$name])) {
      continue;
    }

    $plugin = $plugin_info[$name];
    // Register all plugins that need to be loaded.
    $settings[$name] = array();
    // Add path for native external plugins.
    if (empty($plugin['internal']) && isset($plugin['path'])) {
      $settings[$name]['path'] = base_path() . $plugin['path'] . '/';
    }
    // CKEditor defaults to 'plugin.js' on its own when filename is not set.
    if (!empty($plugin['file'])) {
      $settings[$name]['fileName'] = $plugin['file'];
    }
  }

  return $settings;
}

/**
 * Build a JS settings array for Drupal plugins loaded via the proxy plugin.
 */
function ckeditor_wysiwyg_proxy_plugin_settings($editor, $profile, $plugins) {
  $settings = array();
  foreach ($plugins as $name => $plugin) {
    // Populate required plugin settings.
    $settings[$name] = $plugin['dialog settings'] + array(
      'title' => $plugin['title'],
      'icon' => base_path() . $plugin['icon path'] . '/' . $plugin['icon file'],
      'iconTitle' => $plugin['icon title'],
      // @todo These should only be set if the plugin defined them.
      'css' => base_path() . $plugin['css path'] . '/' . $plugin['css file'],
    );
  }
  return $settings;
}

/**
 * Return internal plugins for this editor; semi-implementation of hook_wysiwyg_plugin().
 */
function ckeditor_wysiwyg_plugins($editor) {
  // Convert the more extensive CKEditor plugin definitions to WYSIWYG API's
  // simpler form.
  $ckeditor_plugins = ckeditor_plugins();
  $plugins = array();
  foreach ($ckeditor_plugins as $plugin_name => $plugin) {
    if (isset($plugin['buttons'])) {
      foreach ($plugin['buttons'] as $button_name => $button) {
        $plugin['buttons'][$button_name] = $button['label'];
      }
    }
    $plugins[$plugin_name] = $plugin;
  }
  return $plugins;
}

