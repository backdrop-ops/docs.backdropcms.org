<?php
/**
 * @file
 * Property validation plain text validator.
 */
class property_validation_plain_text_validator extends property_validation_validator {

  /**
   * Validate field.
   */
  public function validate() {
    if ($this->value != '' && (strcmp($this->value, strip_tags($this->value)))) {
      $this->set_error();
    }
  }

}
