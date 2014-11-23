<?php

/**
 * @file
 * The PHP documentation parser that generates content for api.module.
 */

/**
 * Reads in a file and calls a callback function to parse and save it.
 *
 * @param $callback
 *   Name of the function to call to parse and save the file contents, such as
 *   api_parse_php_file(), api_parse_text_file(), or api_parse_html_file()
 *   (pass the function name as a string).
 * @param $file_path
 *   Full path to the file to read in.
 * @param $branch
 *   Object representing the branch to assign the file contents to. It is
 *   assumed to be a fresh load of this branch from the branch database table,
 *   and a valid branch.
 * @param $file_name
 *   File name to store in the database for this file.
 */
function api_parse_file($callback, $file_path, $branch, $file_name) {
  $basename = drupal_basename($file_name);
  $docblock = array(
    'object_name' => $file_name,
    'branch' => $branch,
    'object_type' => 'file',
    'file_name' => $file_name,
    'title' => $basename,
    'basename' => $basename,
    'documentation' => '',
    'references' => array(),
    'modified' => filemtime($file_path),
    'queued' => 0,
    'source' => str_replace(array("\r\n", "\r"), array("\n", "\n"), file_get_contents($file_path)),
    'content' => '',
    'class' => '',
    'namespaced_name' => '',
    'modifiers' => '',
  );

  $callback($docblock);
}

/**
 * Saves contents of a file as a single piece of text documentation.
 *
 * Callback for api_parse_file().
 *
 * @param $docblock
 *   Array from api_parse_file() containing the file contents and information
 *   about the file, branch, etc.
 */
function api_parse_text_file($docblock) {
  // See if the file contains an @file block, and use that for the
  // documentation if so; otherwise, just use the file as a whole. This is
  // probably only present for Twig files.
  $matches = array();
  if (preg_match('|/\*\*[\s\*]+@file.+\*/|Us', $docblock['source'], $matches)) {
    // Let Grammar parser and api_save_documentation() parse out this
    // documentation.
    if (!api_libraries_loaded()) {
      return;
    }
    $editor = PGPEditor::getInstance();
    $docblock['content'] = $editor->commentToString(array('value' => $matches[0]));
  }
  else {
    $docblock['documentation'] = api_format_documentation($docblock['source'], FALSE);
  }

  // We don't actually want to keep track of references for text files.
  $references = array();
  $docblock['code'] = api_format_php($docblock['source'], $references, TRUE, FALSE);

  api_save_documentation(array($docblock));
}

/**
 * Saves contents of a file as a single piece of HTML documentation.
 *
 * Escapes any HTML special characters in the text, so that it can be
 * displayed to show the HTML tags.
 *
 * Callback for api_parse_file().
 *
 * @param $docblock
 *   Array from api_parse_file() containing the file contents and information
 *   about the file, branch, etc.
 */
function api_parse_html_file($docblock) {
  $docblock['code'] = '<pre>' . htmlspecialchars($docblock['source']) . '</pre>';
  $title_match = array();
  if (preg_match('!<title>([^<]+)</title>!is', $docblock['source'], $title_match)) {
    $docblock['title'] = trim($title_match[1]);
    $docblock['summary'] = $docblock['title'];
  }
  $documentation_match = array();
  if (preg_match('!<body>(.*?</h1>)?(.*)</body>!is', $docblock['source'], $documentation_match)) {
    $docblock['documentation'] = $documentation_match[2];
  }

  api_save_documentation(array($docblock));
}

/**
 * Parses a PHP file and saves the file and its contents as documentation.
 *
 * PHP functions, classes, global variables, constants, etc. in the file
 * are saved as documentation, if they have docblocks.
 *
 * Callback for api_parse_file().
 *
 * @param $docblock
 *   Array from api_parse_file() containing the file contents and information
 *   about the file, branch, etc.
 */
function api_parse_php_file($docblock) {
  if (!api_libraries_loaded()) {
    return;
  }

  // Build grammar statements.
  $editor = PGPEditor::getInstance();
  $reader = $editor->getReader();
  $reader->setPreserveWhitespace(FALSE);
  $reader->setSnippet($docblock['source']);
  $reader->buildGrammar();

  // Retrieve items of interest.
  $statements = $reader->getStatements();
  if (!$statements) {

    // This is a text file or template file with no declarations of functions,
    // constants, etc. But we do want to track references in it.
    $docblock['code'] = api_format_php($docblock['source'], $docblock['references'], TRUE);

    api_save_documentation(array($docblock));
    // Free up memory.
    $docblock = NULL;
    $statements = NULL;
    $reader->reset();
    $reader = NULL;
    $editor = NULL;
    return;
  }

  // Reserve the first array slot for the file documentation block. Note that
  // we will only keep some of the references we parsed out.
  $references = array();
  $docblock['code'] = api_format_php($docblock['source'], $references, TRUE);
  $docblocks = array($docblock);

  // Set default documentation block array for items other than the file.
  $default_block = array(
    'branch' => $docblock['branch'],
    'file_name' => $docblock['file_name'],
    'class' => '',
    'object_type' => '',
    'documentation' => '',
    'references' => array(),
    'see' => '',
    'deprecated' => '',
    'start_line' => 0,
    'namespaced_name' => '',
    'modifiers' => '',
  );

  $found_references = api_documentation_loop($statements, $default_block, $docblocks);

  // For the file, save only the references not found in parsing the
  // statements within the file. This will only save references to function
  // calls in the global area of the file, excluding those found in
  // functions declared in the file. This doesn't apply to namespace and use
  // references though -- these are found only in api_documentation_loop().

  foreach ($found_references as $type => $list) {
    // For namespaces and use aliases, merge these into the main references.
    if ($type == 'namespace' || $type == 'use_alias') {
      $refs = array($type => $list);
      $references = _api_merge_references($references, $refs);
    }
    else {
      // For other references, remove them.
      foreach ($list as $name => $info) {
        unset($references[$type][$name]);
      }
    }
  }
  $docblocks[0]['references'] = $references;

  api_save_documentation($docblocks);

  // Free up memory.
  $references = NULL;
  $found_references = NULL;
  $docblock = NULL;
  $default_block = NULL;
  $docblocks = NULL;
  $statements = NULL;
  $reader->reset();
  $reader = NULL;
  $editor = NULL;
}

/**
 * Saves contents of a YAML file.
 *
 * Callback for api_parse_file().
 *
 * @param $docblock
 *   Array from api_parse_file() containing the file contents and information
 *   about the file, branch, etc.
 */
function api_parse_yaml_file($docblock) {
  // Just use the file name as the documentation, since the file contents
  // are not good documentation.
  $docblock['documentation'] = $docblock['file_name'];

  // Parse the YAML in the file.
  module_load_include('php', 'api', 'yaml/ExceptionInterface');
  module_load_include('php', 'api', 'yaml/RuntimeException');
  module_load_include('php', 'api', 'yaml/ParseException');
  module_load_include('php', 'api', 'yaml/DumpException');
  module_load_include('php', 'api', 'yaml/Unescaper');
  module_load_include('php', 'api', 'yaml/Inline');
  module_load_include('php', 'api', 'yaml/Parser');
  $parser = new \Symfony\Component\Yaml\Parser();
  try {
    $parsed = $parser->parse($docblock['source']);
  }
  catch (Exception $e) {
    $parsed = array();
    watchdog('api', 'YAML parsing failed for %filename with message %msg', array('%filename' => $docblock['file_name'], '%msg' => $e->getMessage()), WATCHDOG_WARNING);
  }

  // Find potential references, which are array values that look like they
  // could be callback function names.
  $references = _api_find_yaml_references($parsed);
  $docblock['references'] = array('potential callback' => $references);

  // Format the source, putting spans around the found reference strings.
  $dummy = array();
  $code = api_format_php($docblock['source'], $dummy, TRUE, FALSE);
  foreach ($references as $string) {
    $code = str_replace($string, '<span class="yaml-reference">' . $string . '</span>', $code);
  }
  $docblock['code'] = $code;

  api_save_documentation(array($docblock));
}

/**
 * Recursively finds potential references in a parsed YAML array.
 */
function _api_find_yaml_references($yaml) {
  if (empty($yaml)) {
    return array();
  }

  $yaml = (array) $yaml;
  $references = array();
  foreach ($yaml as $value) {
    if (is_string($value)) {
      $matches = array();
      if (preg_match("|^['\"]*(" . API_RE_FUNCTION_IN_TEXT . ")['\"]*$|", $value, $matches)) {
        // Special case the commonly-found TRUE and FALSE.
        if ($matches[1] != 'TRUE' && $matches[1] != 'FALSE') {
          $references[] = $matches[1];
        }
      }
    }
    elseif (is_array($value) || is_object($value)) {
      $references = array_merge($references, _api_find_yaml_references($value));
    }
  }

  return array_unique($references);
}

/**
 * Builds a list of documentation items.
 *
 * @param PGPBody $statements
 *   A PGPBody object containing body statements.
 * @param array $default_block
 *   The default documentation block to use to build documentation items.
 * @param array $docblocks
 *   The array of documentation block items. Documentation items
 *   are added to the end of this array.
 *
 * @return
 *   An array of all the references found while parsing the statements.
 */
