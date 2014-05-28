<?php

/**
 * @file
 * Formatting functions for the API module.
 */


/**
 * Turns function names into links in code.
 *
 * @param string $code
 *   PHP code to scan for function names.
 * @param object $branch
 *   Branch to make the links in.
 * @param int $file_did
 *   Documentation ID of the file the code is in (for namespace information).
 *   Can omit if namespaces are not relevant.
 * @param int $class_did
 *   Documentation ID of the class the code is in (if any).
 * @param bool $is_drupal
 *   (optional) If set explicitly to FALSE, omit the Drupal-specific link steps.
 *
 * @return string
 *   Code with function names formatted as links.
 *
 * @see api_link_documentation
 */
function api_link_code($code, $branch, $file_did = NULL, $class_did = NULL, $is_drupal = TRUE) {
  if ($is_drupal) {
    $steps = array('code hook name', 'code alter hook name', 'code theme hook name', 'code function', 'code member', 'code string', 'yaml string', 'yaml file', 'yaml reference', 'annotation string', 'code global', 'code class');
  }
  else {
    $steps = array('code function', 'code member', 'code string', 'yaml reference', 'annotation string', 'code global', 'code class');
  }

  return _api_link_documentation($code, $branch, $file_did, $class_did, $steps);
}

/**
 * Turns function names into links in documentation.
 *
 * @param string $documentation
 *   Documentation to scan for function names.
 * @param object $branch
 *   Branch to make the links in.
 * @param int $file_did
 *   Documentation ID of the file the code is in (for namespace information).
 *   Can omit if namespaces are not relevant.
 * @param int $class_did
 *   Documentation ID of the class the documentation is in (if any).
 * @param bool $aggressive_classes
 *   Try linking every word with a capital letter to a class or interface, if
 *   TRUE. Otherwise, just try to link words with backslashes in them.
 * @param bool $aggressive_topics
 *   For use in @see only, if TRUE try linking every paragraph as a topic name.
 * @param bool $is_drupal
 *   (optional) If set explicitly to FALSE, omit the Drupal-specific link steps.
 *
 * @return string
 *   Documentation with function names formatted as links.
 *
 * @see api_link_code
 */
function api_link_documentation($documentation, $branch, $file_did = NULL, $class_did = NULL, $aggressive_classes = FALSE, $aggressive_topics = FALSE, $is_drupal = TRUE) {

  // First make as many links as possible in the text. To do that, we have to
  // find a branch to use if possible, and figure out what stages we need.

  if (is_null($branch)) {
    $branch = api_get_active_branch();
  }

  // Start with the code-related stages, for the @code sections. Follow with
  // the basic documentation stages.
  if ($is_drupal) {
    $stages = array('code hook name', 'code alter hook name', 'code theme hook name', 'code function', 'code member', 'code string', 'yaml string', 'yaml file', 'annotation string', 'code global', 'code class', 'tags', 'link', 'function');
  }
  else {
    $stages = array('code function', 'code member', 'code string', 'annotation string', 'code global', 'code class', 'tags', 'link', 'function');
  }

  if ($aggressive_topics) {
    // Look for topics before classes, constants, and files.
    $stages[] = 'topic';
  }
  $stages[] = 'file';
  $stages[] = 'constant';
  if ($aggressive_classes) {
    $stages[] = 'class';
  }
  else {
    $stages[] = 'definite class';
  }

  $documentation = _api_link_documentation($documentation, $branch, $file_did, $class_did, $stages);

  // Now remove escaping from \@.
  $documentation = preg_replace('!\\\@!', '@', $documentation);

  // Now use the standard Drupal URL filter to make links out of bare URLs in
  // the text.
  $filter = new stdClass();
  $filter->callback = '_filter_url';
  $filter->settings = array('filter_url_length' => 72);

  return _filter_url($documentation, $filter);
}

/**
 * Makes links in documentation and code.
 *
 * Recursively calls itself to iterate through various stages. At each stage,
 * api_split() is called to find matches for a particular regular expression
 * pattern and process the matches via callback functions api_link_name(),
 * api_link_member_name(), and api_link_link().
 *
 * @param string $documentation
 *   PHP code or documentation to scan for text to link.
 * @param object $branch
 *   Branch to make the links in.
 * @param int $file_did
 *   Documentation ID of the file the code is in (for namespace information).
 *   Can omit if namespaces are not relevant.
 * @param int $class_did
 *   Documentation ID of the class the documentation is in (if any).
 * @param array $stages
 *   Array of stages to process, which determines what type of links to make.
 *
 * @return string
 *   $documentation with text turned into links.
 */
