<?php
/**
 * Admin menu callbacks and theme functions for CKEditor module.
 */

/**
 * Editor settings callback; Provide options for CKEditor module.
 */
function ckeditor_settings_form(&$form, $form_state, $editor, $format) {
  form_load_include($form_state, 'inc', 'ckeditor', 'includes/ckeditor.admin');
  $plugins = ckeditor_plugins();

  $elements['toolbar'] = array(
    '#type' => 'fieldset',
    '#title' => t('Toolbar'),
    '#parents' => array('editor_settings'),
    '#attached' => array(
      'library' => array(
        array('ckeditor', 'drupal.ckeditor.admin'),
      ),
      'js' => array(
        array('data' => array('ckeditor' => array('toolbarAdmin' => theme('ckeditor_settings_toolbar', array('editor' => $editor, 'plugins' => $plugins)))), 'type' => 'setting')
      ),
    ),
    '#attributes' => array('class' => array('ckeditor-toolbar-configuration')),
  );
  $elements['toolbar']['toolbar'] = array(
    '#type' => 'textarea',
    '#title' => t('Toolbar configuration'),
    '#default_value' => json_encode($editor->settings['toolbar']),
    '#attributes' => array('class' => array('ckeditor-toolbar-textarea')),
  );
  $elements['toolbar']['format_list'] = array(
    '#type' => 'textfield',
    '#title' => t('Format list'),
    '#default_value' => implode(', ', $editor->settings['format_list']),
    '#description' => t('A list of tags that will be provided in the "Format" dropdown, separated by commas.')
  );
  $elements['toolbar']['style_list'] = array(
    '#type' => 'textarea',
    '#title' => t('Style list'),
    '#rows' => 4,
    '#default_value' => implode("\n", $editor->settings['style_list']),
    '#description' => t('A list of classes that will be provided in the "Styles" dropdown, each on a separate line. These styles should be available in your theme\'s editor.css as well as in your theme\'s main CSS file.')
  );

  array_unshift($form['#submit'], 'ckeditor_settings_form_submit');
  return $elements;
}

/**
 * Additional submit handler for filter_admin_format_form().
 */
function ckeditor_settings_form_submit($form, &$form_state) {
  $settings = $form_state['values']['editor_settings'];
  form_set_value($form['editor_settings']['toolbar']['toolbar'], json_decode($settings['toolbar'], FALSE), $form_state);

  $format_list = array();
  foreach (explode(',', $settings['format_list']) as $format) {
    $format_list[] = trim($format);
  }
  form_set_value($form['editor_settings']['toolbar']['format_list'], $format_list, $form_state);

  $styles = array();
  foreach (explode("\n", $settings['style_list']) as $style) {
    $styles[] = trim($style);
  }
  form_set_value($form['editor_settings']['toolbar']['style_list'], $styles, $form_state);
}


/**
 * Preprocess variables for theme_ckeditor_settings_toolbar().
 */
function template_preprocess_ckeditor_settings_toolbar(&$variables) {
  // Simplify the language direction information for toolbar buttons.
  global $language;
  $variables['language_direction'] = isset($language->direction) && $language->direction === LANGUAGE_RTL ? 'rtl' : 'ltr';

  // Create lists of active and disabled buttons.
  $editor = $variables['editor'];
  $plugins = $variables['plugins'];
  $buttons = array();
  $variables['multiple_buttons'] = array();
  foreach ($plugins as $plugin) {
    if (isset($plugin['buttons'])) {
      foreach ($plugin['buttons'] as $button_name => $button) {
        if (!empty($button['multiple'])) {
          $variables['multiple_buttons'][$button_name] = $button;
        }
        $button['name'] = $button_name;
        $buttons[$button_name] = $button;
      }
    }
  }
  $variables['active_buttons'] = array();
  foreach ($editor->settings['toolbar'] as $row_number => $row) {
    foreach ($row as $button_name) {
      if (isset($buttons[$button_name])) {
        $variables['active_buttons'][$row_number][] = $buttons[$button_name];
        if (empty($buttons[$button_name]['multiple'])) {
          unset($buttons[$button_name]);
        }
      }
    }
  }
  $variables['disabled_buttons'] = array_diff_key($buttons, $variables['multiple_buttons']);
}

/**
 * Displays the toolbar configuration for CKEditor.
 */
