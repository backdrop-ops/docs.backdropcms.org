<?php
/**
 * @file
 * Field validation email validator.
 */
class field_validation_email_validator extends field_validation_validator {

  /**
   * Validate field.
   */
  public function validate() {
    if ($this->value != '' && (!valid_email_address($this->value))) {
      $this->set_error();
    }
  }

}
