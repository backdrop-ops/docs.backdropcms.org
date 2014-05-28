<?php

/**
 * @file
 * A sample file.
 */

/**
 * @defgroup samp_GRP-6.x Samples
 *
 * A sample group.
 *
 * @section sec_one Section 1
 * This is the content of the first section. This is a link to the other
 * section @ref sec_two right here in the text.
 *
 * @subsection sub_a Sub-section A
 * Here's a sub-section.
 *
 * @section sec_two Section 2
 * This is the content of the second section. This is a link to the
 * sub-section @ref sub_a right here in the text.
 *
 * @{
 */

/**
 * A sample global.
 *
 * This should not link to SAMPLE_FUNCTION(). And this should not link to
 * sample_constant. And this text should not link to either, because they match
 * but are not separate words: SAMPLE_CONSTANT/sample_function(). The see below
 * should not link to the defgroup above either.
 *
 * @see http://example.com/samp_GRP-6.x
 *
 * @deprecated This global is deprecated for sample purposes. Use
 *   something else instead.
 */
global $sample_global;

/**
 * A sample constant.
 *
 * @deprecated This constant is deprecated for sample purposes. Use
 *   something else instead.
 */
define('SAMPLE_CONSTANT', 7);

/**
 * A sample function.
 *
 * @see duplicate_function()
 *
 * Use for sample-related purposes.
 *
 * This is a sample list:
 * - One item.
 * - Another item.
 *   - A sub-item. This one goes for multiple lines, just to make
 *     sure that that works. It should. And here's a colon: just to
 *     make sure that isn't wonky.
 *   - Another sub-item.
 * - A third item.
 * This is not part of the list.
 *
 * This list uses our key format:
 * - key1: The usual format, no quotes.
 * - 'key2': Sometimes we have quotes.
 * - "key3 multiple": Sometimes double quotes and multiple words.
 * - The following item should not have strong formatting.
 * - http://example.com
 *
 * And here is a code block with indentation:
 * @code
 *   $message = t('An error occurred.');
 *   drupal_set_message($message, 'error');
 *   $output .= $message;
 * @endcode
 *
 * And another way to have indentation, in a list:
 * - Here is some code:
 *   @code
 *   $message = t('An error occurred.');
 *   drupal_set_message($message, 'error');
 *   $output .= $message;
 *   @endcode
 *
 * Here are some references to files: (htmlfile.html), textfile.txt, classes.php
 * in the docs. They should all be links, and just in case, put one in parens.
 * And here are some using link tags:
 * - @link htmlfile.html HTML link text @endlink
 * - @link textfile.txt Text link text @endlink
 * - @link classes.php PHP link text @endlink
 *
 * @param $parameter
 *   A generic parameter.
 * @param $complex_parameter
 *   Information about the $complex_parameter parameter. Example:
 *   @code
 *     $complex_parameter = 3;
 *   @endcode
 *
 *   A second paragraph about the $complex_parameter parameter.
 *
 *   @link http://php.net this is a link for the parameter @endlink
 *
 * @return
 *   Something about the return value.
 *
 *   A second paragraph about the return value.
 *
 * @see SAMPLE_CONSTANT
 */
function sample_function($parameter, $complex_parameter) {
  // Have this function call itself, to verify that it doesn't come up counted
  // as a function call.
  $foo = sample_function();
}

/**
 * @} end samp_GRP-6.x
 */

/**
 * Function that has classes for parameter and return value.
 *
 * This code segment is used to test linking.
 * @code
 * $foo = sample_function();
 * $bar = theme('sample_one', $foo);
 * $x = module_invoke_all('sample_name', $foo, $bar);
 * $xx = drupal_alter('another_sample', $foo);
 * $k = SAMPLE_CONSTANT;
 * @endcode
 *
 * @param SubSample $parameter
 *   This parameter should link to the class.
 *
 * @return SampleInterface
 *   This return value should link to the interface.
 *
 * @deprecated This function is deprecated for sample purposes. Use
 *   sample_in_code_links() instead.
 *
 * @ingroup samp_GRP-6.x
 */
function sample_class_function($parameter) {
}

/**
 * For testing duplicate function name linking.
 *
 * Also, here is some test documentation for multiple links in one paragraph.
 * This comes from the Simplenews project, modified to make links to groups
 * that we have here.
 *
 * @link samp_GRP-6.x Subscribers @endlink subscribe to @link class_samples
 * newsletters (categories) @endlink. That connection is called
 * a @link samp_GRP-6.x subscription @endlink. Nodes of enabled content types
 * are @link class_samples newsletter issues @endlink. These are then sent to
 * the subscribers of the newsletter the issue is attached to.
 *
 * And here is a test of a link with an apostrophe in it, and how about some
 * double quotes for good measure?
 *
 * @link class_samples Won't the apostophe "just work" here? @endlink
 * @link http://example.com Won't the apostophe "just work" here too? @endlink
 */