function theme_ckeditor_settings_toolbar($variables) {
  $editor = $variables['editor'];
  $plugins = $variables['plugins'];
  $rtl = $variables['language_direction'] === 'rtl' ? '_rtl' : '';

  // Assemble items to be added to active button rows.
  foreach ($variables['active_buttons'] as $row_number => $row_buttons) {
    foreach ($row_buttons as $button) {
      $button_name = $button['name'];
      if (isset($button['image_alternative'])) {
        $data = $button['image_alternative'];
      }
      elseif (isset($button['image'])) {
        $data = theme('image', array('path' => $button['image' . $rtl], 'title' => $button['label']));
      }
      else {
        $data = '?';
      }
      $button_item = array(
        'data' => $data,
        'data-button-name' => $button_name,
      );
      if (!empty($button['multiple'])) {
        $button['attributes']['class'][] = 'ckeditor-multiple-button';
      }
      if (!empty($button['attributes'])) {
        $button_item = array_merge($button_item, $button['attributes']);
      }
      $active_buttons[$row_number][] = $button_item;
    }
  }
  // Assemble list of disabled buttons (which are always a single row).
  foreach ($variables['disabled_buttons'] as $button_name => $button) {
    if (isset($button['image_alternative'])) {
      $data = $button['image_alternative'];
    }
    elseif (isset($button['image'])) {
      $data = theme('image', array('path' => $button['image' . $rtl], 'title' => $button['label']));
    }
    else {
      $data = '?';
    }
    $button_item = array(
      'data' => $data,
      'data-button-name' => $button_name,
    );
    if (isset($button['attributes'])) {
      $button_item = array_merge($button_item, $button['attributes']);
    }
    $disabled_buttons[] = $button_item;
  }
  // Assemble list of multiple buttons that may be added multiple times.
  foreach ($variables['multiple_buttons'] as $button_name => $button) {
    if (isset($button['image_alternative'])) {
      $data = $button['image_alternative'];
    }
    elseif (isset($button['image'])) {
      $data = theme('image', array('path' => $button['image' . $rtl], 'title' => $button['label']));
    }
    else {
      $data = '?';
    }
    $button_item = array(
      'data' => $data,
      'data-button-name' => $button_name,
    );
    $button['attributes']['class'][] = 'ckeditor-multiple-button';
    if (isset($button['attributes'])) {
      $button_item = array_merge($button_item, $button['attributes']);
    }
    $multiple_buttons[] = $button_item;
  }

  // We don't use theme_item_list() below in case there are no buttons in the
  // active or disabled list, as theme_item_list() will not print an empty UL.
  $output = '';
  $output .= '<strong>' . t('Active toolbar') . '</strong>';
  $output .= '<div class="ckeditor-toolbar-active clearfix">';
  foreach ($active_buttons as $button_row) {
    $output .= '<ul class="ckeditor-buttons">';
    foreach ($button_row as $button) {
      $contents = $button['data'];
      unset($button['data']);
      $attributes = drupal_attributes($button);
      $output .= '<li' . $attributes . '>' . $contents . '</li>';
    }
    $output .= '</ul>';
  }
  if (empty($active_buttons)) {
    $output .= '<ul class="ckeditor-buttons">';
    $output .= '</ul>';
  }

  $output .= '<div class="ckeditor-row-controls">';
  $output .= '<a href="#" class="ckeditor-row-remove" title="' . t('Remove row') . '">-</a>';
  $output .= '<a href="#" class="ckeditor-row-add" title="' . t('Add row') . '">+</a>';
  $output .= '</div>';

  $output .= '</div>';

  $output .= '<strong>' . t('Available buttons') . '</strong>';
  $output .= '<div class="ckeditor-toolbar-disabled clearfix">';
  $output .= '<ul class="ckeditor-buttons">';
  foreach ($disabled_buttons as $button) {
    $contents = $button['data'];
    unset($button['data']);
    $attributes = drupal_attributes($button);
    $output .= '<li' . $attributes . '>' . $contents . '</li>';
  }
  $output .= '</ul>';
  $output .= '<strong class="ckeditor-multiple-label">' . t('Dividers') . ': </strong>';
  $output .= '<ul class="ckeditor-multiple-buttons">';
  foreach ($multiple_buttons as $button) {
    $contents = $button['data'];
    unset($button['data']);
    $attributes = drupal_attributes($button);
    $output .= '<li' . $attributes . '>' . $contents . '</li>';
  }
  $output .= '</ul>';
  $output .= '</div>';

  return $output;
}
