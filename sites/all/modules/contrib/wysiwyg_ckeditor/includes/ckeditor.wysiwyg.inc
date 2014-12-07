<?php
/**
 * @file
 * Provides all integration hooks with WYSIWYG module.
 *
 * This file is always included on all requests, as it contains several hooks.
 * It is kept separate from the main module file to maintain compatibility
 * with the Drupal 8 version, which does not require any of these hooks.
 */

/**
 * Implements hook_wysiwyg_include_directory().
 */
function ckeditor_wysiwyg_include_directory($plugin_type) {
  if ($plugin_type === 'editors') {
    return 'includes';
  }
}

/**
 * Implements hook_module_implements_alter().
 */
function ckeditor_module_implements_alter(&$implementations, $hook) {
  // WYSIWYG module has a namespace conflict with hook_ckeditor_plugins().
  // This prevents wysiwyg_ckeditor_plugins() from being an implementation of
  // hook_ckeditor_plugins().
  if ($hook === 'ckeditor_plugins') {
    if (isset($implementations['wysiwyg'])) {
      unset($implementations['wysiwyg']);
    }
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function ckeditor_form_wysiwyg_profile_form_alter(&$form, $form_state) {
  $profile = $form_state['build_info']['args'][0];
  $format = filter_format_load($profile->format);

  // Set default settings for the profile.
  $ckeditor_editor_info = ckeditor_editor_info();
  $profile->settings = (array) $profile->settings + $ckeditor_editor_info['ckeditor']['default settings'];

  // Hide the basic settings, as they affect behavior of the WYSIWYG module.
  $form['basic']['#access'] = FALSE;

  // We don't need the toggle link as you can just use the source button.
  $form['basic']['default']['#default_value'] = TRUE;
  $form['basic']['show_toggle']['#default_value'] = FALSE;

  // Remove all the settings we either don't use or replace.
  unset($form['buttons'], $form['appearance'], $form['output'], $form['css']);

  // Block formats need to exist to prevent notices in WYSIWYG module.
  $form['block_formats'] = array(
    '#type' => 'value',
    '#value' => '',
  );

  // Add an additional submit handler to clean up our modifications.
  // This submit handler must be added before ckeditor_settings_form() is called
  // below, as it adds another submit handler before it.
  array_unshift($form['#submit'], 'ckeditor_wysiwyg_settings_form_submit');

  // Add the CKEditor specialized form. We place these in "editor_settings" to
  // match the Drupal 8 editor.module form structure, which means more
  // compatibility in ckeditor_settings_form_submit().
  module_load_include('inc', 'ckeditor', 'includes/ckeditor.admin');
  $form['editor_settings'] = ckeditor_settings_form($form, $form_state, $profile, $format);
  $form['editor_settings']['#tree'] = TRUE;
}

/**
 * Additional submit handler for the wysiwyg_profile_form().
 */
function ckeditor_wysiwyg_settings_form_submit($form, &$form_state) {
  // Move the settings back out of the editor_settings parents so they save.
  $form_state['values'] = array_merge($form_state['values'], $form_state['values']['editor_settings']);
  unset($form_state['values']['editor_settings']);
}
