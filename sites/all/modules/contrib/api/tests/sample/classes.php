<?php

/**
 * @file Object-oriented tests.
 */

/**
 * @defgroup class_samples Class Samples
 *
 * A sample group of classes. Should not include members.
 *
 * @{
 */

/**
 * Sample class.
 *
 * This example now includes some annotation for testing, including a function
 * name that should turn into a link in the code section.
 *
 * @Plugin(
 *   id = "samplefoo",
 *   title = @Translation("A great plugin title"),
 *   description = @Translation("A very descriptive plugin description."),
 *   something = 'foo_sample_name'
 * )
 */
class Sample extends ClassNotDefinedHere implements SampleInterface {
  /**
   * A class constant.
   */
  const constant = 'constant value';

  /**
   * A property.
   *
   * @var SampleInterface
   *
   * Some text to go after the var line to verify that it gets put into the
   * body of the documentation and not as part of the var. It also should be
   * longer than 255 characters just as an added check, because the var will not
   * hold that much text. So it rambles a bit, but it's important nonetheless.
   * Just a bit more text and it is done.
   *
   * @deprecated This property is deprecated for sample purposes. Use
   *   something else instead.
   */
  private $property = 'variable value';

  /**
   * Metasyntatic member function.
   *
   * @throws SampleException when it all goes wrong.
   */
  public function foo() {
    // Test linking to a method.
    $x = self::baz();

    // Test linking to a property.
    $this->property = 3;

    // Test linking to a constant.
    $y = $this->constant;

    // But this shouldn't be a link, wrong syntax.
    bar();
  }

  /**
   * Only implemented in children.
   */
  public function baz() {
    // This should be a link.
    Sample::foo();

    // This should link to a search.
    $x->bar();
  }
}

/**
 * Sample interface.
 */
interface SampleInterface {
  /**
   * Implement this API.
   */
  public function foo();
}

/**
 * Subclass.
 *
 * @deprecated This class is deprecated for sample purposes. Use
 *   something else instead.
 *
 * @see Sample::foo()
 */
class SubSample extends Sample implements SampleInterfaceTwo, InterfaceNotDefinedHere {
  // Not documented (this is intentional for testing).
  public function bar() {
    // This should link to parent method.
    $x = parent::foo();

    // This should link to the parent method, which is not overridden on
    // this class.
    $this->baz();
  }
}

/**
 * Another Sample interface.
 */
interface SampleInterfaceTwo {
  /**
   * A public method.
   */
  public function bar();
}

$random_assignment_not_to_be_parsed = NULL;

abstract class Sample2 implements SampleInterface {
  public function baz() {
  }
}

/**
 * @} end class_samples
 */

// This function call in the global space is for testing whether references
// can be detected here.
$xyz = sample_class_function();
