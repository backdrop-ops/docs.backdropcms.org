<?php

/**
 * @file No namespace here.
 */

/**
 * Sample class not in a namespace.
 */
class ClassQ {
  /**
   * A method.
   */
  function bMethod() {
    // This should turn into a link to the YML file, as if it were config.
    $my_config->get('sample.routing');
  }
}

/**
 * And a function name.
 */
function another_function() {
  // This should turn into a link to the yml file.
  $foo->url('user_register');
}
