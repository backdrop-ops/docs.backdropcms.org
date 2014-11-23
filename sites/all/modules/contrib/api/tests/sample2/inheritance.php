<?php

/**
 * @file
 * Samples to use for testing class and interface inheritance.
 */

/**
 * A base interface.
 */
interface BaseInterface {
  /**
   * Defines a function that all classes really need.
   */
  function base_function() {}
}

/**
 * Another base interface.
 */
interface AnotherBaseInterface {
  /**
   * Defines another function that all classes really need.
   */
  function another_base_function() {}
}

/**
 * An extending interface.
 */
interface SecondInterface extends BaseInterface {
  /**
   * Defines another function.
   */
  function second_function() {}
}

/**
 * A third interface, with two extends in it.
 */
interface ThirdInterface extends BaseInterface, SomeExternalInterface {
  /**
   * Defines a third function.
   */
  function third_function() {}
}

/**
 * An interface that extends both base interfaces.
 */
interface FourthInterface extends BaseInterface, AnotherBaseInterface {
  /**
   * Defines a fourth function.
   */
  function fourth_function() {}
}

/**
 * A class that implements the second interface but not the third.
 */
class ExcitingClass implements SecondInterface {
  /**
   * Implements a second function.
   */
  function second_function() {
    return "second";
  }

  /**
   * Implements a base function.
   */
  function base_function() {
    return "base";
  }
}

/**
 * A class that extends ExcitingClass.
 *
 * It has one new method, and inherits the others without overriding.
 */
class AnotherExcitingClass extends ExcitingClass {

  /**
   * Defines a new function.
   */
  function another_function() {
    return "another";
  }
}

/**
 * Yet another extending function.
 *
 * This one overrides one of the functions on ExcitingClass.
 */
class YetAnotherExcitingClass extends AnotherExcitingClass {

  /**
   * Overrides the exciting function.
   */
  function second_function() {
    return "two";
  }
}