function api_documentation_loop($statements, $default_block, &$docblocks) {
  if (!api_libraries_loaded()) {
    return array();
  }

  // Keep track of all references found.
  $all_references = array();

  static $object_types = array(
    T_ASSIGNMENT => 'property',
    T_CONST => 'constant',
  );
  $editor = PGPEditor::getInstance();

  // Traverse statement list to gather documentation items.
  $current = $statements->first();
  while ($current != NULL) {
    $statement = $current->data;
    $type = is_object($statement) ? $statement->type : $statement['type'];

    // Common processing.
    switch ($type) {
      case T_NAMESPACE:
        // $statement->name has the name of the namespace for the file.
        $namespace = $statement->name->toString();
        if (!empty($namespace)) {
          $references = array('namespace' => $namespace);
          $all_references = _api_merge_references($all_references, $references);
        }
        break;

      case T_USE:
        $references = array('use_alias' => $statement->aliasesToArray());
        $all_references = _api_merge_references($all_references, $references);
        break;

      case T_ASSIGNMENT: // Class property
        if (empty($default_block['class'])) {
          break;
        } // Explicit fallthrough.
      case T_INTERFACE:
      case T_CLASS:
      case T_FUNCTION:
      case T_DEFINE:
      case T_CONST:
      case T_GLOBAL:
        $docblock = $default_block;
        $docblock['object_type'] = isset($object_types[(int) $type]) ? $object_types[(int) $type] : $editor->statementTypeToString($statement);

        $class_prefix = empty($default_block['class']) ? '' : $default_block['class'] . '::';
        if ($type == T_GLOBAL || $type == T_ASSIGNMENT) {
          $docblock['member_name'] = preg_replace('/^\$/', '', $editor->statementOperandToText($statement));
        }
        else {
          $docblock['member_name'] = $editor->statementOperandToText($statement);
        }
        $docblock['object_name'] = $class_prefix . $docblock['member_name'];
        $docblock['title'] = $class_prefix . $editor->statementOperandToText($statement);
        $docblock['start_line'] = $current->line;
        $docblock['content'] = $editor->commentToString($statement->comment);
        $statement->comment = '';
        $references = array();
        $docblock['code'] = api_format_php("<?php\n" . $statement->toString() . "\n?" . ">", $references, FALSE, ($statement->type == T_FUNCTION));

        if (isset($statement->modifiers) && !$statement->modifiers->isEmpty()) {
          $docblock['modifiers'] = $statement->modifiers->toString();
        }

        if ($statement->type == T_FUNCTION) {
          $docblock['signature'] = $editor->functionGetSignature($statement);
          $docblock['references'] = $references;
          $all_references = _api_merge_references($all_references, $references);
        }
        elseif ($statement->type == T_CLASS || $statement->type == T_INTERFACE) {
          $docblock['extends'] = is_object($statement->extends) ? explode(', ', $statement->extends->toString()) : array();
          $docblock['implements'] = is_object($statement->implements) ? explode(', ', $statement->implements->toString()) : array();
        }
        $docblocks[] = $docblock;
        break;

      case T_DOC_COMMENT:
        $docblock = $default_block;
        $docblock['content'] = $editor->commentToString($statement);
        $docblock['start_line'] = $current->line;
        $docblocks[] = $docblock;
        break;
    }

    // Additional recursive processing on statements with bodies.
    switch ($type) {
      case T_INTERFACE:
      case T_CLASS:
        if (!empty($statement->body)) {
          $references = api_documentation_loop($statement->body, array_merge($default_block, array('class' => $docblock['object_name'])), $docblocks);
          $all_references = _api_merge_references($all_references, $references);
        }
        break;
    }
    $current = $current->next;
  }

  return $all_references;
}

/**
 * Merges references, with warnings for duplicate namespaces.
 *
 * @param array $master
 *   Master list of references.
 * @param array $new
 *   New references to merge in.
 *
 * @return array
 *   Merged references. References in $new are appended to references in
 *   $master, and if there are duplicate namespace or use references, the
 *   $master list is used and a warning is generated.
 */
function _api_merge_references($master, $new) {

  // We're supporting only one namespace declaration per file.
  if (isset($master['namespace']) && isset($new['namespace'])) {
    if ($master['namespace'] != $new['namespace']) {
      watchdog('api', 'Multiple namespace declarations found in file. Only first is used.', array(), WATCHDOG_WARNING);
    }
    unset($new['namespace']);
  }

  // We're supporting only non-conflicting use declarations.
  if (isset($new['use_alias']) && isset($master['use_alias'])) {
    foreach ($new['use_alias'] as $alias => $class) {
      if (isset($master['use_alias'][$alias]) && $master['use_alias'][$alias] != $class) {
        watchdog('api', 'Conflicting use declarations for %name found in file. Only first is used.', array('%name' => $usealias), WATCHDOG_WARNING);
        unset($new['use_alias'][$alias]);
      }
    }
  }

  return array_merge_recursive($master, $new);
}

/**
 * Saves documentation information to the database.
 *
 * @param $docblocks
 *   An array containing information about API documentation items. The first
 *   array element should be for the file as a whole, and subsequent elements
 *   should have information for items declared in that file (if any).
 */
function api_save_documentation($docblocks) {
  $nested_groups = array();
  $namespace = '';
  $use_aliases = array();

  // Make a list of the documentation IDs that were contained in this file
  // the last time through. We'll remove any items that are not in this file
  // any more after we get through the $docblocks array.
  $old_dids = db_select('api_documentation', 'd')
    ->fields('d', array('did'))
    ->condition('branch_id', $docblocks[0]['branch']->branch_id)
    ->condition('file_name', $docblocks[0]['file_name'])
    ->execute()
    ->fetchCol();

  $dids = array();
  $class_dids = array();
  $function_insert_query = NULL;
  $functions_to_delete = array();

  // Look for @file block first so $docblocks[0] gets filled in before it is processed.
  foreach ($docblocks as $docblock) {
    if (preg_match('/' . API_RE_TAG_START . 'file/', $docblock['content'])) {
      $content = $docblock['content'];
      // Remove @file tag from this docblock.
      $content = str_replace('@file', '', $content);

      // If this docblock contains @mainpage or @defgroup, this will cause
      // problems, because we won't have a @file doc block any more -- it will
      // be co-opted, and then the site will be screwed up. So, remove these
      // tags and save a watchdog message.

      if (preg_match('/' . API_RE_TAG_START . 'mainpage/', $content) ||
        preg_match('/' . API_RE_TAG_START . 'defgroup/', $content)) {

        $content = str_replace('@mainpage', '', $content);
        $content = str_replace('@defgroup', '', $content);

        watchdog('api', '@file docblock containing @defgroup or @mainpage in %file at line %line. Extraneous tags ignored.', array('%file' => $docblocks[0]['file_name'], '%line' => $docblock['start_line']), WATCHDOG_WARNING);
      }

      $docblocks[0]['content'] = $content;
      break;
    }
  }

  foreach ($docblocks as $docblock) {
    // Keep track of the namespace and add it to all docblocks for this file.
    if (isset($docblock['references']['namespace']) && !empty($docblock['references']['namespace'])) {
      $namespace = $docblock['references']['namespace'];
    }
    $docblock['namespace'] = $namespace;

    // Keep track of the use aliases so we can put the right classes into the
    // extends/implements references for classes we encounter.
    if (isset($docblock['references']['use_alias'])) {
      $use_aliases = array_merge($docblock['references']['use_alias'], $use_aliases);
    }


    // Change @Annotation to @ingroup annotation.
    $docblock['content'] = preg_replace('/' . API_RE_TAG_START . 'Annotation' . API_RE_WORD_BOUNDARY_END . '/', "\n@ingroup annotation\n", $docblock['content']);

    if (preg_match('/' . API_RE_TAG_START . 'mainpage/', $docblock['content'])) {
      preg_match('/' . API_RE_TAG_START . 'mainpage (.*?)\n/', $docblock['content'], $mainpage_matches);
      $docblock['title'] = $mainpage_matches[1];
      $docblock['content'] = preg_replace('/' . API_RE_TAG_START . 'mainpage.*?\n/', '', $docblock['content']);
      $docblock['object_type'] = 'mainpage';
      $docblock['object_name'] = $docblocks[0]['branch']->branch_name;
    }
    elseif (preg_match('/' . API_RE_TAG_START . 'defgroup/', $docblock['content'])) {
      if (preg_match('/' . API_RE_TAG_START . 'defgroup ([a-zA-Z0-9_.-]+) +(.*?)\n/', $docblock['content'], $group_matches)) {
        $group_name = $group_matches[1];
        // See if the group already exists in another file in this branch, and
        // if so, treat this as an @addtogroup.
        $did = db_select('api_documentation', 'd')
          ->condition('object_name', $group_name)
          ->condition('object_type', 'group')
          ->condition('file_name', $docblock['file_name'], '<>')
          ->condition('branch_id', $docblocks[0]['branch']->branch_id)
          ->fields('d', array('did'))
          ->execute()
          ->fetchField();
        if ($did > 0) {
          $docblock['content'] = str_replace('defgroup', 'addtogroup', $docblock['content']);
          watchdog('api', 'Duplicate @defgroup in %file at line %line treated as @addtogroup.', array('%file' => $docblocks[0]['file_name'], '%line' => $docblock['start_line']), WATCHDOG_WARNING);

        }
        else {
          $docblock['object_name'] = $group_name;
          $docblock['title'] = $group_matches[2];
          $docblock['content'] = preg_replace('/' . API_RE_TAG_START . 'defgroup.*?\n/', '', $docblock['content']);
          $docblock['object_type'] = 'group';
        }
      }
      else {
        watchdog('api', 'Malformed @defgroup in %file at line %line.', array('%file' => $docblocks[0]['file_name'], '%line' => $docblock['start_line']), WATCHDOG_WARNING);
      }
    }

    // Determine group membership.
    $match = array();
    if (preg_match_all('/' . API_RE_TAG_START . '(ingroup|addtogroup) ([a-zA-Z0-9_.-]+)/', $docblock['content'], $match)) {
      $docblock['groups'] = $match[2];
      $docblock['content'] = preg_replace('/' . API_RE_TAG_START . '(ingroup|addtogroup).*?\n/', '', $docblock['content']);
    }

    // Handle nested groups.
    if (!isset($nested_groups[$docblock['class']])) {
      $nested_groups[$docblock['class']] = array();
    }
    foreach ($nested_groups[$docblock['class']] as $group_id) {
      if (!empty($group_id)) {
        $docblock['groups'][] = $group_id;
      }
    }
    if (preg_match('/' . API_RE_TAG_START . '{/', $docblock['content'])) {
      if ($docblock['object_type'] === 'group') {
        array_push($nested_groups[$docblock['class']], $docblock['object_name']);
      }
      elseif (isset($docblock['groups'])) {
        array_push($nested_groups[$docblock['class']], reset($docblock['groups']));
      }
      else {
        array_push($nested_groups[$docblock['class']], '');
      }
    }
    if (preg_match('/' . API_RE_TAG_START . '}/', $docblock['content'])) {
      array_pop($nested_groups[$docblock['class']]);
    }

    // At this point, we might have been dealing with a "block" that is
    // just an @} or an object with no name, or something like that. We needed
    // to do the processing above, but we don't want to save this as an object.
    if (empty($docblock['object_type']) || empty($docblock['object_name'])) {
      continue;
    }

    // Treat {@inheritdoc} as a blank doc block, meaning inherit from parent.
    $docblock['content'] = str_replace('{@inheritdoc}', '', $docblock['content']);

    if ($docblock['content'] && trim($docblock['content'])) {
      // Find parameter definitions.
      $params = '';
      while (preg_match('/' . API_RE_TAG_START . 'param\s(.*?)(?=\n' . API_RE_TAG_START . '|$)/s', $docblock['content'], $param_match)) {
        $docblock['content'] = str_replace($param_match[0], '', $docblock['content']);
        // Add some formatting to the parameter -- strong tag for everything
        // that was on the @param line, and a colon after. Note that tags
        // are stripped out below, so we use [strong] and then fix it later.
        $this_param = $param_match[1];
        $this_param = preg_replace('|^([^\n]+)|', '[strong]$1[/strong]:', $this_param);
        $params .= "\n\n" . $this_param;
      }
      // Format and then replace our fake tags with real ones.
      $params = api_format_documentation($params);
      $params = str_replace('[strong]', '<strong>', $params);
      $params = str_replace('[/strong]', '</strong>', $params);
      $docblock['parameters'] = $params;

      // Find return value definitions.
      $docblock['return_value'] = '';
      preg_match_all('/' . API_RE_TAG_START . 'return\s(.*?)(?=\n' . API_RE_TAG_START . '|$)/s', $docblock['content'], $return_matches, PREG_SET_ORDER);
      foreach ($return_matches as $return_match) {
        $docblock['content'] = str_replace($return_match[0], '', $docblock['content']);
        $docblock['return_value'] .= "\n\n" . $return_match[1];
      }
      $docblock['return_value'] = api_format_documentation($docblock['return_value']);

      // Find @see lines.
      $docblock['see'] = '';
      while (preg_match('/' . API_RE_TAG_START . 'see\s(.*?)\n/s', $docblock['content'], $match)) {
        $docblock['content'] = str_replace($match[0], '', $docblock['content']);
        $docblock['see'] .= "\n\n" . $match[1];
      }
      $docblock['see'] = api_format_documentation($docblock['see']);

      // Find @var, a class or variable type name.
      $docblock['var'] = '';
      if (preg_match('/' . API_RE_TAG_START . 'var\s(.*?)\n/s', $docblock['content'], $match)) {
        $docblock['content'] = str_replace($match[0], '', $docblock['content']);
        $docblock['var'] = trim($match[1]);
      }

      // Find @throws, a paragraph (could be more than one).
      $docblock['throws'] = '';
      while (preg_match('/' . API_RE_TAG_START . 'throws\s(.*?)(?=\n' . API_RE_TAG_START . '|$)/s', $docblock['content'], $match)) {
        $docblock['content'] = str_replace($match[0], '', $docblock['content']);
        $docblock['throws'] .= "\n\n" . $match[1];
      }
      $docblock['throws'] = api_format_documentation($docblock['throws']);

      // Find @deprecated, a paragraph (could be more than one).
      $docblock['deprecated'] = '';
      while (preg_match('/' . API_RE_TAG_START . 'deprecated\s(.*?)(?=\n' . API_RE_TAG_START . '|$)/s', $docblock['content'], $match)) {
        $docblock['content'] = str_replace($match[0], '', $docblock['content']);
        $docblock['deprecated'] .= "\n\n" . $match[1];
      }
      $docblock['deprecated'] = api_format_documentation($docblock['deprecated']);

      // Format everything remaining as the main documentation.
      $docblock['documentation'] = api_format_documentation($docblock['content']);
    }
    // Grab the first line as a summary, unless already provided.
    if (!isset($docblock['summary'])) {
      $docblock['summary'] = api_documentation_summary($docblock['documentation']);
    }

    if (!empty($docblock['class'])) {
      $docblock['class_did'] = $class_dids[$docblock['class']];
    }

    // Figure out the namespaced name.
    $docblock['namespaced_name'] = api_full_classname($docblock['object_name'], $namespace, $use_aliases);

    // See if this docblock already existed, and get its ID.
    $docblock['did'] = db_select('api_documentation', 'd')
      ->fields('d', array('did'))
      ->condition('object_name', $docblock['object_name'])
      ->condition('file_name', $docblock['file_name'])
      ->condition('object_type', $docblock['object_type'])
      ->condition('branch_id', $docblocks[0]['branch']->branch_id)
      ->execute()
      ->fetchField();

    if ($docblock['did'] > 0) {
      _api_add_text_defaults($docblock, 'api_documentation');
      drupal_write_record('api_documentation', $docblock, 'did');
    }
    else {
      $nid = api_new_documentation_node();
      $docblock['did'] = $nid;
      $docblock['branch_id'] = $docblocks[0]['branch']->branch_id;
      _api_add_text_defaults($docblock, 'api_documentation');
      drupal_write_record('api_documentation', $docblock);
    }

    // Keep track of class membership
    if ($docblock['object_type'] === 'class' || $docblock['object_type'] === 'interface') {
      $class_dids[$docblock['object_name']] = $docblock['did'];
    }

    // Delete all overrides information we had for this item. We will compute
    // it again in api_shutdown().
    db_delete('api_overrides')
      ->condition(db_or()
        ->condition('did', $docblock['did'])
        ->condition('overrides_did', $docblock['did'])
      )
      ->execute();

    switch ($docblock['object_type']) {
      case 'function':
        _api_add_text_defaults($docblock, 'api_function');
        // Combine all the deletes/inserts for {api_function} into one query
        // each so that they will run faster, since we don't need this table
        // to be updated during the processing of this file.
        $functions_to_delete[] = $docblock['did'];
        if (is_null($function_insert_query)) {
          $function_insert_query = db_insert('api_function')
            ->fields(array('did', 'signature', 'parameters', 'return_value'));
        }
        $function_insert_query->values(array(
            'did' => $docblock['did'],
            'signature' => $docblock['signature'],
            'parameters' => $docblock['parameters'],
            'return_value' => $docblock['return_value'],
          ));
        api_replace_references($docblocks[0]['branch'], $docblock['did'], $docblock['object_name'], $docblock['references'], FALSE, $namespace, $use_aliases);
        break;

      case 'file':
        // Note that this will set the 'queued' field back to zero, since
        // $docblock['queued'] is set to 0 up at the top of api_parse_file().
        drupal_write_record('api_file', $docblock, 'did');
        api_replace_references($docblock['branch'], $docblock['did'], $docblock['object_name'], $docblock['references'], TRUE, $namespace, $use_aliases);
        break;

      case 'interface':
      case 'class':
        db_delete('api_reference_storage')
          ->condition('branch_id', $docblocks[0]['branch']->branch_id)
          ->condition('object_type', array('class', 'interface'))
          ->condition('from_did', $docblock['did'])
          ->execute();
        foreach ($docblock['extends'] as $extend) {
          $refname = api_full_classname($extend, $namespace, $use_aliases);
          api_reference($docblocks[0]['branch'], 'class', $refname, $docblock['did']);
        }
        foreach ($docblock['implements'] as $implement) {
          $refname = api_full_classname($implement, $namespace, $use_aliases);
          api_reference($docblocks[0]['branch'], 'interface', $refname, $docblock['did']);
        }
        break;
    }

    db_delete('api_reference_storage')
      ->condition('branch_id', $docblocks[0]['branch']->branch_id)
      ->condition('object_type', 'group')
      ->condition('from_did', $docblock['did'])
      ->execute();

    if (isset($docblock['groups'])) {
      foreach ($docblock['groups'] as $group_name) {
        api_reference($docblocks[0]['branch'], 'group', $group_name, $docblock['did']);
      }
    }

    $dids[] = $docblock['did'];
  }

  // Run the queued-up queries on the api_function table.
  if (count($functions_to_delete) > 0) {
    db_delete('api_function')
      ->condition('did', $functions_to_delete)
      ->execute();
  }
  if (!is_null($function_insert_query)) {
    $function_insert_query->execute();
  }

  // Clean out all of the doc objects from this file that no longer exist.
  $old_dids = array_diff($old_dids, $dids);
  api_delete_items($old_dids, FALSE);

  api_schedule_shutdown();
}

