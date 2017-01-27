<?php

/**
 * @file
 * DL list plugin for Views.
 */

/**
 * Style plugin to render items in a DL list.
 *
 * Note that the rows must be formatted into DT and DD elements!
 */
class api_views_plugin_style_dl_list extends views_plugin_style {
  function option_definition() {
    $options = parent::option_definition();

    $options['class'] = array('default' => '');
    $options['wrapper_class'] = array('default' => 'item-list');

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['note'] = array(
      '#type' => 'markup',
      '#markup' => '<h3>' . t('You must format the rows into DT and DT elements yourself!') . '</h3>',
      '#weight' => -999,
    );

    $form['wrapper_class'] = array(
      '#title' => t('Wrapper class'),
      '#description' => t('The class to provide on the wrapper, outside the list.'),
      '#type' => 'textfield',
      '#size' => '30',
      '#default_value' => $this->options['wrapper_class'],
    );
    $form['class'] = array(
      '#title' => t('List class'),
      '#description' => t('The class to provide on the list element itself.'),
      '#type' => 'textfield',
      '#size' => '30',
      '#default_value' => $this->options['class'],
    );

  }
}
