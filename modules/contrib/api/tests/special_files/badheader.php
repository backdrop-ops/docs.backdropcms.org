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