function duplicate_function() {
  $foo = sample_function();
}

/**
 * For testing duplicate constant linking.
 */
define('DUPLICATE_CONSTANT', 12);

/**
 * Respond to sample updates.
 *
 * This hook is for testing hook linking.
 */
function hook_sample_name() {
  // Verify that wrong-case links are not made.
  $x = SAMPLE_FUNCTION();
  $y = sample_constant;
}

/**
 * Alter samples.
 *
 * This hook is for testing alter hook linking.
 */
function hook_another_sample_alter() {
  $foo = sample_function();
}

/**
 * Returns HTML for a sample.
 *
 * This theme function is for testing linking in theme().
 *
 * @param $variables
 *   An associative array containing:
 *   - foo: The foo object that is being formatted.
 *   - show_bar: TRUE to show the bar component, FALSE to omit it.
 */
function theme_sample_one($variables) {
  $foo = sample_function();
}

/**
 * Returns HTML for another sample.
 *
 * This theme function is for testing linking in theme(). It should not be
 * linked, because of the sample-two.tpl.php file, which has higher priority.
 *
 * @param $variables
 *   An associative array containing:
 *   - foo: The foo object that is being formatted.
 *   - show_bar: TRUE to show the bar component, FALSE to omit it.
 */
function theme_sample_two($variables) {
  $foo = sample_function();
}

/**
 * Returns HTML for yet another sample.
 *
 * This theme function is for testing linking in theme().
 *
 * @param $variables
 *   An associative array containing:
 *   - foo: The foo object that is being formatted.
 *   - show_bar: TRUE to show the bar component, FALSE to omit it.
 */
function theme_sample_four($variables) {
  $foo = sample_function();
}

/**
 * Does something interesting, to test in-code linking.
 *
 * @see samp_GRP-6.x
 */
function sample_in_code_links() {
  // This should make a link to the global.
  global $sample_global;
  // This is not really a global.
  global $nonexistent_global;

  // Should link to function.
  $foo = sample_function();
  // Should link to theme function.
  $bar = theme('sample_one', $foo);
  // Should link to theme template, not function, though both exist.
  $baz = theme('sample_two', $foo);
  // Should link to theme template.
  $boo = theme('sample_three');
  // Should link to the sample_four theme function.
  $bop = theme('sample_four__option', $foo);

  // Should link to hook.
  $x = module_invoke_all('sample_name', $foo, $baz);
  $stuff = '';
  // Should link to hook.
  foreach (module_implements('sample_name') as $module) {
    // Should link to hook. Note that the variable name has to be $module for
    // this link to work.
    module_invoke($module, 'sample_name', $baz);
  }

  // Should link to alter hook.
  $xx = drupal_alter('another_sample', $foo);

  // Should link to search for this function.
  $z = duplicate_function();

  // Should link to class.
  $j = new SubSample();

  // Should link to constant.
  $k = SAMPLE_CONSTANT;
  // Should link to search for this constant.
  $l = DUPLICATE_CONSTANT;

  $menu = array(
    'title' => 'A title goes here.',
    // Should link to sample_function.
    'page callback' => 'sample_function',
  );

  // Functions that don't exist, so should not be linked, but
  // should still be visible.
  $a = nonexistent_function($a, $b);
  $b = module_invoke_all('nonexistent_hook', $foo);
  $c = theme('nonexistent_theme_hook', $foo);
  foreach (module_implements('nonexistent_hook') as $module) {
    module_invoke($module, 'nonexistent_hook', $foo);
  }
  $d = drupal_alter('nonexistent_alter_name', $foo);

  // This should make a link only if the second sample is loaded.
  $x = second_sample_function();
}

/**
 * Implements hook_sample_name().
 *
 * This is used for testing the "N functions implement hook()" link on function
 * pages.
 */
function foo_sample_name() {
  $foo = sample_function();
}

/**
 * Function with same name as a hook.
 */
function sample_name() {
  // This should turn into a link to the hook, and the hook should reference
  // this function as an invoker.
  module_invoke_all('sample_name');
  $foo = sample_function();
}

/**
 * Function with same name as an alter hook.
 */
function another_sample() {
  // This should turn into a link to the hook, and the hook should reference
  // this function as an invoker.
  drupal_alter('another_sample');
  $foo = sample_function();
}

/**
 * Function with the same name as a theme hook.
 */
function sample_one() {
  // This should turn into a link to the theme function, , and the theme
  // function should reference this function as an invoker.
  theme('sample_one');
  $foo = sample_function();
}


// For testing that functions without names are not saved as doc objects.
$var = 1;
function () use ($var) {};
function () use ($var) {};