function _api_link_documentation($documentation, $branch, $file_did = NULL, $class_did = NULL, $stages = array()) {
  // Pop off the next stage to run.
  $stage = array_shift($stages);

  // For this stage, figure out what regular expression pattern to use for
  // matching, what callback to call from api_split() for pattern matches,
  // and what arguments to pass to the callback function.
  $callback_match = 'api_link_name';
  $prepend = '';
  $append = '';
  $prepend_if_not_found = NULL;
  $use_php = FALSE;
  $type = '';
  $pattern = '';
  $continue_matching = FALSE;

  switch ($stage) {
    case 'tags':
      // Find HTML tags, not filtered.
      $callback_match = NULL;
      $pattern = '/(<[^>]+?' . '>)/';
      break;

    case 'link':
      // Find @link.
      $pattern = '/' . API_RE_TAG_START . 'link\s+(.*)\s+' . API_RE_TAG_START . 'endlink/U';
      $callback_match = 'api_link_link';
      break;

    case 'function':
      // Find function names, which are preceded by white space and followed by
      // '('.
      $append = '(';
      $pattern = '!' . API_RE_WORD_BOUNDARY_START . '(' . API_RE_FUNCTION_IN_TEXT . ')\(!';
      $type = 'function';
      $use_php = TRUE;
      break;

    case 'code function':
      // Find function names in marked-up code.
      $pattern = '!<span class="php-function-or-constant">(' . DRUPAL_PHP_FUNCTION_PATTERN . ')</span>!';
      $prepend = '<span class="php-function-or-constant">';
      $append = '</span>';
      $type = 'function_or_constant';
      $use_php = TRUE;
      $continue_matching = TRUE;
      break;

    case 'code class':
      // Find class names in marked-up code, as constructors.
      $pattern = '!<span class="php-function-or-constant">(' . API_RE_CLASS_NAME_TEXT . ')</span>!';
      $prepend = '<span class="php-function-or-constant">';
      $append = '</span>';
      $type = 'class';
      break;

    case 'code global':
      // Find global variable names in marked-up code.
      $pattern = '!<span class="php-keyword">global</span> <span class="php-variable">\$(' . DRUPAL_PHP_FUNCTION_PATTERN . ')</span>!';
      $prepend = '<span class="php-keyword">global</span> <span class="php-variable">$';
      $append = '</span>';
      $type = 'global';
      break;

    case 'code string':
      // Find potential function names (callback strings) in marked-up code.
      // These are all strings that are legal function names, where the function
      // name is put into something like a hook_menu() page callback as a
      // string.
      $pattern = '!<span class="php-string">\'(' . DRUPAL_PHP_FUNCTION_PATTERN . ')\'</span>!';
      $prepend = '<span class="php-function-or-constant">\'';
      $append = '\'</span>';
      $prepend_if_not_found = '<span class="php-string">\'';
      $type = 'function';
      $continue_matching = TRUE;
      break;

    case 'yaml string':
      // Find potential YAML key strings in marked-up code.
      $pattern = '!<span class="php-string">\'(' . API_RE_YAML_STRING . ')\'</span>!';
      $prepend = '<span class="php-function-or-constant">\'';
      $append = '\'</span>';
      $prepend_if_not_found = '<span class="php-string">\'';
      $type = 'yaml_string';
      $continue_matching = TRUE;
      break;

    case 'yaml file':
      // Find potential YAML file name strings in marked-up code.
      $pattern = '!<span class="php-string">\'(' . API_RE_YAML_STRING . ')\'</span>!';
      $prepend = '<span class="php-function-or-constant">\'';
      $append = '\'</span>';
      $prepend_if_not_found = '<span class="php-string">\'';
      $type = 'yaml_file';
      break;

    case 'annotation string':
      // Find potential function, method, and class strings in annotations.
      $pattern = '!<span class="php-string">&quot;(' . API_RE_FUNCTION_IN_TEXT . ')&quot;</span>!';
      $prepend = '<span class="php-function-or-constant">"';
      $append = '"</span>';
      $prepend_if_not_found = '<span class="php-string">"';
      $type = 'function_or_constant';
      $continue_matching = TRUE;
      break;

    case 'yaml reference':
      // Find potential function names (callback strings) in marked-up YAML
      // code. These are all strings that are legal function names, possibly
      // with namespaces and class names, possibly in quotes.
      $pattern = '!<span class="yaml-reference">(' . API_RE_FUNCTION_IN_TEXT . ')</span>!';
      $prepend = '<span class="php-function-or-constant">';
      $append = '</span>';
      $type = 'yaml_reference';
      break;

    case 'code hook name':
      // Find potential hook names in marked-up code. These are strings that
      // are legal function names, which were found in parsing to be inside
      // module_implements() and related functions.
      $pattern = '!<span class="php-string potential-hook">\'(' . API_RE_FUNCTION_CHARACTERS . ')\'</span>!';
      $prepend = '<span class="php-function-or-constant">\'';
      $append = '\'</span>';
      $prepend_if_not_found = '<span class="php-string">\'';
      $type = 'hook';
      break;

    case 'code alter hook name':
      // Works like 'code hook name' above, but for alter hooks.
      $pattern = '!<span class="php-string potential-alter">\'(' . API_RE_FUNCTION_CHARACTERS . ')\'</span>!';
      $prepend = '<span class="php-function-or-constant">\'';
      $append = '\'</span>';
      $prepend_if_not_found = '<span class="php-string">\'';
      $type = 'alter hook';
      break;

    case 'code theme hook name':
      // Works like 'code hook name' above, but for theme hooks.
      $pattern = '!<span class="php-string potential-theme">\'(' . DRUPAL_PHP_FUNCTION_PATTERN . ')\'</span>!';
      $prepend = '<span class="php-function-or-constant">\'';
      $append = '\'</span>';
      $prepend_if_not_found = '<span class="php-string">\'';
      $type = 'theme';
      break;

    case 'code member':
      // Works like 'code hook name' above, but for class members.
      $callback_match = 'api_link_member_name';
      $pattern = '!(<span class="(?:php-function-or-constant|php-variable) [^"]+ member-of-[^"]+">\$*' . DRUPAL_PHP_FUNCTION_PATTERN . '</span>)!';
      break;

    case 'file':
      // Find file names, which are an arbitrary number of strings joined with
      // '.'
      $pattern = '%' . API_RE_WORD_BOUNDARY_START . API_RE_FILENAME . API_RE_WORD_BOUNDARY_END . '%';
      $type = 'file';
      break;

    case 'constant':
      // Find constants, UPPERCASE_LETTERS_WITH_UNDERSCORES.
      $pattern = '/' . API_RE_WORD_BOUNDARY_START . '([A-Z_]+)' . API_RE_WORD_BOUNDARY_END . '/';
      $type = 'constant';
      break;

    case 'class':
      // Find class names, which have a capital letter.
      $pattern = '/' . API_RE_WORD_BOUNDARY_START . '(' . API_RE_CLASS_NAME_TEXT . ')' . API_RE_WORD_BOUNDARY_END . '/';
      $type = 'class';
      break;

    case 'definite class':
      // Find definite class names, which have a backslash.
      $pattern = '/' . API_RE_WORD_BOUNDARY_START . '(' . API_RE_DEFINITE_CLASS_NAME_TEXT . ')' . API_RE_WORD_BOUNDARY_END . '/';
      $type = 'class';
      break;

    case 'topic':
      // Find topic/group names.
      $pattern = '/' . API_RE_WORD_BOUNDARY_START . '(' . API_RE_GROUP_NAME . ')' . API_RE_WORD_BOUNDARY_END . '/';
      $type = 'group';
      // Patterns that match topics might also match other objects.
      // keep looking for matches after running the linker.
      $continue_matching = TRUE;
      break;
  }

  // See if we have more stages to do. If so, set up to call this function
  // again; if not, we're done.
  $append_if_not_found = $append;
  $prepend_if_no_change = '';
  $append_if_no_change = '';
  if (count($stages) > 0) {
    $callback = '_api_link_documentation';
    if ($continue_matching) {
      // In this case, we want to tell the matching callback function not
      // to prepend or append anything, and api_split to put back the wrappers
      // if there was no change.
      $prepend_if_no_change = ($prepend_if_not_found) ? $prepend_if_not_found : $prepend;
      $append_if_no_change = $append;
      $prepend_if_not_found = '';
      $append_if_not_found = '';
    }
  }
  else {
    $callback = NULL;
  }

  return api_split($pattern, $documentation, $callback_match, array($branch, $prepend, $append, $file_did, $class_did, NULL, FALSE, $use_php, $prepend_if_not_found, $append_if_not_found, $type), $callback, array($branch, $file_did, $class_did, $stages), $continue_matching, $prepend_if_no_change, $append_if_no_change);
}

