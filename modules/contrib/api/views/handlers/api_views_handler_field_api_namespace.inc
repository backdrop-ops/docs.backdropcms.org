<?php

/**
 * @file
 * Defines a handler for the namespace field for API documentation.
 */

/**
 * Handles namespace field for API documetation.
 *
 * Allows a link to be made to the namespace page.
 */
class api_views_handler_field_api_namespace extends views_handler_field {

  /**
   * Overrides views_handler_field::init().
   *
   * Adds additional database fields that we need for formatting.
   */
  function init(&$view, &$options) {
    parent::init($view, $options);
    // Make sure we have the information we need.
    if (!empty($this->options['link_to_namespace']) && $this->options['link_to_namespace'] != 'none') {
      $this->additional_fields['branch_id'] = array('table' => 'api_documentation', 'field' => 'branch_id');
      $this->additional_fields['namespace'] = array('table' => 'api_documentation', 'field' => 'namespace');
    }
  }

  /**
   * Overrides views_handler_field::option_definition().
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['link_to_namespace'] = array(
      'default' => 'none',
    );
    return $options;
  }

  /**
   * Provides the option to make the text into a link.
   */
  function options_form(&$form, &$form_state) {
    $form['link_to_namespace'] = array(
      '#title' => t('Make this a link'),
      '#type' => 'radios',
      '#options' => array(
        'none' => t('No link'),
        'object' => t('Link to namespace page'),
      ),
      '#default_value' => empty($this->options['link_to_namespace']) ? 'none' : $this->options['link_to_namespace'],
    );

    parent::options_form($form, $form_state);
  }

  /**
   * Renders the text as a link to the object or file.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($data, $values) {
    if (!empty($this->options['link_to_namespace']) && $this->options['link_to_namespace'] != 'none' && !empty($this->additional_fields['branch_id'])) {
      if ($data !== NULL && $data !== '') {
        $branch_id = $this->get_value($values, 'branch_id');
        $namespace = $this->get_value($values, 'namespace');
        $this->options['alter']['make_link'] = TRUE;
        $this->options['alter']['path'] = api_namespace_url($branch_id, $namespace);
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
