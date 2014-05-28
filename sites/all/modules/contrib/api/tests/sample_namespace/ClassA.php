<?php

/**
 * @file Contains \api\test1\ClassA.
 */

namespace api\test1;

use api\test2\ClassB as MyClassB;
use api\test2\InterfaceC;

/**
 * Sample class in a namespace.
 *
 * This documentation should make links to another_function() as well
 * as MyClassB::bMethod() and InterfaceD::dMethod().
 */
class ClassA extends MyClassB implements InterfaceC, InterfaceD {

  /**
   * Overrides InterfaceC::cMethod();
   */
  function cMethod() {
    $foo = new \api\test2\ClassE();
    $bar = \api\test2\ClassE::staticMethod();
    $baz = parent::bMethod();
    $fb = self::sMethod();
    $blech = static::sMethod();
    $fuz = $this->dMethod();
  }

  /**
   * Overrides InterfaceD::dMethod();
   */
  function dMethod() {
  }

  /**
   * A static method.
   */
  static function sMethod() {
  }
}
