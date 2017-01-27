<?php

/**
 * @file
 * Defines a handler for a string argument allowing partial matching.
 */

/**
 * Handles an argument that allows partial string matching.
 */
class api_views_handler_argument_partial_match extends views_handler_argument_string {

  function init(&$view, &$options) {
    parent::init($view, $options);
  }

  function option_definition() {
    $options = parent::option_definition();

    $options['partial_match'] = array('default' => FALSE, 'bool' => TRUE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['partial_match'] = array(
      '#type' => 'checkbox',
      '#title' => t('Partial match'),
      '#description' => t('If checked, the argument is a partial match for the field value. If not checked, an exact match is required. Some other options will be ignored if partial matchin is selected, such as many-to-one, glossary mode, and/or handling, etc.'),
      '#default_value' => $this->options['partial_match'],
    );
  }

  /**
   * Build the query.
   */
  function query($group_by = FALSE) {
    $argument = $this->argument;

    if (!empty($this->options['transform_dash'])) {
      $argument = strtr($argument, '-', ' ');
    }

    if (!empty($this->options['partial_match'])) {
      // Simple partial matching has been chosen.
      $argument = '%' . str_replace(array('_', '%'), array('\_', '\%'), $argument) . '%';
      $operator = 'LIKE';
      $this->ensure_my_table();
      $field = "$this->table_alias.$this->real_field";
      $this->query->add_where(0, $field, $argument, $operator);
      return;
    }

    if (!empty($this->options['break_phrase'])) {
      views_break_phrase_string($argument, $this);
    }
    else {
      $this->value = array($argument);
      $this->operator = 'or';
    }

    if (!empty($this->definition['many to one'])) {
      if (!empty($this->options['glossary'])) {
        $this->helper->formula = TRUE;
      }
      $this->helper->ensure_my_table();
      $this->helper->add_filter();
      return;
    }

    $this->ensure_my_table();
    $formula = FALSE;
    if (empty($this->options['glossary'])) {
      $field = "$this->table_alias.$this->real_field";
    }
    else {
      $formula = TRUE;
      $field = $this->get_formula();
    }

    if (count($this->value) > 1) {
      $operator = 'IN';
      $argument = $this->value;
    }
    else {
      $operator = '=';
    }

    if ($formula) {
      $placeholder = $this->placeholder();
      if ($operator == 'IN') {
        $field .= " IN($placeholder)";
      }
      else {
        $field .= ' = ' . $placeholder;
      }
      $placeholders = array(
        $placeholder => $argument,
      );
      $this->query->add_where_expression(0, $field, $placeholders);
    }
    else {
      $this->query->add_where(0, $field, $argument, $operator);
    }
  }
}