/**
 * Splits a string using a regular expression and processes using callbacks.
 *
 * @param $pattern
 *   The regular expression to match for splitting.
 * @param $subject
 *   The string to process.
 * @param $callback_match
 *   Function name to be called for text which matches $pattern. The first
 *   parameter will be the parenthesized expression in the pattern. Should
 *   return a string. NULL to pass the text through unchanged.
 * @param $callback_match_arguments
 *   An array of additional parameters for $callback_match.
 * @param $callback
 *   Function name to be called for text which does not match $pattern. The
 *   first parameter will be the text. Should return a string. NULL to pass the
 *   text through unchanged.
 * @param $callback_arguments
 *   An array of additional parameters for $callback.
 * @param $continue_matching
 *   If TRUE, call $callback again on the matched text if it is left unchanged
 *   by its callback.
 * @param $prepend_if_not_changed
 *   String to prepend if the match callback makes no change and we are
 *   continuing.
 * @param $append_if_not_changed
 *   String to append if the match callback makes no change and we are
 *   continuing.
 *
 * @return
 *   The original string, with both matched and unmatched portions filtered by
 *   the appropriate callbacks.
 */
function api_split($pattern, $subject, $callback_match = NULL, $callback_match_arguments = array(), $callback = NULL, $callback_arguments = array(), $continue_matching = FALSE, $prepend_if_not_changed = '', $append_if_not_changed = '') {
  $return = '';
  $matched = FALSE;
  foreach (preg_split($pattern . 'sm', $subject, -1, PREG_SPLIT_DELIM_CAPTURE) as $part) {
    // The return values will alternate being unmatched text and delimeters.
    // And note that the "delimiters" are only part of the expression.
    if ($matched) {
      // This is a "delimiter", which is a piece of the matched regular
      // expression (whatever is in parens).
      if (is_null($callback_match)) {
        $return .= $part;
      }
      else {
        $new_text = call_user_func_array($callback_match, array_merge(array($part), $callback_match_arguments));

        if ($new_text == $part && $continue_matching) {
          $new_text = call_user_func_array($callback, array_merge(array($prepend_if_not_changed . $part . $append_if_not_changed), $callback_arguments));
        }

        $return .= $new_text;
      }
    }
    else {
      // This is a part of the input text that did not match.
      if (is_null($callback)) {
        $return .= $part;
      }
      else {
        $return .= call_user_func_array($callback, array_merge(array($part), $callback_arguments));
      }
    }

    // This makes the foreach alternate between thinking it's a delimeter and
    // unmatched text.
    $matched = !$matched;
  }
  return $return;
}

/**
 * Links an object name to its documentation.
 *
 * Callback for api_split() in _api_link_documentation().
 *
 * Tries to find something to link to by looking for matches in the following
 * order:
 * - The passed-in branch.
 * - PHP reference branches, if $use_php is TRUE.
 * - The core branch with the same core compatibility.
 * - An API reference core branch with the same core compatibility.
 * - Any other branch with the same core compatibility.
 * - Any API reference branch with the same core compatibility.
 *
 * @param $name
 *   Object name to link to.
 * @param $branch
 *   Branch object indicating which branch to make the link in.
 * @param $prepend
 *   Text to prepend on the link.
 * @param $append
 *   Text to append on the link.
 * @param $file_did
 *   Documentation ID of the file the code is in (for namespace information).
 *   Can omit if namespaces are not relevant.
 * @param $class_did
 *   (unused) Documentation ID of the class this is part of (if any).
 * @param $text
 *   Link text. If omitted, uses $name.
 * @param $is_link
 *   TRUE if this was inside a @link.
 * @param $use_php
 *   TRUE if links to functions found in PHP reference branches should be
 *   checked for and made; FALSE to skip this (normally only set to TRUE if
 *   this is a Drupal-specific thing like a hook name and thus looking for PHP
 *   reference functions would be pointless). API reference branches are always
 *   checked regardless of $use_php.
 * @param $prepend_if_not_found
 *   Text to prepend if object is not found (defaults to $prepend).
 * @param $append_if_not_found
 *   Text to append if object is not found (defaults to $append).
 * @param $type
 *   The type of information $name represents. Possible values:
 *   - '': (default) $name is a normal object name.
 *   - 'hook': $name is a hook name.
 *   - 'alter hook': $name is an alter hook name.
 *   - 'theme': $name is a theme hook name.
 *   - 'function': $name is specifically a function ('file', 'constant', etc.
 *     also are supported).
 *   - 'function_or_constant': $name is either a function, constant, or class.
 *   - 'group': $name is a group/topic identifier.
 *   - 'global': $name is a global variable.
 *   - 'yaml_reference': $name is the possibly namespaced name of a function or
 *     method, or class, possibly in single or double quotes (in a YAML file).
 *   - 'yaml_string': $name is a string that could be a 'yaml string' reference.
 *   - 'yaml_file': $name is possibly the name of a YAML config file (missing
 *     the .yml extension).
 *
 * @return
 *   The text as a link to the object page.
 *
 * @see api_link_member_name()
 * @see api_link_link()
 */
