<?php

/**
 * @file
 * A file that should not be indexed, as it is in an excluded directory.
 */

/**
 * An excluded function that should not be in the package.
 */
function excluded_function() {
}

/**
 * @defgroup samp_GRP-6.x Samples
 *
 * A group with a duplicate machine name, to test whether that works.
 *
 * @{
 */

/**
 * A duplicate class for testing whether that works.
 */
class Sample implements SampleInterface {
}

/**
 * A duplicate interface for testing whether that works.
 */
interface SampleInterface {
}

/**
 * A non-duplicate function to verify it gets into the main group.
 */
function non_duplicate_name() {
}

/**
 * @} end samp_GRP-6.x
 */

/**
 * @defgroup sample_function Test group
 *
 * A group whose machine name is the same as another function.
 *
 * @{
 */

/**
 * Some function in this group.
 */
function some_name() {
}

/**
 * @} end sample_function
 */

