<?php

/**
 * @file
 * Defines a handler for linkable fields for API documentation.
 */

/**
 * Handles linkable fields for API documetation.
 *
 * Allows a link to be made to the object or the file.
 */
class api_views_handler_field_api_linkable extends views_handler_field {

  /**
   * Overrides views_handler_field::init().
   *
   * Adds additional database fields that we need for formatting.
   */
  function init(&$view, &$options) {
    parent::init($view, $options);
    // Make sure we have the information we need.
    if (!empty($this->options['link_to_docs']) && $this->options['link_to_docs'] != 'none') {
      $this->additional_fields['object_name'] = array('table' => 'api_documentation', 'field' => 'object_name');
      $this->additional_fields['object_type'] = array('table' => 'api_documentation', 'field' => 'object_type');
      $this->additional_fields['file_name'] = array('table' => 'api_documentation', 'field' => 'file_name');
      $this->additional_fields['branch_id'] = array('table' => 'api_documentation', 'field' => 'branch_id');
    }
  }

  /**
   * Overrides views_handler_field::option_definition().
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['link_to_docs'] = array(
      'default' => 'none',
    );
    $options['prefix'] = array(
      'default' => '',
    );
    $options['suffix'] = array(
      'default' => '',
    );
    return $options;
  }

  /**
   * Provides the option to make the text into a link.
   */
  function options_form(&$form, &$form_state) {
    $form['link_to_docs'] = array(
      '#title' => t('Make this a documentation link'),
      '#type' => 'radios',
      '#options' => array(
        'none' => t('No link'),
        'object' => t('Link to documentation object'),
        'file' => t('Link to documentation file'),
        'file_base' => t('Link to documentation file, with only basename as link text'),
      ),
      '#default_value' => empty($this->options['link_to_docs']) ? 'none' : $this->options['link_to_docs'],
    );

    $form['prefix'] = array(
      '#title' => t('Prefix'),
      '#description' => t('If making a link, HTML/text to output before the link'),
      '#type' => 'textfield',
      '#default_value' => empty($this->options['prefix']) ? '' : $this->options['prefix'],
    );

    $form['suffix'] = array(
      '#title' => t('Suffix'),
      '#description' => t('If making a link, HTML/text to output after the link'),
      '#type' => 'textfield',
      '#default_value' => empty($this->options['suffix']) ? '' : $this->options['suffix'],
    );

    parent::options_form($form, $form_state);
  }

  /**
   * Renders the text as a link to the object or file.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($data, $values) {
    if (!empty($this->options['link_to_docs']) && $this->options['link_to_docs'] != 'none' && !empty($this->additional_fields['object_name']) && !empty($this->additional_fields['object_type']) && !empty($this->additional_fields['file_name']) && !empty($this->additional_fields['branch_id'])) {
      $prefix = (isset($this->options['prefix']) ? $this->options['prefix'] : '');
      $suffix = (isset($this->options['suffix']) ? $this->options['suffix'] : '');
      $this->options['alter']['prefix'] = $prefix;
      $this->options['alter']['suffix'] = $suffix;

      if ($data !== NULL && $data !== '') {
        $obj = (object) array();
        $obj->object_name = $this->get_value($values, 'object_name');
        $obj->object_type = $this->get_value($values, 'object_type');
        $obj->file_name = $this->get_value($values, 'file_name');
        $obj->branch_id = $this->get_value($values, 'branch_id');
        switch ($this->options['link_to_docs']) {
          case 'file':
            $this->options['alter']['make_link'] = TRUE;
            $this->options['alter']['path'] = api_url($obj, TRUE);
            break;

          case 'object':
            $this->options['alter']['make_link'] = TRUE;
            $this->options['alter']['path'] = api_url($obj, FALSE);
            break;

          case 'file_base':
            return $prefix . str_replace('/', '/<wbr>', dirname($obj->file_name)) . '/<wbr>' . l(basename($obj->file_name), api_url($obj, TRUE)) . $suffix;
        }
      }
      else {
        $this->options['alter']['make_link'] = FALSE;
      }
    }

    return $data;
  }

  /**
   * Overrides views_handler_field::render().
   */
  function render($values) {
    $value = $this->get_value($values);
    return $this->render_link($this->sanitize_value($value), $values);
  }
}