function api_link_name($name, $branch, $prepend = '', $append = '', $file_did = NULL, $class_did = NULL, $text = NULL, $is_link = FALSE, $use_php = TRUE, $prepend_if_not_found = NULL, $append_if_not_found = NULL, $type = '') {

  if (!$text) {
    $text = $name;
  }
  $name = trim($name);

  if ($type == 'yaml_reference') {
    // Trim off quotes and match as any object name.
    $name = trim($name, '"\'');
    $type = 'function_or_constant';
    $is_link = FALSE;
  }
  elseif ($type == 'yaml_file') {
    // Add .yml and match as a file.
    $name = $name . '.yml';
    $type = 'file';
    $is_link = FALSE;
  }

  // Get some information we will need in several places below.
  $nameinfo = api_get_namespace_info($file_did);
  $namespaced_name = api_full_classname($name, $nameinfo['namespace'], $nameinfo['use_alias']);
  $core_compatibility = ($branch) ? $branch->core_compatibility : variable_get('api_default_core_compatibility', '');
  $api_branches = api_get_php_branches();

  // If we get here, we're looking to match some kind of documentation object.
  // Try to match within the passed-in branch.
  if ($branch) {
    $link = _api_make_match_link($name, $namespaced_name, $text, $type, $is_link, $branch);
    if ($link) {
      return $prepend . $link . $append;
    }
  }

  // If we get here, there wasn't a match. Try PHP functions.
  if ($use_php) {
    $link = _api_make_php_reference_link($name, $text);
    if ($link) {
      return $prepend . $link . $append;
    }
  }

  // If this is not a core branch, try matching in the core branch.
  $core_branch = api_find_core_branch($branch);

  if ($core_branch && $core_branch->branch_id != $branch->branch_id) {
    $link = _api_make_match_link($name, $namespaced_name, $text, $type, $is_link, $core_branch);
    if ($link) {
      return $prepend . $link . $append;
    }
  }

  if (!$core_branch) {
    // We do not have a core branch. See if there is an API reference core
    // branch to use, and if we can make a link there.
    $ids = array();
    foreach ($api_branches as $api_branch) {
      if ($api_branch->reference_type == 'api' &&
        ($api_branch->project_type == 'core') &&
        ($core_compatibility == $api_branch->core_compatibility)) {
        $ids[] = $api_branch->branch_id;
      }
    }

    if (count($ids)) {
      $link = _api_make_match_link($name, $namespaced_name, $text, $type, $is_link, NULL, '', $ids);
      if ($link) {
        return $prepend . $link . $append;
      }
    }
  }

  // Try to find a match at least within this same core compatibility, but
  // only if this was not a core branch.
  if (!$core_branch || $core_branch->branch_id != $branch->branch_id) {
    $link = _api_make_match_link($name, $namespaced_name, $text, $type, $is_link, NULL, $core_compatibility);
    if ($link) {
      return $prepend . $link . $append;
    }

    // Also try API reference branches with this core compatibility.
    $ids = array();
    foreach ($api_branches as $api_branch) {
      if ($api_branch->reference_type == 'api' &&
        ($core_compatibility == $api_branch->core_compatibility)) {
        $ids[] = $api_branch->branch_id;
      }
    }

    if (count($ids)) {
      $link = _api_make_match_link($name, $namespaced_name, $text, $type, $is_link, NULL, '', $ids);
      if ($link) {
        return $prepend . $link . $append;
      }
    }
  }

  // If we get here, there still wasn't a match, so return non-linked text.
  if (isset($prepend_if_not_found)) {
    $prepend = $prepend_if_not_found;
  }
  if (isset($append_if_not_found)) {
    $append = $append_if_not_found;
  }

  return $prepend . $text . $append;
}

/**
 * Finds matches for an object name in a branch and makes a link.
 *
 * Helper function for api_link_name().
 *
 * @param $name
 *   Name to match (text found in the code or documentation).
 * @param $namespaced_name
 *   Fully namespaced name to look for. Only used when $type is 'function',
 *   'function_or_constant', or 'class'. In these cases, the function tries to
 *   find matches of the namespaced name first, and if that fails, then it
 *   tries again with the plain name.
 * @param $text
 *   Text to put in the link.
 * @param $type
 *   Type of object to match (see api_link_name() for options).
 * @param $try_link
 *   If TRUE, try making links as if this is to a topic or file first.
 * @param $branch
 *   Object representing the branch to search. If NULL, use core compatibility
 *   instead.
 * @param $core_compatibility
 *   If $branch is NULL, search all branches with this core compatibility.
 * @param array $api_branch_ids
 *   If set, instead of looking in core branches, look in API reference branches
 *   that have an ID in this array.
 *
 * @return
 *   Link to either a single matching object or a search if multiple matches
 *   exist; if there are no matches, FALSE.
 */
