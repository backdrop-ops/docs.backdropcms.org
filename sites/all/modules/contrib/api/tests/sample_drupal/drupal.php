<?php

/**
 * @file
 * A few core Drupal functions to test linking.
 */

/**
 * Invokes a hook in a particular module.
 *
 * @param $module
 *   The name of the module (without the .module extension).
 * @param $hook
 *   The name of the hook to invoke.
 * @param ...
 *   Arguments to pass to the hook implementation.
 *
 * @return
 *   The return value of the hook implementation.
 */
function module_invoke($module, $hook) {
  // Function body omitted for this sample.
}

/**
 * Invokes a hook in all enabled modules that implement it.
 *
 * @param $hook
 *   The name of the hook to invoke.
 * @param ...
 *   Arguments to pass to the hook.
 *
 * @return
 *   An array of return values of the hook implementations. If modules return
 *   arrays from their implementations, those are merged into one array.
 */
function module_invoke_all($hook) {
  // This should turn into a link.
  theme('foo', $data);

  // This should not turn into a link.
  sample_function();
}

/**
 * Determines which modules are implementing a hook.
 *
 * @param $hook
 *   The name of the hook (e.g. "help" or "menu").
 *
 * @return
 *   An array with the names of the modules which are implementing this hook.
 *
 * @see module_implements_write_cache()
 */
function module_implements($hook) {
  // Function body omitted for this sample.
}

/**
 * Passes alterable variables to specific hook_TYPE_alter() implementations.
 *
 * This dispatch function hands off the passed-in variables to type-specific
 * hook_TYPE_alter() implementations in modules. It ensures a consistent
 * interface for all altering operations.
 *
 * @param $type
 *   A string describing the type of the alterable $data. 'form', 'links',
 *   'node_content', and so on are several examples. Alternatively can be an
 *   array, in which case hook_TYPE_alter() is invoked for each value in the
 *   array, ordered first by module, and then for each module, in the order of
 *   values in $type. For example, when Form API is using drupal_alter() to
 *   execute both hook_form_alter() and hook_form_FORM_ID_alter()
 *   implementations, it passes array('form', 'form_' . $form_id) for $type.
 * @param $data
 *   The variable that will be passed to hook_TYPE_alter() implementations to be
 *   altered. The type of this variable depends on the value of the $type
 *   argument. For example, when altering a 'form', $data will be a structured
 *   array. When altering a 'profile', $data will be an object.
 * @param $context1
 *   (optional) An additional variable that is passed by reference.
 * @param $context2
 *   (optional) An additional variable that is passed by reference. If more
 *   context needs to be provided to implementations, then this should be an
 *   associative array as described above.
 */
function drupal_alter($type, &$data, &$context1 = NULL, &$context2 = NULL) {
  // Function body omitted for this sample.
}

/**
 * Generates themed output.
 *
 * All requests for themed output must go through this function. It examines
 * the request and routes it to the appropriate
 * @link themeable theme function or template @endlink, by checking the theme
 * registry.
 *
 * @param $hook
 *   The name of the theme hook to call. If the name contains a
 *   double-underscore ('__') and there isn't an implementation for the full
 *   name, the part before the '__' is checked. This allows a fallback to a more
 *   generic implementation. For example, if theme('links__node', ...) is
 *   called, but there is no implementation of that theme hook, then the 'links'
 *   implementation is used.
 * @param $variables
 *   An associative array of variables to merge with defaults from the theme
 *   registry, pass to preprocess and process functions for modification, and
 *   finally, pass to the function or template implementing the theme hook.
 *   Alternatively, this can be a renderable array, in which case, its
 *   properties are mapped to variables expected by the theme hook
 *   implementations.
 *
 * @return
 *   An HTML string representing the themed output.
 */
function theme($hook, $variables = array()) {
  // Function body omitted for this sample.
}
