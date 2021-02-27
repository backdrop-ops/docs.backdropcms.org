<?php
/**
 * @file
 * Field validation must be empty validator.
 */
class field_validation_must_be_empty_validator extends field_validation_validator {

  /**
   * Validate field.
   */
  public function validate() {
    if ($this->value != '') {
      $this->set_error();
    }
  }

}