function _api_make_match_link($name, $namespaced_name, $text, $type, $try_link = FALSE, $branch = NULL, $core_compatibility = '', $api_branch_ids = NULL) {

  if ($try_link) {
    // Before trying standard matches, see if this is a link to a group/topic.
    $link = _api_make_match_link($name, $namespaced_name, $text, 'group', FALSE, $branch, $core_compatibility, $api_branch_ids);
    if ($link) {
      return $link;
    }

    // Also see if it could be a file name being linked.
    $link = _api_make_match_link($name, $namespaced_name, $text, 'file', FALSE, $branch, $core_compatibility, $api_branch_ids);
    if ($link) {
      return $link;
    }
  }

  // Now do the standard linking tries.
  // Build a query to find the matches.
  if ($api_branch_ids) {
    // This will not work for 'yaml_string' references.
    if ($type == 'yaml_string') {
      return FALSE;
    }
    $using_api_branch = TRUE;
    $query = db_select('api_external_documentation', 'ad')
      ->fields('ad')
      ->condition('ad.branch_id', $api_branch_ids);
  }
  else {
    $using_api_branch = FALSE;
    $query = db_select('api_documentation', 'ad')
      ->fields('ad', array('did', 'branch_id', 'object_name', 'title', 'object_type', 'summary', 'file_name'));
    $query->innerJoin('api_branch', 'b', 'ad.branch_id = b.branch_id');
    $query->fields('b', array('branch_name', 'preferred', 'project'));
    if ($branch) {
      $query->condition('ad.branch_id', $branch->branch_id);
    }
    else {
      $query->condition('b.core_compatibility', $core_compatibility);
    }
  }

  // Deal with namespaces.
  if (in_array($type, array('function', 'function_or_constant', 'class'))) {
    $original_name = $name;
    $name = $namespaced_name;
    $match_name = $query->addField('ad', 'namespaced_name', 'match_name');
    $match_name_field = 'ad.namespaced_name';
    $search_name = $query->addField('ad', 'object_name', 'search_name');
    $search_name_field = 'ad.object_name';
  }
  elseif ($type != 'theme' && $type != 'file' && $type != 'yaml_string') {
    $match_name = $query->addField('ad', 'object_name', 'match_name');
    $match_name_field = 'ad.object_name';
    $search_name_field = NULL;
  }

  // Figure out what potential names we should match on.
  $potential_names = array($name);
  $prefer_shorter = FALSE;
  $prefer_earlier = FALSE;
  $search_term = NULL;

  if ($type == 'hook') {
    $potential_names = array(
      'hook_' . $name,
      'hook_entity_' . $name,
      'hook_entity_bundle_' . $name,
      'hook_field_' . $name,
      'field_default_' . $name,
      'hook_user_' . $name,
      'hook_node_' . $name,
    );
    $prefer_earlier = TRUE;
    $query->condition('ad.object_type', 'function');
    $search_term = '_' . $name;
  }
  elseif ($type == 'alter hook') {
    $potential_names = array('hook_' . $name . '_alter');
    $query->condition('ad.object_type', 'function');
  }
  elseif ($type == 'theme') {
    $potential_names = array();
    // Potential matches are the whole theme call, or with stripped off pieces
    // separated by __. And we look for template files preferably over
    // functions.
    $prefer_shorter = TRUE;
    $hook_elements = explode('__', $name);
    while (count($hook_elements) > 0) {
      $hook = implode('__', $hook_elements);
      $potential_names[] = str_replace('_', '-', $hook) . '.html.twig';
      $potential_names[] = str_replace('_', '-', $hook) . '.tpl.php';
      $potential_names[] = 'theme_' . $hook;
      array_pop($hook_elements);
    }
    // Because this needs to match theme files, match on object title (which is
    // the file base name).
    $query->condition('ad.object_type', array('file', 'function'));
    $match_name = $query->addField('ad', 'title', 'match_name');
    $match_name_field = 'ad.title';
  }
  elseif ($type == 'function') {
    $query->condition('ad.object_type', 'function');
  }
  elseif ($type == 'global') {
    $query->condition('ad.object_type', 'global');
  }
  elseif ($type == 'function_or_constant') {
    $query->condition('ad.object_type', array('function', 'constant', 'class', 'interface'));
  }
  elseif ($type == 'file') {
    // For files other than HTML type, the title is the basename of the file.
    // For HTML files, the title is taken from the HTML title element. So,
    // if we are matching in an API reference branch, try to match on the title
    // field, which is what we have. If we are matching in a regular branch,
    // join to the file table and match on the basename field.
    if ($using_api_branch) {
      $query->condition('ad.object_type', array('file'));
      $match_name = $query->addField('ad', 'title', 'match_name');
      $match_name_field = 'ad.title';
    }
    else {
      $query->leftJoin('api_file', 'af', 'ad.did = af.did');
      $match_name = $query->addField('af', 'basename', 'match_name');
      $match_name_field = 'af.basename';
    }
  }
  elseif ($type == 'constant') {
    $query->condition('ad.object_type', 'constant');
  }
  elseif ($type == 'class') {
    $query->condition('ad.object_type', array('class', 'interface'));
  }
  elseif ($type == 'group') {
    $query->condition('ad.object_type', 'group');
  }
  elseif ($type == 'yaml_string') {
    // This is a bit of a different case. Here, we have a string and we are
    // trying to see if it was defined as a top-level key in a YAML services
    // or routing file. This would be stored in the {api_reference_storage}
    // table.
    $query->innerJoin('api_reference_storage', 'ar', 'ad.did = ar.from_did');
    $match_name = $query->addField('ar', 'object_name', 'match_name');
    $query->condition('ar.object_type', 'yaml string');
    $match_name_field = 'ar.object_name';
    $search_name_field = NULL;
  }

  // Execute the query and make an array of matches, making sure to only
  // keep the highest-priority matches.
  $query->condition($match_name_field, $potential_names);

  $results = $query->execute();
  $best = array();
  $name_matched = '';
  $preferred_matched = 0;
  foreach ($results as $object) {
    // MySQL is not case-sensitive, so check the match for exact string.
    $matched = $object->match_name;
    if (!in_array($matched, $potential_names)) {
      continue;
    }

    // See if this matched name takes precedence over the previous one.
    $priority = _api_match_priority($matched, $name_matched, $prefer_shorter, $potential_names, $prefer_earlier);
    if ($priority == 0 && ($preferred_matched == $object->preferred)) {
      // Same priority: add to array.
      $best[] = $object;
    }
    elseif ($priority > 0 || ($object->preferred == 1 && $preferred_matched == 0)) {
      // Higher priority: start new array.
      $best = array($object);
      $name_matched = $matched;
      $preferred_matched = 1;
    }
  }

  if (!count($best) || ($type == 'group' && count($best) != 1)) {
    // If we didn't find anything, and we were trying to match within a
    // namespace, try to match without the namespace.
    if (in_array($type, array('function', 'function_or_constant', 'class')) && ('\\' . $original_name != $namespaced_name)) {
      return _api_make_match_link($original_name, '\\' . $original_name, $text, $type, FALSE, $branch, $core_compatibility, $api_branch_ids);
    }

    return FALSE;
  }

  // If we get here, we found one or more matches. If we found just one,
  // make a link and return.

  if (count($best) == 1) {
    $options = array(
      'attributes' => array(
        'title' => api_entity_decode($best[0]->summary),
        'class' => array('local'),
      ),
    );

    if ($using_api_branch) {
      $url = $best[0]->url;
      unset($options['attributes']['class']);
    }
    else {
      $url = api_url($best[0]);
    }

    if ($type == 'group' && $text == $name) {
      // Override with the group name if no link text was provided.
      $text = $best[0]->title;
    }
    elseif ($type == 'group') {
      // If this is a topic link and someone provided text, then it was
      // check_plained at parse time. Don't double-encode it!
      $options['html'] = TRUE;
    }
    return l($text, $url, $options);
  }

  // If we get here, we found multiple matches. So, return a link to a search
  // page.
  // @todo If we found multiple matches in the case of a multi-branch
  // search, we should probably go to a multi-branch search page, but this
  // does not exist yet. So just go to the first found branch.
  if (isset($search_name_field)) {
    $search_name = $best[0]->search_name;
  }
  else {
    $search_name = $best[0]->match_name;
  }

  $options = array(
    'attributes' => array(
      'title' => t('Multiple implementations exist.'),
      'class' => array('local'),
    ),
  );

  if ($using_api_branch) {
    $api_branches = api_get_php_branches();
    $api_branch = $api_branches[$best[0]->branch_id];
    $url = $api_branch->search_url;
    unset($options['attributes']['class']);
  }
  else {
    $url = 'api/' . $best[0]->project . '/' . $best[0]->branch_name . '/search/';
  }
  $url = $url . $search_name;

  return l($text, $url, $options);
}

