<?php

/**
 * Helper class to ensure callbacks can be objects.
 */
class FeedsTestsPreprocess {

/**
 * Preprocess callback for test_target.
 *
 * @see feeds_tests_feeds_processor_targets()
 */
  public static function callback(array $target, array &$mapping) {
    $mapping['required_value'] = TRUE;
  }
}
