<?php
/**
 * @file
 * Extra items to include for tests of branch linking.
 */

/**
 * For testing duplicate constant linking.
 */
define('DUPLICATE_CONSTANT', 12);

/**
 * Sample class.
 */
class Sample {

  /**
   * A member function.
   */
  public function foo() {
  }
}

/**
 * Another sample class.
 */
class DifferentClassName {

  /**
   * A member function that happens to have the same name as another one.
   */
  public function foo() {
  }
}