/**
 * Finds matches for an object name in a PHP reference branch and makes a link.
 *
 * Helper function for api_link_name().
 *
 * @param $name
 *   Name to match (text found in the code or documentation).
 * @param $text
 *   Text to put in the link.
 *
 * @return
 *   Link to either a single matching object or a search if multiple matches
 *   exist; if there are no matches, FALSE.
 */
function _api_make_php_reference_link($name, $text) {
  $query = db_select('api_php_documentation', 'd')
    ->fields('d', array('object_name', 'documentation'));
  $b = $query->innerJoin('api_php_branch', 'b', "b.branch_id = d.branch_id");
  $query
    ->fields($b, array('data'))
    ->condition('d.object_type', 'function')
    ->condition('d.object_name', $name);
  $result = $query->execute();
  foreach ($result as $info) {
    // MySQL is not case-sensitive, so check the match for exact string.
    if ($info->object_name != $name) {
      continue;
    }

    $data = unserialize($info->data);
    $link = strtr($data['path'], array('!function' => $name));
    return l($text, $link, array('attributes' => array('title' => api_entity_decode($info->documentation), 'class' => array('php-manual'))));
  }

  // If we get here, we did not find a match in the PHP reference branches.
  return FALSE;
}

/**
 * Checks if the name found has higher or lower priority than previous match.
 *
 * Helper function for api_link_name(), to distinguish between theme functions
 * and theme templates.
 *
 * @param $current
 *   Current matched string.
 * @param $previous
 *   Previous matched string.
 * @param $prefer_shorter
 *   TRUE to prefer shorter names.
 * @param $potential_names
 *   Array of the potential names we were matching on.
 * @param $prefer_earlier
 *   TRUE to prefer earlier matches in list of potential names.
 *
 * @return
 *   1 if previous is empty or current has higher priority. 0 if they have
 *   the same priority. -1 if current has lower priority.
 */
function _api_match_priority($current, $previous, $prefer_shorter = TRUE, $potential_names, $prefer_earlier) {
  if (strlen($previous) == 0) {
    return 1;
  }
  if ($current == $previous) {
    return 0;
  }

  // Theme templates have higher priority than theme functions.
  $current_is_theme_function = (strpos($current, 'theme_') === 0);
  $current_is_theme_template = (strpos($current, '.tpl.php') === strlen($current) - 8);
  $previous_is_theme_function = (strpos($previous, 'theme_') === 0);
  $previous_is_theme_template = (strpos($previous, '.tpl.php') === strlen($previous) - 8);
  if ($current_is_theme_function && $previous_is_theme_template) {
    return -1;
  }
  if ($current_is_theme_template && $previous_is_theme_function) {
    return 1;
  }

  // Prefer the shorter item.
  if ($prefer_shorter && (strlen($current) < strlen($previous))) {
    return 1;
  }
  if ($prefer_shorter && (strlen($previous) < strlen($current))) {
    return -1;
  }

  // Prefer the earlier item.
  if ($prefer_earlier) {
    $current_index = array_search($current, $potential_names);
    $previous_index = array_search($previous, $potential_names);
    if ($current_index === FALSE) {
      return -1;
    }
    if ($previous_index === FALSE) {
      return 1;
    }
    if ($current_index < $previous_index) {
      return 1;
    }
    if ($current_index > $previous_index) {
      return -1;
    }
  }

  // All things being equal...
  return 0;
}


/**
 * Links text to an appropriate class member variable, constant, or function.
 *
 * Callback for api_split() in _api_link_documentation().
 *
 * Tries to find something to link to by looking for matches in the following
 * order:
 * - The passed-in branch.
 * - The core branch with the same core compatibility.
 * - An API reference core branch with the same core compatibility.
 * - Any other branch with the same core compatibility.
 * - Any API reference branch with the same core compatibility.
 *
 * @param $text
 *   Text matched by the regular expression.
 * @param $branch
 *   Branch object indicating which branch to make the link in.
 * @param $prepend
 *   Unused.
 * @param $append
 *   Unused.
 * @param $file_did
 *   Documentation ID of the file the code is in (for namespace information).
 *   Can omit if namespaces are not relevant.
 * @param $class_did
 *   Documentation ID of the class this is part of (if any).
 *
 * @see api_link_name()
 * @see api_link_link()
 */
