<?php

/**
 * @file
 * Classes with members that extend each other in a loop.
 */

/**
 * First class.
 */
class FirstClass extends SecondClass {
  /**
   * A member function.
   */
  function member_one() {
  }
}

/**
 * Second class.
 */
class SecondClass extends ThirdClass {
  /**
   * A member function.
   */
  function member_one() {
  }
}

/**
 * Third class.
 */
class ThirdClass extends FirstClass {
  /**
   * A member function.
   */
  function member_one() {
  }
}