/**
 * Saves PHP branch documentation information to the database.
 *
 * @param $docblocks
 *   An array containing information about functions found in the branch.
 */
function api_save_php_documentation($docblocks) {
  // Remove all the old items from this branch. We don't care about preserving
  // IDs, so there is no reason to use complicated logic and update existing
  // records.
  db_delete('api_php_documentation')
    ->condition('branch_id', $docblocks[0]['branch_id'])
    ->execute();

  $did = api_new_php_doc_id();
  $count = 0;
  foreach ($docblocks as $docblock) {
    if (!$count) {
      // Build up queries and save the documentation in chunks, for speed.
      $insertQuery = db_insert('api_php_documentation')
        ->fields(array('did', 'branch_id', 'object_name', 'member_name', 'object_type', 'documentation'));
    }
    $docblock['did'] = $did;
    $insertQuery->values($docblock);
    $did++;
    $count++;
    if ($count >= 500) {
      // Using a larger chunk size could risk running into database placeholder
      // limits.
      $insertQuery->execute();
      $count = 0;
    }
  }

  if ($count > 0) {
    $insertQuery->execute();
  }
}

/**
 * Saves API branch documentation information to the database.
 *
 * @param $docblocks
 *   An array containing information about functions found in the branch.
 */
function api_save_api_documentation($docblocks) {
  // Remove all the old items from this branch. We don't care about preserving
  // IDs, so there is no reason to use complicated logic and update existing
  // records.
  db_delete('api_external_documentation')
    ->condition('branch_id', $docblocks[0]['branch_id'])
    ->execute();

  $did = api_new_external_doc_id();
  $count = 0;
  foreach ($docblocks as $docblock) {
    if (!$count) {
      // Build up queries and save the documentation in chunks, for speed.
      $insertQuery = db_insert('api_external_documentation')
        ->fields(array('did', 'branch_id', 'object_name', 'namespaced_name', 'title', 'member_name', 'summary', 'object_type', 'url'));
    }
    $docblock['did'] = $did;
    $insertQuery->values($docblock);
    $did++;
    $count++;
    if ($count >= 500) {
      // Using a larger chunk size could risk running into database placeholder
      // limits.
      $insertQuery->execute();
      $count = 0;
    }
  }

  if ($count > 0) {
    $insertQuery->execute();
  }
}

/**
 * Returns a new ID for a PHP doc item.
 */
function api_new_php_doc_id() {
  $select = db_select('api_php_documentation', 'p');
  $select->addExpression('MAX(did)', 'a');
  $ids = $select->groupBy('branch_id')
    ->execute()
    ->fetchCol();

  if (count($ids)) {
    return max($ids) + 1;
  }

  return 1;
}

/**
 * Returns a new ID for an API reference branch doc item.
 */
function api_new_external_doc_id() {
  $select = db_select('api_external_documentation', 'p');
  $select->addExpression('MAX(did)', 'a');
  $ids = $select->groupBy('branch_id')
    ->execute()
    ->fetchCol();

  if (count($ids)) {
    return max($ids) + 1;
  }

  return 1;
}

/**
 * Replaces any existing references for function/hook calls with new ones.
 *
 * @param $branch
 *   Branch object for the item making the calls.
 * @param $did
 *   Documentation ID of the item making the calls.
 * @param $name
 *   Name of the item making the calls.
 * @param $references
 *   Array of references set up by api_format_php().
 * @param $do_namespaces
 *   TRUE to save namespace references. FALSE (default) to ignore them.
 * @param $namespace
 *   Namespace for the file these references are from.
 * @param $use_aliases
 *   Use aliases for the file these references are from.
 */