function api_link_member_name($text, $branch, $prepend = '', $append = '', $file_did = NULL, $class_did = NULL) {

  // The pattern matched to get here contains the entire span with all of its
  // classes. Parse it out.
  $matches = array();
  preg_match('!<span class="(?:php-function-or-constant|php-variable) ([^"]+) member-of-([^"]+)">(\$*' . DRUPAL_PHP_FUNCTION_PATTERN . ')</span>!', $text, $matches);
  $name = $matches[3];
  $member_type = $matches[2];
  $object_type = $matches[1];

  $prepend = '<span class="php-function-or-constant">';
  $append = '</span>';

  // Strip off a $ if there is one at the start.
  if (strpos($name, '$') === 0) {
    $name = substr($name, 1);
    $prepend .= '$';
  }

  // Get some information we will need in several places below.
  $nameinfo = api_get_namespace_info($file_did);
  $core_compatibility = ($branch) ? $branch->core_compatibility : variable_get('api_default_core_compatibility', '');
  $api_branches = api_get_php_branches();

  // Convert parent references to the name of the parent class, which must be
  // in the api_reference_storage table for this branch.
  if ($member_type == 'parent') {
    if (!$class_did) {
      // We cannot do a parent reference if we do not know the class, so do
      // not make a link at all.
      return $prepend . $name . $append;
    }

    $parent_class = db_select('api_reference_storage', 'ars')
      ->condition('ars.from_did', $class_did)
      ->condition('ars.object_type', 'class')
      ->fields('ars', array('object_name'))
      ->execute()
      ->fetchField();
    if (!$parent_class) {
      return $prepend . $name . $append;
    }
    $member_type = 'class-' . $parent_class;
  }

  // Try matching in this branch.
  $link = _api_make_match_link_class_member($name, $nameinfo, $member_type, $object_type, $class_did, $file_did, $branch);
  if ($link) {
    return $prepend . $link . $append;
  }

  // If this is not a core branch, try matching in the core branch.
  $core_branch = api_find_core_branch($branch);
  if ($core_branch && $core_branch->branch_id != $branch->branch_id) {
    $link = _api_make_match_link_class_member($name, $nameinfo, $member_type, $object_type, $class_did, $file_did, $core_branch);
    if ($link) {
      return $prepend . $link . $append;
    }
  }

  if (!$core_branch) {
    // We do not have a core branch. See if there is an API reference core
    // branch to use, and if we can make a link there.
    $ids = array();
    foreach ($api_branches as $api_branch) {
      if ($api_branch->reference_type == 'api' &&
        ($api_branch->project_type == 'core') &&
        ($core_compatibility == $api_branch->core_compatibility)) {
        $ids[] = $api_branch->branch_id;
      }
    }

    if (count($ids)) {
      $link = _api_make_match_link_class_member($name, $nameinfo, $member_type, $object_type, $class_did, $file_did, NULL, '', $ids);
      if ($link) {
        return $prepend . $link . $append;
      }
    }
  }

  // Try to find a match at least within this same core compatibility, but
  // only if this was not a core branch.
  if (!$core_branch || $core_branch->branch_id != $branch->branch_id) {
    $link = _api_make_match_link_class_member($name, $nameinfo, $member_type, $object_type, $class_did, $file_did, NULL, $core_compatibility);
    if ($link) {
      return $prepend . $link . $append;
    }

    // Also try API reference branches with this core compatibility.
    $ids = array();
    foreach ($api_branches as $api_branch) {
      if ($api_branch->reference_type == 'api' &&
        ($core_compatibility == $api_branch->core_compatibility)) {
        $ids[] = $api_branch->branch_id;
      }
    }

    if (count($ids)) {
      $link = _api_make_match_link_class_member($name, $nameinfo, $member_type, $object_type, $class_did, $file_did, NULL, '', $ids);
      if ($link) {
        return $prepend . $link . $append;
      }
    }
  }

  // If we got here, we didn't have a match.
  return $prepend . $name . $append;
}

/**
 * Finds matches for a class member object name in a branch and makes a link.
 *
 * Helper function for api_link_member_name().
 *
 * @param $name
 *   Name to match (text found in the code).
 * @param $namespaced_info
 *   Namespace information for the file context
 *   (output of api_get_namespace_info()).
 * @param $member_type
 *   What type of reference to find: 'parent', 'self', 'variable', or
 *   'class-NAME'. This is set up in the parser.
 * @param $object_type
 *   'function' if this is a member function, etc.
 * @param $class_did
 *   Documentation ID of the class this is part of (if any).
 * @param $file_did
 *   Documentation ID of the file the code is in (for namespace information).
 *   Can omit if namespaces are not relevant.
 * @param $branch
 *   Object representing the branch to search. If NULL, use core compatibility
 *   instead.
 * @param $core_compatibility
 *   If $branch is NULL, search all branches with this core compatibility.
 * @param array $api_branch_ids
 *   If set, instead of looking in core branches, look in API reference branches
 *   that have an ID in this array.
 *
 * @return
 *   Link to either a single matching object or a search if multiple matches
 *   exist; if there are no matches, FALSE.
 */
