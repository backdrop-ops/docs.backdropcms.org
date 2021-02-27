<?php
/**
 * @file
 * Field validation plain text validator.
 */
class field_validation_plain_text_validator extends field_validation_validator {

  /**
   * Validate field.
   */
  public function validate() {
    if ($this->value != '' && (strcmp($this->value, strip_tags($this->value)))) {
      $this->set_error();
    }
  }

}