function api_replace_references($branch, $did, $name, $references, $do_namespaces = FALSE, $namespace, $use_aliases) {
  // Find the name without class prefix, if any.
  $shortname = $name;
  if (($pos = strpos($name, '::')) !== FALSE && $pos > 1) {
    $shortname = substr($name, $pos + 2);
  }

  // Remove any existing references.
  $types_to_remove =  array('function', 'potential hook', 'potential theme', 'potential alter', 'potential callback', 'member-parent', 'member-self', 'member', 'member-class');

  db_delete('api_reference_storage')
    ->condition('branch_id', $branch->branch_id)
    ->condition('object_type', $types_to_remove)
    ->condition('from_did', $did)
    ->execute();

  if ($do_namespaces) {
    $types_to_remove = array('namespace', 'use_alias');
    db_delete('api_namespace')
      ->condition('did', $did)
      ->condition('object_type', $types_to_remove)
      ->execute();
  }

  // Add the new references.
  foreach ($references as $type => $list) {
    switch ($type) {
      case 'namespace':
        if ($do_namespaces) {
          // In this case, $list is actually a string containing the namespace
          // for a file, not a list.
          api_namespace($did, $type, '', $list, TRUE);
        }
        break;

      case 'use_alias':
        if ($do_namespaces) {
          // Save all of the use aliases.
          foreach ($list as $alias => $class) {
            api_namespace($did, $type, $alias, $class, TRUE);
          }
        }
        break;

      case 'member-class':
        // Don't save a reference to the item itself.
        unset($list[$shortname]);
        foreach ($list as $call) {
          // These are references to ClassName::method(). Make sure
          // they are fully namespaced.
          $call = api_full_classname($call, $namespace, $use_aliases);
          api_reference($branch, $type, $call, $did, TRUE);
        }

        break;

      case 'function':
      case 'potential callback':
      case 'member-self':
        // Don't save a reference to the item itself.
        unset($list[$shortname]);

        // Intentional fallthrough here!

      default:
        foreach ($list as $call) {
          api_reference($branch, $type, $call, $did, TRUE);
        }
    }
  }

  // Cause all the queued references to get saved.
  api_reference(TRUE);
  if ($do_namespaces) {
    api_namespace(TRUE);
  }
}

/**
 * Formats a documentation block as HTML.
 *
 * First escapes all HTML tags. Then processes links and code blocks, and
 * converts newlines into paragraphs.
 *
 * @param $documentation
 *   Documentation string to format.
 * @param $auto_p
 *   TRUE (default) to convert to paragraphs. FALSE to skip this conversion and
 *   put the documentation in PRE tags.
 *
 * @return
 *   Formatted documentation.
 */
function api_format_documentation($documentation, $auto_p = TRUE) {
  // Don't do processing on empty text (so we don't end up with empty paragraphs).
  if (empty($documentation)) {
    return '';
  }

  // Check for invalid Unicode, which screws everything up.
  if (!mb_check_encoding($documentation, 'UTF-8')) {
    return t('Non-displayable characters.');
  }
  // Replace 4-byte characters not supported by MySQL's utf8 encoding
  $documentation = preg_replace('/[^\x{0000}-\x{FFFF}]/u', '�', $documentation);

  $documentation = check_plain($documentation);

  // @link full URLs.
  $documentation = preg_replace('/' . API_RE_TAG_START . 'link ((http:\/\/|https:\/\/|ftp:\/\/|mailto:|smb:\/\/|afp:\/\/|file:\/\/|gopher:\/\/|news:\/\/|ssl:\/\/|sslv2:\/\/|sslv3:\/\/|tls:\/\/|tcp:\/\/|udp:\/\/)([a-zA-Z0-9@:%_+*~#?&=.,\/;-]*[a-zA-Z0-9@:%_+*~#&=\/;-])) (.*?) ' . API_RE_TAG_START . 'endlink/', '<a href="$1">$4</a>', $documentation);
  // Site URLs.
  $documentation = preg_replace('/' . API_RE_TAG_START . 'link \/([a-zA-Z0-9_\/-]+) (.*?) ' . API_RE_TAG_START . 'endlink/', str_replace('%24', '$', l('$2', '$1')), $documentation);

  // Process sections.
  $regexp = '/^' . API_RE_TAG_START . 'section ([a-zA-Z0-9_-]+) (.*)$/m';
  preg_match_all($regexp, $documentation, $section_matches, PREG_SET_ORDER);
  if (!empty($section_matches)) {
    $documentation = preg_replace($regexp, '<h3 id="$1">$2</h3>', $documentation);
  }

  // Process sub-sections.
  $regexp = '/^' . API_RE_TAG_START . 'subsection ([a-zA-Z0-9_-]+) (.*)$/m';
  preg_match_all($regexp, $documentation, $subsection_matches, PREG_SET_ORDER);
  if (!empty($subsection_matches)) {
    $documentation = preg_replace($regexp, '<h4 id="$1">$2</h4>', $documentation);
  }

  // Process in-page references to sections/subsections.
  if (!empty($section_matches) || !empty($subsection_matches)) {
    $search = array();
    $replace = array();
    foreach (array_merge($section_matches, $subsection_matches) as $match) {
      array_shift($match);
      $id = array_shift($match);
      $caption = trim(array_shift($match));
      $search[] = '/' . API_RE_TAG_START . 'ref ' . $id . '/';
      // Note that we cannot use l() here to make the link -- it doesn't have
      // a way to make a self-link, since url('') is a synonym for front page.
      // Also note that the ID here is already constrained to legal characters
      // by the regexp above.
      $replace[] = '<a href="#' . $id . '">' . check_plain($caption) . '</a>';
    }
    $documentation = preg_replace($search, $replace, $documentation);
  }

  // Replace left over curly braces
  $documentation = preg_replace('/' . API_RE_TAG_START . '[{}]/', '', $documentation);

  // Change @Plugin and other annotation sections into @code. They have to be
  // at the very end of the documentation block.
  $documentation = preg_replace('/' . API_RE_TAG_START . '(' . API_RE_FUNCTION_CHARACTERS . '\(.*\))$/s', '<h3>' . t('Plugin annotation') . '</h3>' . "\n@code\n@" . '$1' . "\n@endcode", $documentation);

  // Process the @code @endcode tags.
  $documentation = preg_replace_callback('/' . API_RE_TAG_START . 'code(.+?)' . API_RE_TAG_START . 'endcode/s', 'api_format_embedded_php', $documentation);

  // Convert newlines into paragraphs.
  if ($auto_p) {
    $documentation = api_autop($documentation);
  }
  else {
    $documentation = '<pre class="api-text">' . $documentation . '</pre>';
  }

  return $documentation;
}

/**
 * Converts newlines into paragraphs.
 *
 * Like _filter_autop(), but does not add <br /> tags.
 *
 * @param $text
 *   Text to convert.
 *
 * @return
 *   Converted text.
 */
function api_autop($text) {
  // All block level tags
  $block = '(?:table|thead|tfoot|caption|colgroup|tbody|tr|td|th|div|dl|dd|dt|ul|ol|li|pre|select|form|blockquote|address|p|h[1-6]|hr)';

  // Split at <pre>, <script>, <style> and </pre>, </script>, </style> tags.
  // We don't apply any processing to the contents of these tags to avoid messing
  // up code. We look for matched pairs and allow basic nesting. For example:
  // "processed <pre> ignored <script> ignored </script> ignored </pre> processed"
  $chunks = preg_split('@(</?(?:pre|script|style|object)[^>]*>)@i', $text, -1, PREG_SPLIT_DELIM_CAPTURE);
  // Note: PHP ensures the array consists of alternating delimiters and literals
  // and begins and ends with a literal (inserting NULL as required).
  $ignore = FALSE;
  $ignoretag = '';
  $output = '';
  foreach ($chunks as $i => $chunk) {
    if ($i % 2) {
      // Opening or closing tag?
      $open = ($chunk[1] != '/');
      list($tag) = preg_split('/[ >]/', substr($chunk, 2 - $open), 2);
      if (!$ignore) {
        if ($open) {
          $ignore = TRUE;
          $ignoretag = $tag;
        }
      }
      // Only allow a matching tag to close it.
      elseif (!$open && $ignoretag == $tag) {
        $ignore = FALSE;
        $ignoretag = '';
      }
    }
    elseif (!$ignore) {
      $chunk = api_format_documentation_lists($chunk);
      $chunk = preg_replace('|\n*$|', '', $chunk) . "\n\n"; // just to make things a little easier, pad the end
      $chunk = preg_replace('|<br />\s*<br />|', "\n\n", $chunk);
      $chunk = preg_replace('!(<' . $block . '[^>]*>)!', "\n$1", $chunk); // Space things out a little
      $chunk = preg_replace('!(</' . $block . '>)!', "$1\n\n", $chunk); // Space things out a little
      $chunk = preg_replace("/\n\n+/", "\n\n", $chunk); // take care of duplicates
      $chunk = preg_replace('/\n?(.+?)(?:\n\s*\n|\z)/s', "<p>$1</p>\n", $chunk); // make paragraphs, including one at the end
      $chunk = preg_replace('|<p>\s*</p>\n|', '', $chunk); // under certain strange conditions it could create a P of entirely whitespace
      $chunk = preg_replace("|<p>(<li.+?)</p>|", "$1", $chunk); // problem with nested lists
      $chunk = preg_replace('|<p><blockquote([^>]*)>|i', "<blockquote$1><p>", $chunk);
      $chunk = str_replace('</blockquote></p>', '</p></blockquote>', $chunk);
      $chunk = preg_replace('!<p>\s*(</?' . $block . '[^>]*>)!', "$1", $chunk);
      $chunk = preg_replace('!(</?' . $block . '[^>]*>)\s*</p>!', "$1", $chunk);
      $chunk = preg_replace('/&([^#])(?![A-Za-z0-9]{1,8};)/', '&amp;$1', $chunk);
    }
    $output .= $chunk;
  }
  return $output;
}

/**
 * Regular expression callback for \@code tags in api_format_documentation().
 */
function api_format_embedded_php($matches) {
  $references = array();
  return "\n" . api_format_php("<?php" . decode_entities($matches[1]) . "?" . ">", $references, FALSE, FALSE) . "\n";
}

/**
 * Formats documentation lists as HTML lists.
 *
 * Parses a block of text for lists that uses hyphens or asterisks as bullets,
 * and format the lists as proper HTML lists.
 *
 * @param $documentation
 *   Documentation string to format.
 *
 * @return
 *   $documentation with lists formatted.
 */
function api_format_documentation_lists($documentation) {
  $lines = explode("\n", $documentation);
  $output = '';
  $bullet_indents = array(-1);

  foreach ($lines as $line) {
    preg_match('!^( *)([*-] )?(.*)$!', $line, $matches);
    $indent = strlen($matches[1]);
    $bullet_exists = $matches[2];
    $is_start = FALSE;

    if ($indent < $bullet_indents[0]) {
      // First close off any lists that have completed.
      while ($indent < $bullet_indents[0]) {
        array_shift($bullet_indents);
        $output .= '</li></ul>';
      }
    }

    if ($indent == $bullet_indents[0]) {
      if ($bullet_exists) {
        // A new bullet at the same indent means a new list item.
        $output .= '</li><li>';
        $is_start = TRUE;
      }
      else {
        // If the indent is the same, but there is no bullet, that also
        // signifies the end of the list.
        array_shift($bullet_indents);
        $output .= '</li></ul>';
      }
    }

    if ($indent > $bullet_indents[0] && $bullet_exists) {
      // A new list at a lower level.
      array_unshift($bullet_indents, $indent);
      $output .= '<ul><li>';
      $is_start = TRUE;
    }

    // At the start of a bullet, if there is a ":" followed by a space, put
    // everything before the : in bold.
    if ($is_start && (($p = strpos($matches[3], ': ')) > 0)) {
      $matches[3] = '<strong>' . substr($matches[3], 0, $p) . '</strong>' .
        substr($matches[3], $p);
    }
    $output .= $matches[3] . "\n";
  }

  // Clean up any unclosed lists.
  array_pop($bullet_indents);
  foreach ($bullet_indents as $indent) {
    $output .= '</li></ul>';
  }

  // To make sure that api_autop() doesn't get confused, remove newlines
  // immediately before </li> tags.
  $output = str_replace("\n</li>", "</li>", $output);

  return $output;
}

