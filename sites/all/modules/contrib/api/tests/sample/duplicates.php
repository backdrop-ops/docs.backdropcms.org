<?php

function duplicate_function() {
  // Test linking to the two PHP branches.
  $x = foo_function($y, $z);
  $q = bar_function($x, $y);
  $z = not_a_function();
  $r = substr('some text goes here', 0, 3);
}

define('DUPLICATE_CONSTANT');
