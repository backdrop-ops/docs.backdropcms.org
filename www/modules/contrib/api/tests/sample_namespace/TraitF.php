<?php

/**
 * @file Contains \api\test1\TraitF.
 */

namespace api\test1;

/**
 * A sample trait.
 *
 * Longer description of the trait.
 */
trait TraitF {

  /**
   * Property to inherit.
   */
  public $fvar = 'hello';

  /**
   * Method to inherit.
   */
  protected function xyz() {
    return 7;
  }
}