/**
 * Retrieves a summary from a documentation block.
 *
 * @param $documentation
 *   Documentation block to find the summary of. Should be pre-formatted into
 *   paragraphs.
 *
 * @return
 *   First paragraph of the documentation block, stripped of tags, and
 *   truncated to 255 characers.
 */
function api_documentation_summary($documentation) {
  $pos = strpos($documentation, '</p>');
  if ($pos !== FALSE) {
    $documentation = substr($documentation, 0, $pos);
  }
  $documentation = trim(strip_tags($documentation));

  if (strlen($documentation) > 255) {
    return substr($documentation, 0, strrpos(substr($documentation, 0, 252), ' ')) . '…';
  }
  else {
    return $documentation;
  }
}

/**
 * Colorizes and formats PHP code, and finds references in it.
 *
 * @param $code
 *   PHP code to format. Note that any actual PHP code needs to be enclosed in
 *   PHP start/end tags to be formatted correctly.
 * @param $references
 *   Array to return references in, if $track_references is TRUE. References
 *   are hook invocations, function calls, etc. $references is an associative
 *   array where the keys are the types of references ('function',
 *   'potential hook', etc.), and the values are arrays of the names of this
 *   type that were found.
 * @param $number
 *   FALSE to not number the lines (default). TRUE to number the lines, or an
 *   integer to start numbering at that line number.
 * @param $track_references
 *   TRUE (default) to track references, FALSE to skip reference tracking.
 *
 * @return
 *   HTML-formatted code, with spans enclosing various PHP elements.
 */
function api_format_php($code, &$references, $number = FALSE, $track_references = TRUE) {

  // Check for invalid Unicode, which screws everything up.
  if (!mb_check_encoding($code, 'UTF-8')) {
    return t('Non-displayable characters.');
  }
  // Replace 4-byte characters not supported by MySQL's utf8 encoding
  $code = preg_replace('/[^\x{0000}-\x{FFFF}]/u', '�', $code);

  $output = '';

  if (!defined('T_ML_COMMENT')) {
    define('T_ML_COMMENT', T_COMMENT);
  }
  if (!defined('T_DOC_COMMENT')) {
    define('T_DOC_COMMENT', T_COMMENT);
  }

  // Set up to track references, if we are tracking them.
  if ($track_references) {
    $references = array(
      'function' => array(),
      'potential hook' => array(),
      'potential theme' => array(),
      'potential alter' => array(),
      'potential callback' => array(),
      'member-parent' => array(),
      'member-self' => array(),
      'member' => array(),
      'member-class' => array(),
    );
  }

  // Keep track of:
  // - Are we in a double-quoted string?
  // - Are we in the parameters for a hook or theme invocation?
  // - How many PHP start/end tags we found (will remove at end if there is
  //   only one set).
  // - Class members: $foo->bar or Foo::bar.
  // - Are we building up a namespaced class name?

  // Double-quoted strings.
  $in_string = FALSE;

  // Hook/theme invokes.
  $invoke_type = '';
  $invoke_function_names = array(
    '_field_invoke',
    '_field_invoke_default',
    '_field_invoke_multiple',
    '_field_invoke_multiple_default',
    'bootstrap_invoke_all',
    'entity_invoke_bundle_hook',
    'getImplementations',
    'implementsHook',
    'invoke',
    'invokeAll',
    'invokeHook',
    'module_hook',
    'module_implements',
    'module_invoke',
    'module_invoke_all',
    'node_invoke',
    'user_module_invoke',
  );
  $paren_count = 0;
  $target_paren_count = 0;

  // PHP open/close tag counts and string size.
  $open_count = 0;
  $close_count = 0;
  $open_size = 0;
  $close_size = 0;

  // Class members.
  $in_member = FALSE;
  $member_of = '';

  // Namespaces.
  $in_namespace = FALSE;
  $full_namespaced_name = '';

  // Split the code into tokens.
  $tokens = token_get_all($code);
  foreach ($tokens as $array_index => $token) {
    $close_size = 0;

    // Take care of double-quoted strings.
    if ($in_string) {
      $in_member = FALSE;
      $member_of = '';
      $in_namespace = FALSE;
      $full_namespaced_name = '';
      if ($token == '"') {
        $output .= '"</span>';
        $in_string = FALSE;
      }
      else {
        $output .= is_array($token) ? htmlspecialchars($token[1]) : htmlspecialchars($token);
      }
      continue;
    }
    elseif ($token == '"') {
      $output .= '<span class="php-string">"';
      $in_string = TRUE;
      $in_member = FALSE;
      $member_of = '';
      $in_namespace = FALSE;
      $full_namespaced_name = '';
      continue;
    }

    if (is_array($token)) {
      $type = $token[0];
      $value = htmlspecialchars($token[1]);

      switch ($type) {
        case T_OPEN_TAG:
          $in_member = FALSE;
          $member_of = '';
          $in_namespace = FALSE;
          $full_namespaced_name = '';
          $to_add = '<span class="php-boundry">' . $value . '</span>';
          $open_count++;
          if (strlen($output) == 0) {
            // This is at the start of $output, so we tentatively want
            // to remove it.
            $open_size = strlen($to_add);
          }

          $output .= $to_add;
          break;

        case T_CLOSE_TAG:
          $in_member = FALSE;
          $member_of = '';
          $in_namespace = FALSE;
          $full_namespaced_name = '';
          $to_add = '<span class="php-boundry">' . $value . '</span>';
          $close_count++;
          // Save the size of the closing tag here. If we didn't encounter
          // any more tokens after this, it will remain (this is reset to zero
          // each time through the loop).
          $close_size = strlen($to_add);
          $output .= $to_add;
          break;

        case T_COMMENT:
        case T_ML_COMMENT:
        case T_DOC_COMMENT:
          $in_member = FALSE;
          $member_of = '';
          $in_namespace = FALSE;
          $full_namespaced_name = '';
          $output .= '<span class="php-comment">' . $value . '</span>';
          break;

        case T_VARIABLE:
          $in_member = FALSE;
          $member_of = $value;
          $in_namespace = FALSE;
          $full_namespaced_name = '';
          $output .= '<span class="php-variable">' . $value . '</span>';
          break;

        case T_CONSTANT_ENCAPSED_STRING:
          $in_member = FALSE;
          $member_of = '';
          $in_namespace = FALSE;
          $full_namespaced_name = '';
          // This is a string like 'foo'. If we're in a call of some sort,
          // then this is a potential function, hook, etc. name.
          $class = 'php-string';
          if ($invoke_type != '' && preg_match("|^'" . API_RE_FUNCTION_CHARACTERS . "'$|", $value)) {
            $class .= ' potential-' . $invoke_type;
            if ($track_references) {
              $newname = str_replace("'", '', $value);
              $references['potential ' . $invoke_type][$newname] = $newname;
            }
          }
          if ($track_references && preg_match("|^'" . DRUPAL_PHP_FUNCTION_PATTERN . "'$|", $value)) {
            $newname = str_replace("'", '', $value);
            $references['potential callback'][$newname] = $newname;
          }

          $output .= '<span class="' . $class . '">' . $value . '</span>';
          break;

        case T_INLINE_HTML:
          $in_member = FALSE;
          $member_of = '';
          $in_namespace = FALSE;
          $full_namespaced_name = '';
          $output .= '<span class="php-string">' . $value . '</span>';
          break;

        case T_STRING:
          // This is a function, constant, etc. being declared or used.

          // See if the next token is a backslash. In which case, add to the
          // name so far and continue on.
          if (isset($tokens[$array_index + 1]) && is_array($tokens[$array_index + 1]) && $tokens[$array_index + 1][0] == T_NS_SEPARATOR) {
            $in_namespace = TRUE;
            $full_namespaced_name .= $value;
            break;
          }
          else {
            // No more namespace tokens are coming. Prepend any namespace
            // we have already encountered and reset for the next one.
            if ($in_namespace) {
              $value = $full_namespaced_name . $value;
            }
            $full_namespaced_name = '';
            $in_namespace = FALSE;
          }

          $class = 'php-function-or-constant';
          $reference_type = 'function';
          $reference = $value;

          // See if this is a function call or not, by looking ahead to the
          // next token and seeing if it is an open paren.
          $is_function = (isset($tokens[$array_index + 1]) && !is_array($tokens[$array_index + 1]) && $tokens[$array_index + 1] == '(');

          if ($in_member) {
            if ($is_function) {
              $class .= ' function';
            }
            else {
              $class .= ' property';
            }
            if ($member_of == 'parent') {
              $class .= ' member-of-parent';
              $reference_type = 'member-parent';
            }
            elseif ($member_of == '$this' || $member_of == 'self' || $member_of == 'static') {
              $class .= ' member-of-self';
              $reference_type = 'member-self';
            }
            elseif (strpos($member_of, '$') === 0) {
              $class .= ' member-of-variable';
              $reference_type = 'member';
            }
            elseif (strlen($member_of) > 0) {
              $class .= ' member-of-class-' . $member_of;
              $reference_type = 'member-class';
              $reference = $member_of . '::' . $value;
            }
            else {
              $class .= ' member-of-variable';
              $reference_type = 'member';
            }
          }

          $output .= '<span class="' . $class . '">' . $value . '</span>';

          // Save references -- only for functions.
          if ($track_references && $is_function) {
            $references[$reference_type][$value] = $reference;
          }

          // Update $in_member/$member_of status.
          if ($in_member) {
            // If we were already in a member chain, clear $member_of, because
            // we won't know what the next type is if -> comes next.
            $member_of = '';
          }
          else {
            // We were not already in a member chain. So save this keyword
            // in case it is the start of a chain.
            $member_of = $value;
          }
          $in_member = FALSE;

          // Save $invoke_type state.
          if ($value == 'theme') {
            $invoke_type = 'theme';
            $target_paren_count = $paren_count + 1;
          }
          elseif (in_array($value, $invoke_function_names)) {
            $invoke_type = 'hook';
            $target_paren_count = $paren_count + 1;
          }
          elseif (in_array($value, array('drupal_alter', 'alter'))) {
            $invoke_type = 'alter';
            $target_paren_count = $paren_count + 1;
          }

          break;

        case T_NS_SEPARATOR:
          $in_namespace = TRUE;
          $full_namespaced_name .= $value;
          $in_member = FALSE;
          $member_of = '';
          break;

        case T_LNUMBER:
        case T_DNUMBER:
          $in_member = FALSE;
          $member_of = '';
          $in_namespace = FALSE;
          $full_namespaced_name = '';
          $output .= '<span class="php-constant">' . $value . '</span>';
          break;

        case T_ARRAY_CAST:
        case T_ARRAY:
        case T_AS:
        case T_BOOL_CAST:
        case T_BREAK:
        case T_CASE:
        case T_CLASS:
        case T_CONST:
        case T_CONTINUE:
        case T_DECLARE:
        case T_DEFAULT:
        case T_DO:
        case T_DOUBLE_CAST:
        case T_ECHO:
        case T_ELSE:
        case T_ELSEIF:
        case T_EMPTY:
        case T_ENDDECLARE:
        case T_ENDFOR:
        case T_ENDFOREACH:
        case T_ENDIF:
        case T_ENDSWITCH:
        case T_ENDWHILE:
        case T_EVAL:
        case T_EXIT:
        case T_EXTENDS:
        case T_FOR:
        case T_FOREACH:
        case T_FUNCTION:
        case T_GLOBAL:
        case T_IF:
        case T_INCLUDE_ONCE:
        case T_INCLUDE:
        case T_INT_CAST:
        case T_ISSET:
        case T_LIST:
        case T_NAMESPACE:
        case T_NEW:
        case T_OBJECT_CAST:
        case T_PRINT:
        case T_REQUIRE_ONCE:
        case T_REQUIRE:
        case T_RETURN:
        case T_STATIC:
        case T_STRING_CAST:
        case T_SWITCH:
        case T_UNSET_CAST:
        case T_UNSET:
        case T_USE:
        case T_VAR:
        case T_WHILE:
          $in_member = FALSE;
          $member_of = '';
          $in_namespace = FALSE;
          $full_namespaced_name = '';
          $output .= '<span class="php-keyword">' . $value . '</span>';
          break;

        case T_OBJECT_OPERATOR:
        case T_DOUBLE_COLON:
          $output .= $value;
          $in_member = TRUE;
          $in_namespace = FALSE;
          $full_namespaced_name = '';
          break;

        default:
          $in_member = FALSE;
          $member_of = '';
          $in_namespace = FALSE;
          $full_namespaced_name = '';
          $output .= $value;
      }
    }
    else {
      // Non-array (simple) tokens.
      $in_member = FALSE;
      $member_of = '';
      $in_namespace = FALSE;
      $full_namespaced_name = '';

      // Keep track of parentheses for whether we are in a hook/theme/alter
      // call.
      if ($token == '(') {
        $paren_count++;
      }
      elseif ($token == ')') {
        $paren_count--;
        // See if we've reached the end of this function call.
        if ($paren_count < $target_paren_count) {
          $invoke_type = '';
        }
      }

      $output .= $token;
    }
  }

  // If this block of code started and ended with PHP open/close tags and
  // didn't have any additional open/close inside, remove the open/close tags.
  if ($open_size > 0 && $close_size > 0 && $open_count == 1 && $close_count == 1) {
    $output = substr($output, $open_size, strlen($output) - $open_size - $close_size);
  }

  // Add line numbering, if requested.
  if ($number !== FALSE) {
    $start = (is_int($number)) ? $number : 1;
    $lines = explode("\n", $output);
    $output = '<ol class="code-lines" start="' . $number . '">';
    foreach ($lines as $line) {
      $output .= '<li>' . $line . "\n</li>";
    }
    $output .= '</ol>';
  }

  $output = '<pre class="php"><code>' . $output . '</code></pre>';
  return $output;
}

