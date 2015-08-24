<?php

/**
 * @file badheader.php
 *
 * This is a test of a non-conforming file docblock with defgroup in it.
 *
 * @defgroup mygroup Test group
 * @{
 */

/**
 * A function.
 */
function badheader_fun() {
  theme('sample', array());
}

/**
 * @} End of "defgroup mygroup".
 */

/**
 * @defgroup another Another bad group
 *
 * This doc block comes before a class but has a defgroup in it.
 * Testing a bug: making sure that the class still gets saved.
 */
class ClassWithDefgroupDocBlock {
  function foo() {
    return 1;
  }
}
