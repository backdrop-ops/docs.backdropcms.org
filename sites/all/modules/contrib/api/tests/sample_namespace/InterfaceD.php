<?php

/**
 * @file Contains \api\test1\InterfaceD.
 */

namespace api\test1;

/**
 * Another sample interface in a namespace.
 */
interface InterfaceD {
  /**
   * Another exciting method.
   *
   * @param \api\test2\InterfaceC $foo
   *   Something to test making links to a class.
   */
  function dMethod($foo) {}
}