/**
 * Adds a reference to the {api_reference_storage} table.
 *
 * Since we may parse a file containing a reference before we have parsed the
 * file containing the referenced object, we keep track of the references
 * using a scratch table.
 *
 * @param $branch
 *   Object representing the branch the reference is in, or TRUE to execute
 *   all of the saved-up inserts.
 * @param $to_type
 *   Type of object being referenced.
 * @param $to_name
 *   Name of object being referenced.
 * @param $from_did
 *   Documentation ID of the object that references this object.
 * @param $wait
 *   TRUE to save the insert until api_reference(TRUE) is called, or FALSE
 *   (default) to do the query immediately.
 */
function api_reference($branch, $to_type = '', $to_name = '', $from_did = '', $wait = FALSE) {
  static $is_php_function = array();
  static $query_stored = NULL;

  // Is it time to do all the saved queries?
  if ($branch === TRUE) {
    if (!is_null($query_stored)) {
      $query_stored->execute();
      $query_stored = NULL;
    }
    return;
  }

  // Don't make references to built-in PHP functions.
  if ($to_type == 'function' && !isset($is_php_function[$to_name])) {
    $is_php_function[$to_name] = db_select('api_php_documentation', 'd')
      ->fields('d', array('did'))
      ->condition('d.object_name', $to_name)
      ->execute()
      ->fetchField();
  }

  if ($to_type != 'function' || !$is_php_function[$to_name]) {
    if ($wait) {
      if (is_null($query_stored)) {
        $query_stored = db_insert('api_reference_storage')
          ->fields(array('object_name', 'branch_id', 'object_type', 'from_did'));
      }
      $query_stored->values(array(
          'object_name' => $to_name,
          'branch_id' => $branch->branch_id,
          'object_type' => $to_type,
          'from_did' => $from_did,
        ));
    }
    else {
      db_insert('api_reference_storage')
        ->fields(array(
            'object_name' => $to_name,
            'branch_id' => $branch->branch_id,
            'object_type' => $to_type,
            'from_did' => $from_did,
          ))
        ->execute();
    }
  }
}

/**
 * Adds a reference to the {api_namespace} table.
 *
 * @param $file_did
 *   Documentation ID of the file for this namespace.
 * @param $type
 *   Type of reference ('namespace' or 'use_alias').
 * @param $alias
 *   Alias for the class, if $type is 'use_alias'.
 * @param $class
 *   Full class name for use aliases; namespace name for namespaces.
 * @param $wait
 *   TRUE to save the insert until api_namespace(TRUE) is called, or FALSE
 *   (default) to do the query immediately.
 */
function api_namespace($file_did, $type = '', $alias = '', $class = '', $wait = FALSE) {
  static $query_stored = NULL;

  // Is it time to do all the saved queries?
  if ($file_did === TRUE) {
    if (!is_null($query_stored)) {
      $query_stored->execute();
      $query_stored = NULL;
    }
    return;
  }

  if ($wait) {
    if (is_null($query_stored)) {
      $query_stored = db_insert('api_namespace')
        ->fields(array('did', 'object_type', 'class_alias', 'class_name'));
      }
      $query_stored->values(array(
          'did' => $file_did,
          'object_type' => $type,
          'class_alias' => $alias,
          'class_name' => $class,
        ));
  }
  else {
    db_insert('api_namespace')
      ->fields(array(
          'did' => $file_did,
          'object_type' => $type,
          'class_alias' => $alias,
          'class_name' => $class,
        ))
      ->execute();
  }
}

/**
 * Registers a shutdown function for cron, making sure to do it just once.
 *
 * @see api_shutdown()
 */
function api_schedule_shutdown() {
  static $scheduled = FALSE;

  if (!$scheduled) {
    drupal_register_shutdown_function('api_shutdown');
    $scheduled = TRUE;
  }
}

/**
 * Cleans up at the end of the cron job.
 *
 * Updates the collected references, updates the JSON object list, and clears
 * the cache.
 */
