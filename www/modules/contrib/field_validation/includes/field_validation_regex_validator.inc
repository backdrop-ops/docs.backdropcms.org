<?php
/**
 * @file
 * Field validation regex validator.
 */
class field_validation_regex_validator extends field_validation_validator {

  /**
   * Validate field.
   */
  public function validate() {
    mb_regex_encoding('UTF-8');
    $regex = $this->rule->settings['data'];
    if ($this->value != '' && (!mb_ereg("$regex", $this->value))) {
      $this->set_error();
    }
  }

  /**
   * Provide settings option.
   */
  function settings_form(&$form, &$form_state) {
    $default_settings = $this->get_default_settings($form, $form_state);
    // Print debug($default_settings);
    $form['settings']['data'] = array(
      '#title' => t('Regex code'),
      '#description' => t("Specify regex code to validate the user input against."),
      '#type' => 'textarea',
      '#default_value' => isset($default_settings['data']) ? $default_settings['data'] : '',
    );
    parent::settings_form($form, $form_state);
  }

}