function _api_make_match_link_class_member($name, $namespace_info, $member_type, $object_type, $class_did = NULL, $file_did = NULL, $branch = NULL, $core_compatibility = '', $api_branch_ids = NULL) {

  // If we're looking for a specific class, see if it exists in this branch
  // or the reference branch.
  if (strpos($member_type, 'class-') === 0) {
    $class_name = substr($member_type, 6);
    $namespaced_name = api_full_classname($class_name, $namespace_info['namespace'], $namespace_info['use_alias']);
    if ($api_branch_ids) {
      // If we are looking in a reference branch, do the query to find this
      // exact member name $class_name::$name -- we cannot do anything very
      // fancy in that case, because we do not have the full member information.
      $result = db_select('api_external_documentation', 'ad')
        ->fields('ad')
        ->condition('ad.branch_id', $api_branch_ids)
        ->condition('object_type', $object_type)
        ->condition('namespaced_name', $namespaced_name . '::' . $name)
        ->execute()
        ->fetchObject();
      if ($result) {
        $url = $result->url;
        $options = array(
          'attributes' => array(
            'title' => api_entity_decode($result->summary),
          ),
        );
        return l($name, $result->url, $options);
      }
      else {
        return FALSE;
      }
    }
    // If we get here, we are looking in a real branch or compatible branches.
    // We want to find a matching class_did, and then convert this to being a
    // "self" reference on that class.
    $query = db_select('api_documentation', 'ad');
    $query->fields('ad', array('did'))
      ->condition('object_type', 'class')
      ->condition('namespaced_name', $namespaced_name);
    if (is_null($branch)) {
      $query->innerJoin('api_branch', 'b', 'ad.branch_id = b.branch_id');
      $query->condition('b.core_compatibility', $core_compatibility);
    }
    else {
      $query->condition('ad.branch_id', $branch->branch_id);
    }
    $class_did = $query->execute()->fetchCol();
    $member_type = 'self';
    if (count($class_did) == 1) {
      $class_did = array_pop($class_did);
    }
    elseif (count($class_did) == 0) {
      // Class is not in this branch.
      return FALSE;
    }
  }

  $result = NULL;
  $using_api_branch = FALSE;

  if ($member_type == 'self') {
    // Type 'self' does not make sense for API reference branches.
    if ($api_branch_ids) {
      return FALSE;
    }

    // Looking for a member of a particular class, or one of several classes,
    // whose documentation ID we have already located. Use the {api_members}
    // table to find the right method, since it includes members inherited from
    // parent classes.
    $query = db_select('api_members', 'am');
    $ad = $query->innerJoin('api_documentation', 'ad', 'ad.did = am.did');
    $query->innerJoin('api_branch', 'b', 'ad.branch_id = b.branch_id');
    $query
      ->fields('ad', array('branch_id', 'title', 'object_name', 'summary', 'object_type', 'file_name', 'did', 'member_name'))
      ->fields('b', array('branch_name', 'project'))
      ->condition('am.class_did', $class_did)
      ->condition('ad.member_name', $name);

    if ($object_type == 'function') {
      $query->condition('ad.object_type', 'function');
    }
    else {
      $query->condition('ad.object_type', 'function', '<>');
    }

    $result = $query->execute();
  }
  elseif ($member_type == 'variable') {
    // This was some kind of a variable like $foo->member(). So match any member
    // of any class in this branch or reference branch.
    if ($api_branch_ids) {
      // Looking in API reference branches.
      $using_api_branch = TRUE;
      $query = db_select('api_external_documentation', 'ad')
        ->fields('ad')
        ->condition('ad.branch_id', $api_branch_ids)
        ->condition('ad.member_name', $name);
    }
    else {
      // Looking in regular documentation branches.
      $query = db_select('api_documentation', 'ad');
      $query->innerJoin('api_branch', 'b', 'ad.branch_id = b.branch_id');
      $query
        ->fields('ad', array('branch_id', 'title', 'object_name', 'summary', 'object_type', 'file_name', 'did', 'member_name'))
        ->fields('b', array('branch_name', 'project'))
        ->condition('ad.member_name', $name);

      if (is_null($branch)) {
        $query->condition('b.core_compatibility', $core_compatibility);
      }
      else {
        $query->condition('ad.branch_id', $branch->branch_id);
      }
    }

    if ($object_type == 'function') {
      $query->condition('ad.object_type', 'function');
    }
    else {
      $query->condition('ad.object_type', 'function', '<>');
    }

    $result = $query->execute();
  }

  // See if we have one result, more than one result, or no results.
  $matches = array();
  if (isset($result)) {
    foreach ($result as $object) {
      // MySQL is not case-sensitive, so check the match for exact string.
      if ($object->member_name != $name) {
        continue;
      }
      $matches[] = $object;
    }
  }

  // No matches.
  if (!count($matches)) {
    return FALSE;
  }

  // If we found one match, return its URL as a link.
  if (count($matches) == 1) {
    $object = $matches[0];
    $options = array(
      'attributes' => array(
        'title' => api_entity_decode($object->summary),
        'class' => array('local'),
      ),
    );

    if ($using_api_branch) {
      $url = $object->url;
      unset($options['attributes']['class']);
    }
    else {
      $url = api_url($object);
    }
    return l($name, $url, $options);
  }

  // If we found multiple matches, make a search URL.
  // @todo If we found multiple matches in the case of a multi-branch
  // search, we should probably go to a multi-branch search page, but this
  // does not exist yet. So just go to the first found branch.
  $options = array(
    'attributes' => array(
      'title' => t('Multiple implementations exist.'),
      'class' => array('local'),
    ),
  );

  if ($using_api_branch) {
    $api_branches = api_get_php_branches();
    $api_branch = $api_branches[$matches[0]->branch_id];
    $url = $api_branch->search_url . $name;
    unset($options['attributes']['class']);
  }
  else {
    $url = 'api/' . $matches[0]->project . '/' . $matches[0]->branch_name . '/search/' . $name;
  }

  return l($name, $url, $options);
}

/**
 * Decodes HTML entities.
 *
 * @param $text
 *   Text to decode.
 *
 * @return
 *   Text with all HTML entities decoded.
 */
function api_entity_decode($text) {
  $text = html_entity_decode($text);
  // html_entity_decode does not decode numeric entities, and there are
  // many cases of &#39; (quote) in here.
  $text = str_replace('&#039;', "'", $text);
  $text = str_replace('&#39;', "'", $text);
  return $text;
}

/**
 * Turns text into a link, using the first word as the object name.
 *
 * Callback for api_split() in _api_link_documentation().
 *
 * @param $name
 *   Text to link.
 * @param $branch
 *   Branch object indicating which branch to make the link in.
 * @param $prepend
 *   Text to prepend on the link.
 * @param $append
 *   Text to append on the link.
 * @param $file_did
 *   Documentation ID of the file the code is in (for namespace information).
 *   Can omit if namespaces are not relevant.
 * @param $class_did
 *   Documentation ID of the class the link is in (if any).
 *
 * @return
 *   The text as a link.
 *
 * @see api_link_member_name()
 * @see api_link_name()
 */
function api_link_link($name, $branch, $prepend = '', $append = '', $file_did = NULL, $class_did = NULL) {
  $words = preg_split('/\s+/', trim($name));
  $name = array_shift($words);
  return api_link_name($name, $branch, $prepend, $append, $file_did, $class_did, implode(' ', $words), TRUE);
}


/**
 * Loads namespace and use information for a file.
 *
 * @param int $file_did
 *   Documentation ID of the file.
 *
 * @return
 *   Associative array with elements:
 *   - namespace: Name of the namespace for this file (could be '').
 *   - use_alias: Associative array of use statement class aliases. Keys are
 *     the alias names, and values are the fully namespaced class names.
 */
function api_get_namespace_info($file_did = NULL) {
  static $cache = array();

  $ret = array('namespace' => '', 'use_alias' => array());

  if (!isset($file_did) || !$file_did) {
    return $ret;
  }

  if (isset($cache[$file_did])) {
    return $cache[$file_did];
  }

  $values = db_select('api_namespace', 'n')
    ->condition('did', $file_did)
    ->condition('class_name', '', '<>')
    ->fields('n', array('object_type', 'class_alias', 'class_name'))
    ->execute();
  foreach ($values as $info) {
    // Start namespaces with backslash.
    $name = $info->class_name;
    if (drupal_substr($name, 0, 1) != '\\') {
      $name = '\\' . $name;
    }

    if ($info->object_type == 'namespace') {
      $ret['namespace'] = $name;
    }
    else {
      $ret['use_alias'][$info->class_alias] = $name;
    }
  }

  $cache[$file_did] = $ret;

  return $ret;
}

