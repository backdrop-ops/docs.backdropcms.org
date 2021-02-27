<?php
/**
 * @file
 * Property validation email validator.
 */
class property_validation_email_validator extends property_validation_validator {

  /**
   * Validate property.
   */
  public function validate() {
    if ($this->value != '' && (!valid_email_address($this->value))) {
      $this->set_error();
    }
  }

}
