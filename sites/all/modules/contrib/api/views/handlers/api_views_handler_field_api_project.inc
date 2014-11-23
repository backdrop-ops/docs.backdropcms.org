<?php

/**
 * @file
 * Defines a handler for the project fields for API documentation.
 */

/**
 * Handles the project fields for API documetation.
 *
 * Allows a link to be made to the project.
 */
class api_views_handler_field_api_project extends views_handler_field {

  /**
   * Overrides views_handler_field::init().
   *
   * Adds additional database fields that we need for formatting.
   */
  function init(&$view, &$options) {
    parent::init($view, $options);
    // Make sure we have the information we need.
    if (!empty($this->options['link_to_project']) && $this->options['link_to_project'] != 'none') {
      $this->additional_fields['project_name'] = array('table' => 'api_project', 'field' => 'project_name');
    }
  }

  /**
   * Overrides views_handler_field::option_definition().
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['link_to_project'] = array(
      'default' => 'none',
    );
    return $options;
  }

  /**
   * Provides the option to make the text into a link.
   */
  function options_form(&$form, &$form_state) {
    $form['link_to_project'] = array(
      '#title' => t('Make this a link'),
      '#type' => 'radios',
      '#options' => array(
        'none' => t('No link'),
        'project' => t('Link to project'),
      ),
      '#default_value' => empty($this->options['link_to_project']) ? 'none' : $this->options['link_to_project'],
    );

    parent::options_form($form, $form_state);
  }

  /**
   * Renders the text as a link to the project.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($data, $values) {
    if (!empty($this->options['link_to_project']) && $this->options['link_to_project'] != 'none' && !empty($this->additional_fields['project_name'])) {
      if ($data !== NULL && $data !== '') {
        $project_name = $this->get_value($values, 'project_name');
        switch ($this->options['link_to_project']) {
          case 'project':
            $this->options['alter']['make_link'] = TRUE;
            $this->options['alter']['path'] = url('api/' . $project_name, array('absolute' => TRUE));
            break;
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
