<?php
/**
 * @file
 * Property validation must be empty validator.
 */
/**
 *
 */
class property_validation_must_be_empty_validator extends property_validation_validator {

  /**
   * Validate property.
   */
  public function validate() {
    if ($this->value != '') {
      $this->set_error();
    }
  }

}