function api_shutdown() {
  // We will be re-computing this below.
  db_delete('api_reference_storage')
    ->condition('object_type', 'computed-member')
    ->execute();

  // Figure out which class member documentation objects do not have entries in
  // the {api_overrides} table. These entries are deleted during
  // api_save_documentation(), so that they can be recomputed here for any
  // class members that were updated.
  $query = db_select('api_documentation', 'ad')
    ->fields('ad', array('did', 'object_type', 'member_name', 'class_did', 'documentation', 'throws', 'var', 'code'));
  $ao = $query->leftJoin('api_overrides', 'ao', 'ao.did = ad.did');
  $af = $query->leftJoin('api_function', 'af', 'af.did = ad.did');
  $query->fields('af', array('parameters', 'return_value'))
    ->condition('ad.class_did', 0, '<>')
    ->isNull('ao.did');
  $result = $query->execute();

  // Compute overrides (i.e., if a class member is an override/implementation of
  // another class/interface's member) for each of the resulting objects, and
  // keep track of which classes were changed along the way.
  $changed_classes = array();
  $overrides_query = NULL;
  foreach ($result as $object) {
    $changed_classes[$object->class_did] = TRUE;
    $override = array(
      'did' => $object->did,
      'overrides_did' => 0,
      'documented_did' => api_has_documentation($object) ? $object->did : 0,
    );
    $overrides_did = 0;

    // Go up the parent stack and see if this member overrides a parent member.
    $classes_to_check = array($object->class_did);
    $classes_checked = $classes_to_check;
    while ($class_did = array_shift($classes_to_check)) {
      // If we have al the information we need here, stop this loop.
      if ($override['overrides_did'] > 0 && $override['documented_did'] > 0) {
        break;
      }

      // Avoid loops.
      $classes_checked[] = $class_did;

      // Add parents of this class to the class parent stack, but avoid loops.
      $query = db_select('api_reference_storage', 'ars');
      $query->innerJoin('api_documentation', 'pd', 'ars.object_name = pd.namespaced_name AND ars.branch_id = pd.branch_id');
      $query
        ->condition('ars.object_type', array('class', 'interface'))
        ->condition('pd.object_type', array('class', 'interface'))
        ->condition('ars.from_did', $class_did)
        ->fields('pd', array('did'));
      $parents = $query->execute()->fetchCol();
      foreach ($parents as $did) {
        if (!in_array($did, $classes_checked)) {
          $classes_to_check[] = $did;
        }
      }

      // Skipping the object's own class, see if this class has any members
      // with the same name/type.
      if ($class_did != $object->class_did) {
        $query = db_select('api_documentation', 'd')
          ->condition('class_did', $class_did)
          ->fields('d', array('did', 'summary', 'documentation', 'see', 'deprecated', 'throws', 'var', 'class_did'))
          ->condition('d.object_type', $object->object_type)
          ->condition('d.member_name', $object->member_name);
        $query->leftJoin('api_function', 'af', 'af.did = d.did');
        $query->fields('af', array('parameters', 'return_value'));
        $result_members = $query->execute();
        foreach ($result_members as $parent_member) {
          if (!is_null($parent_member->did)) {
            // This parent class/interface has a matching member name.
            if ($override['overrides_did'] === 0) {
              // This is the first matching member name we've encountered, so
              // this is the member that $object is overriding.
              $override['overrides_did'] = $parent_member->did;
            }
            if ($override['documented_did'] === 0 && api_has_documentation($parent_member)) {
              // $object didn't have documentation, but $parent_member does, so
              // inherit it.
              $override['documented_did'] = $parent_member->did;
              $object->summary = $parent_member->summary;
              drupal_write_record('api_documentation', $object, 'did');
            }
          }
        }
      }
    }

    // We are not using the overrides table during this loop, so do all the
    // inserts together.
    if (is_null($overrides_query)) {
      $overrides_query = db_insert('api_overrides')
        ->fields(array('did', 'overrides_did', 'documented_did'));
    }
    $overrides_query->values($override);
  }

  // Run the saved query to save the overrides.
  if (!is_null($overrides_query)) {
    $overrides_query->execute();
  }

  // Compute and save member list for each changed class/interface. Members can
  // be on the class/interface itself, or can be inherited from a parent
  // class/interface.
  $members_query = NULL;
  $members_delete = array();
  $processed_classes = array();
  while (list($class_did, ) = each($changed_classes)) {
    // Avoid loops.
    $processed_classes[] = $class_did;

    // Add current class's implementing/extending classes to the list, because
    // they need to be recomputed too. But avoid loops.
    $query = db_select('api_reference_storage', 'ars')
      ->fields('ars', array('from_did'));
    $doc = $query->innerJoin('api_documentation', 'ad', 'ars.object_name = ad.namespaced_name AND ars.object_type = ad.object_type AND ars.branch_id = ad.branch_id');
    $result_children = $query
      ->condition('ad.did', $class_did)
      ->condition('ars.from_did', 0, '<>')
      ->condition('ars.object_type', array('class', 'interface'))
      ->execute()
      ->fetchCol();
    foreach ($result_children as $new_class) {
      if (!in_array($new_class, $processed_classes)) {
        $changed_classes[$new_class] = TRUE;
      }
    }

    // Walk up the tree to find members. Since methods, properties, and
    // constants can share names, do each separately.
    $all_members = array(
      'function' => array(''),
      'property' => array(''),
      'constant' => array(''),
    );
    $parents = array($class_did);
    $processed_parents = array();
    while ($parent = array_shift($parents)) {
      // Avoid loops.
      $processed_parents[] = $parent;

      // Add this class's parents to the list, avoiding loops.
      $query = db_select('api_reference_storage', 'ars');
      $doc = $query->innerJoin('api_documentation', 'ad', 'ars.object_name = ad.namespaced_name AND ars.branch_id = ad.branch_id');
      $result_parents = $query
        ->condition('ars.object_type', array('class', 'interface'))
        ->condition('ad.object_type', array('class', 'interface'))
        ->fields($doc, array('did'))
        ->condition('from_did', $parent)
        ->execute()
        ->fetchCol();
      foreach ($result_parents as $new_class) {
        if (!in_array($new_class, $processed_parents)) {
          $parents[] = $new_class;
        }
      }

      // Find members of each type on this parent that haven't yet been added
      // to the list.
      foreach ($all_members as $type => $members) {
        $result_members = db_select('api_documentation', 'd')
          ->fields('d', array('did', 'member_name'))
          ->condition('class_did', $parent)
          ->condition('object_type', $type)
          ->condition('member_name', array_keys($members), 'NOT IN')
          ->execute();
        foreach ($result_members as $member) {
          $members[$member->member_name] = $member->did;
        }
        $all_members[$type] = $members;
      }
    }

    // We don't use the {api_members} table in this loop, so store up what
    // we need to do and do it at the end.
    $members_delete[] = $class_did;
    foreach ($all_members as $type => $members) {
      array_shift($members);
      foreach ($members as $did) {
        if (is_null($members_query)) {
          $members_query = db_insert('api_members')
            ->fields(array('class_did', 'did'));
        }
        $members_query->values(array(
          'class_did' => $class_did,
          'did' => $did,
          ));
      }
    }
  }

  // Run the saved queries for {api_members} table.
  if (count($members_delete)) {
    db_delete('api_members')
      ->condition('class_did', $members_delete)
      ->execute();
  }
  if (!is_null($members_query)) {
    $members_query->execute();
  }

  // Compute the parent classes for each changed class. Exclude interfaces,
  // since they can have multiple inheritance.
  if (count($changed_classes)) {
    $todo = array_keys($changed_classes);
    db_delete('api_extends')
      ->condition('did', $todo)
      ->execute();

    $select = db_select('api_reference_storage', 'r')
      ->condition('r.object_type', 'class')
      ->condition('r.from_did', $todo);
    $select->innerJoin('api_documentation', 'd', 'd.branch_id = r.branch_id AND d.namespaced_name = r.object_name');
    $select->condition('d.object_type', 'class');
    $select->leftJoin('api_documentation', 'd2', 'd2.branch_id = r.branch_id AND d2.namespaced_name = r.object_name AND d2.did <> d.did');
    $select->isNull('d2.did');
    $select->addField('r', 'from_did', 'did');
    $select->addField('d', 'did', 'parent_did');
    db_insert('api_extends')
      ->fields(array('did', 'parent_did'))
      ->from($select)
      ->execute();
  }

  // Figure out the member-self references in reference storage. Basically,
  // ChildClass::foo() calls $this->bar(). We need to figure out the full name
  // of $this->bar(), which could be ChildClass::bar() or
  // SomeParentClass::bar(). The {api_methods} table keeps track of this.

  $select = db_select('api_reference_storage', 'r')
    ->condition('r.object_type', 'member-self');
  $d = $select->innerJoin('api_documentation', 'd', 'r.from_did = d.did');
  $m = $select->innerJoin('api_members', 'm', 'd.class_did = m.class_did');
  $dm = $select->innerJoin('api_documentation', 'dm', 'm.did = dm.did AND dm.member_name = r.object_name');
  $select->condition('dm.object_type', 'function');
  $select->addField($dm, 'namespaced_name', 'object_name');
  $select->addField('r', 'branch_id', 'branch_id');
  $select->addField('r', 'from_did', 'from_did');
  // Note: SelectQuery adds expressions to the query at the end of the field
  // list.
  $select->addExpression("'computed-member'", 'object_type');

  db_insert('api_reference_storage')
    ->fields(array('object_name', 'branch_id', 'from_did', 'object_type'))
    ->from($select)
    ->execute();

  // Do the same for member-parent references, where ChildClass::foo() calls
  // parent::bar() -- figure out the full name of the parent member.

  $select = db_select('api_reference_storage', 'r')
    ->condition('r.object_type', 'member-parent');
  $calling_d = $select->innerJoin('api_documentation', 'cd', 'r.from_did = cd.did');
  $parent = $select->innerJoin('api_extends', 'e', 'e.did = cd.class_did');
  $parent_m = $select->innerJoin('api_members', 'pm', 'pm.class_did = e.parent_did');
  $member_d = $select->innerJoin('api_documentation', 'md', 'md.did = pm.did AND md.member_name = r.object_name');
  $select->condition('md.object_type', 'function');

  $select->addField($member_d, 'namespaced_name', 'object_name');
  $select->addField('r', 'branch_id', 'branch_id');
  $select->addField('r', 'from_did', 'from_did');
  // Note: SelectQuery adds expressions to the query at the end of the field
  // list.
  $select->addExpression("'computed-member'", 'object_type');

  db_insert('api_reference_storage')
    ->fields(array('object_name', 'branch_id', 'from_did', 'object_type'))
    ->from($select)
    ->execute();

  cache_clear_all();
}

/**
 * Determines whether an object contains any documentation.
 *
 * @param $object
 *   An object to test to see if it contains documentation.
 *
 * @return
 *   TRUE if the object has any of the following non-empty properties:
 *   - documentation
 *   - parameters
 *   - return_value
 *   - see
 *   - throws
 *   - var
 */
