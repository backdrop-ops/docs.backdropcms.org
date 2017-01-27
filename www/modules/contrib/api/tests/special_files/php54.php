<?php

/**
 * @file
 * Tests some PHP 5.4 features.
 */

/**
 * Tests that PHP 5.4 array syntax can be parsed and displayed.
 */
function array_5_4() {
  // New array syntax.
  $foo = [
    'href' => 'bar',
    [[ 'nested' => 'valid']],
  ];

  // Use of [] for array indexing.
  $foo = $my_array[$i + 7]['bar'][some_function_call('foo', 'bar', 'baz')][3];

  // Mixture of old and new array syntax.
  $foo = array(
    'bar' => 'baz',
    'boohoo',
    ['mixing' => 'bad', 'but' => 'legal'],
    array(1, 3, 5),
  );
}