function api_has_documentation($object) {
  foreach (array('documentation', 'parameters', 'return_value', 'see', 'deprecated', 'throws', 'var') as $member) {
    if (!empty($object->$member)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Checks all branches to see if it's time to update, and queues if so.
 *
 * @see api_update_branch_php()
 * @see api_update_branch_api()
 * @see api_update_branch_files()
 */
function api_update_all_branches() {
  $queue = DrupalQueue::get('api_branch_update');

  // Check the files branches to see what needs updates.
  foreach (api_get_branches(TRUE) as $branch) {
    if (api_should_queue($branch)) {
      // Add to queue and mark as queued.
      $queue->createItem(array('type' => 'files', 'branch_id' => $branch->branch_id));
      $branch->queued = time();
      drupal_write_record('api_branch', $branch, 'branch_id');
    }
    else {
      watchdog('api', 'Skipped update of %project branch %branch', array('%branch' => $branch->branch_name, '%project' => $branch->project));
    }
  }

  // Check the reference branches to see what needs updates.
  foreach (api_get_php_branches(TRUE) as $branch) {
    if (api_should_queue($branch)) {
      // Add to queue and mark as queued.
      $queue->createItem(array('type' => $branch->reference_type, 'branch_id' => $branch->branch_id));
      $branch->queued = time();
      drupal_write_record('api_php_branch', $branch, 'branch_id');
    }
    else {
      watchdog('api', 'Skipped update of reference branch %branch', array('%branch' => $branch->title));
    }
  }
}

/**
 * Updates a PHP branch.
 *
 * Queries the branch URL to get an updated list of functions, and saves each
 * function in the database.
 *
 * @param $branch
 *   Object representing a PHP branch to update.
 *
 * @return
 *   TRUE if the branch was updated, and FALSE if there was an error.
 *
 * @see api_update_all_branches()
 */
function api_update_branch_php($branch) {
  $response = drupal_http_request($branch->summary);
  if ($response->code === '200') {
    $docblocks = array();
    preg_match_all('!^[a-zA-Z0-9_]* (' . DRUPAL_PHP_FUNCTION_PATTERN . ')\(.*\n.*$!m', $response->data, $function_matches, PREG_SET_ORDER);
    foreach ($function_matches as $function_match) {
      $docblocks[] = array(
        'branch_id' => $branch->branch_id,
        'object_type' => 'function',
        'object_name' => $function_match[1],
        'documentation' => $function_match[0],
        'member_name' => '',
      );
    }

    if (count($docblocks)) {
      api_save_php_documentation($docblocks);
    }

    // Mark the branch as having been updated.
    $branch->last_updated = time();
    $branch->queued = 0;
    drupal_write_record('api_php_branch', $branch, array('branch_id'));
    watchdog('api', 'Updated PHP branch %branch_name.', array('%branch_name' => $branch->title));
    return TRUE;
  }

  // We didn't get a good response.
  $branch->queued = 0;
  drupal_write_record('api_php_branch', $branch, array('branch_id'));
  watchdog('api', 'Bad HTTP response from PHP branch %branch_name.', array('%branch_name' => $branch->title));
  return FALSE;
}

/**
 * Updates an API branch.
 *
 * Queries the branch URL to get an updated item dump, and saves each item in
 * the database.
 *
 * @param $branch
 *   Object representing an API branch to update.
 *
 * @return
 *   TRUE if the branch was updated, and FALSE if there was an error.
 *
 * @see api_update_all_branches()
 */
function api_update_branch_api($branch) {
  $response = drupal_http_request($branch->url);
  if ($response->code === '200') {
    $data = drupal_json_decode($response->data);
    $docblocks = array();
    foreach ($data as $item) {
      $item = (array) $item;
      $item['branch_id'] = $branch->branch_id;
      $docblocks[] = $item;
    }

    if (count($docblocks)) {
      api_save_api_documentation($docblocks);
    }

    // Mark the branch as having been updated.
    $branch->last_updated = time();
    $branch->queued = 0;
    drupal_write_record('api_php_branch', $branch, array('branch_id'));
    watchdog('api', 'Updated API branch %branch_name.', array('%branch_name' => $branch->title));
    return TRUE;
  }

  // We didn't get a good response.
  $branch->queued = 0;
  drupal_write_record('api_php_branch', $branch, array('branch_id'));
  watchdog('api', 'Bad HTTP response from API branch %branch_name.', array('%branch_name' => $branch->title));
  return FALSE;
}

/**
 * Checks to see if we need to queue a branch for updates or not.
 *
 * @param $branch
 *   Object representing the branch.
 *
 * @return
 *   TRUE if we should queue this branch, and FALSE if not.
 */
function api_should_queue($branch) {
  if (isset($branch->queued) && $branch->queued) {
    // It is already queued up.
    return FALSE;
  }

  $last_run = isset($branch->last_updated) ? $branch->last_updated : 0;
  $now = time();
  $add = 0;
  if (isset($branch->update_frequency)) {
    $add = $branch->update_frequency;
  }
  if ($last_run + $add > $now) {
    // It hasn't been long enough yet.
    return FALSE;
  }

  return TRUE;
}

/**
 * Returns the list of parsing functions for file extensions.
 *
 * @return
 *   An associative array whose keys are file extensions, and whose values are
 *   the functions used to parse files with that extension.
 */
function api_parse_functions() {
  // @todo This could use a hook to let other modules extend this array?
  return array(
    'php' => 'api_parse_php_file',
    'module' => 'api_parse_php_file',
    'inc' => 'api_parse_php_file',
    'install' => 'api_parse_php_file',
    'engine' => 'api_parse_php_file',
    'theme' => 'api_parse_php_file',
    'profile' => 'api_parse_php_file',
    'test' => 'api_parse_php_file',

    'yml' => 'api_parse_yaml_file',

    'txt' => 'api_parse_text_file',
    'info' => 'api_parse_text_file',
    'css' => 'api_parse_text_file',
    'sh' => 'api_parse_text_file',
    'twig' => 'api_parse_text_file',

    'htm' => 'api_parse_html_file',
    'html' => 'api_parse_html_file',
  );
}

/**
 * Updates a files branch.
 *
 * Checks the current directories included in the branch to make an updated
 * list of files. Removes documentation from files that no longer exist, adds
 * documentation from new files, and updates documentation for any files that
 * have changed.
 *
 * @param $branch
 *   Object representing a files branch to update.
 *
 * @return
 *   TRUE to indicate that the branch was updated.
 *
 * @see api_update_all_branches()
 */
function api_update_branch_files($branch) {
  $files_to_scan = api_scan_directories($branch->directories, $branch->excluded_directories, $branch->exclude_files_regexp);
  if (!count($files_to_scan)) {
    watchdog('api', 'No files were found in %project branch %branch', array('%branch' => $branch->branch_name, '%project' => $branch->project), WATCHDOG_ERROR);
    $branch->queued = 0;
    drupal_write_record('api_branch', $branch, array('branch_id'));
    return FALSE;
  }

  $parse_functions = api_parse_functions();

  // List all documented files for the branch.
  $files = array();
  $query = db_select('api_documentation', 'd')
    ->fields('d', array('object_name'));
  $f = $query->innerJoin('api_file', 'f', 'd.did = f.did');
  $query->fields($f, array('did', 'modified', 'queued'))
    ->condition('d.branch_id', $branch->branch_id)
    ->condition('d.object_type', 'file');
  $result = $query->execute();
  foreach ($result as $file) {
    $files[$file->object_name] = $file;
  }

  foreach ($files_to_scan as $path => $file_name) {
    preg_match('!\.([a-z]*)$!', $file_name, $matches);
    if (isset($matches[1]) && isset($parse_functions[$matches[1]])) {
      $new = FALSE;
      if (isset($files[$file_name])) {
        $parse = (filemtime($path) > $files[$file_name]->modified) && !$files[$file_name]->queued;
        $file = $files[$file_name];
        unset($files[$file_name]); // All remaining files will be removed.
      }
      else { // New file.
        $parse = TRUE;
        $new = TRUE;
      }
      if ($parse) {
        $queue = DrupalQueue::get('api_parse');
        $queue->createItem(array('parser' => $parse_functions[$matches[1]], 'path' => $path, 'branch' => $branch, 'file' => $file_name, 'branch_name' => $branch->branch_name, 'timestamp' => time()));

        // Make sure this doesn't get added to the parse queue again.

        if ($new) {
          // Add stub to the {api_documentation} and {api_file} table for this
          // file so it doesn't get added to the parse queue again.
          $nid = api_new_documentation_node();
          $doc = array(
            'object_name' => $file_name,
            'file_name' => $file_name,
            'title' => drupal_basename($file_name),
            'object_type' => 'file',
            'branch_id' => $branch->branch_id,
            'queued' => time(),
            'did' => $nid,
          );
          _api_add_text_defaults($doc, 'api_documentation');
          drupal_write_record('api_documentation', $doc);
          drupal_write_record('api_file', $doc);
        }
        else {
          // Update the 'queued' field in the {api_file} table so this does not
          // get queued again.
          $file->queued = time();
          drupal_write_record('api_file', $file, 'did');
        }
      }
    }
  }

  // Remove outdated files.
  foreach (array_keys($files) as $file_name) {
    watchdog('api', 'Removing %file.', array('%file' => $file_name));
    $dids = db_select('api_documentation', 'ad')
      ->fields('ad', array('did'))
      ->condition('file_name', $file_name)
      ->condition('branch_id', $branch->branch_id)
      ->execute()
      ->fetchCol();
    api_delete_items($dids, FALSE);
    api_schedule_shutdown();
  }

  // Mark the branch as having been updated.
  $branch->last_updated = time();
  $branch->queued = 0;
  drupal_write_record('api_branch', $branch, array('branch_id'));
  watchdog('api', 'Updated %project branch %branch_name.', array('%branch_name' => $branch->branch_name, '%project' => $branch->project));

  return TRUE;
}

/**
 * Finds all the files in the directories specified for a branch.
 *
 * @param $directories
 *   List of directories to scan, as text (separated by newlines).
 * @param $excluded_directories
 *   List of directories to exclude, as text (separated by newlines).
 * @param $regexps
 *   List of regular expressions to exclude (files or directories), separated
 *   by newlines.
 *
 * @return
 *   Associative array of files, where the keys are the full paths to the
 *   files and the values are the file names. File names exclude the part of
 *   the full path that is shared by all directories in the scan list. Only
 *   files that can be parsed are included (see api_parse_functions() for a
 *   list). Hidden directories (names starting with .) below top-level entries
 *   in $directories are excluded, as well as directories called CVS.
 */
function api_scan_directories($directories, $excluded_directories, $regexps) {
  $directory_array = array_filter(explode("\n", $directories));
  $excluded_array = array_filter(explode("\n", $excluded_directories));
  $extensions = array_keys(api_parse_functions());
  $regexps = explode("\n", $regexps);
  $regexps = array_filter(array_map('trim', $regexps));

  // Figure out the shared part of the file name that is in all the directories.
  if (count($directory_array) > 1) {
    $directories_components = array();
    foreach ($directory_array as $directory) {
      $directory_components = array();
      $parts = explode(DIRECTORY_SEPARATOR, $directory);
      $starts_with_sep = (strpos($directory, DIRECTORY_SEPARATOR) === 0);
      foreach ($parts as $part) {
        if (strlen($part)) {
          $previous = reset($directory_components);
          if (strlen($previous) > 0 || $starts_with_sep) {
            $previous .= DIRECTORY_SEPARATOR;
          }
          array_unshift($directory_components, $previous . $part);
        }
      }
      $directories_components[] = $directory_components;
    }

    $common_ancestor_components = call_user_func_array('array_intersect', $directories_components);
    $common_ancestor = reset($common_ancestor_components);
  }
  else {
    $common_ancestor = $directories;
  }

  // Scan the directories. Instead of letting file_scan_directory() do its own
  // recursion, we recurse ourselves. The reason is that we can exclude things
  // as we go, saving time and memory scanning .git, CVS, and the excluded
  // directories.
  $source_files = array();
  $todo = $directory_array;
  while (count($todo) > 0) {
    $directory = array_shift($todo);
    $files = file_scan_directory($directory, '/.*/', array('recurse' => FALSE));
    foreach ($files as $path => $file) {
      // Exclude anything that matches one of our regular expressions.
      $ok = TRUE;
      foreach($regexps as $regexp) {
        if (preg_match($regexp, $path)) {
          $ok = FALSE;
          break;
        }
      }
      if (!$ok) {
        continue;
      }

      if (is_dir($path)) {
        // See if we want to scan this path, and if so, add to To Do list.
        // Don't keep any directory that starts with ".", or anything in the
        // excluded list. Note that $file->filename includes $directory, so
        // strip that off before checking the name for ".".
        $dir_name = substr($file->filename, strlen($directory));
        if ((strpos($dir_name, '.') !== 0) &&
          !in_array($path, $excluded_array)) {
          $todo[] = $path;
        }
      }
      else {
        // This is a regular file. See if it has an extension we recognize,
        // and save it in our return list if so.
        $ext = pathinfo($path, PATHINFO_EXTENSION);
        if (in_array($ext, $extensions)) {
          $file_name = substr($path, strlen($common_ancestor) + 1);
          $source_files[$path] = $file_name;
        }
      }
    }
  }

  return $source_files;
}

/**
 * Adds defaults for TEXT fields to a database record.
 *
 * These cannot come from the schema, because TEXT fields have no defaults.
 *
 * @param $record
 *   Record about to be written to the database with drupal_write_record().
 * @param $table
 *   Table $record is going into.
 */
function _api_add_text_defaults(&$record, $table) {
  switch ($table) {
    case 'api_documentation':
      $record += array(
        'summary' => '',
        'documentation' => '',
        'code' => '',
        'see' => '',
        'deprecated' => '',
        'var' => '',
        'throws' => '',
        'namespace' => '',
        'namespaced_name' => '',
        'modifiers' => '',
      );
      break;

    case 'api_function':
      $record += array(
        'parameters' => '',
        'return_value' => '',
      );
      break;
  }
}

/**
 * Creates a new documentation object node (for comments).
 *
 * @return
 *   Node ID of the created node.
 */
function api_new_documentation_node() {
  $node = new stdClass();
  $node->type = 'api';
  $node->uid = 0;
  if (module_exists('comment')) {
    $node->comment =  variable_get('comment_api', COMMENT_NODE_OPEN);
  }
  node_save($node);

  return $node->nid;
}
