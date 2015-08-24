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
  // See if this is a Backdrop file or Backdrop-excluded file.
  $backdrop_regexps = (isset($branch->exclude_backdropism_regexp) ? explode("\n", $branch->exclude_backdropism_regexp) : array());
  $backdrop_regexps = array_filter(array_map('trim', $backdrop_regexps));
  $is_backdrop = TRUE;
  foreach ($backdrop_regexps as $regexp) {
    if (preg_match($regexp, $file_path)) {
      $is_backdrop = FALSE;
      break;
    }
  }

  $basename = backdrop_basename($file_name);
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
    'is_backdrop' => $is_backdrop,
    'code' => '',
  );

  $callback($docblock);
}

/**
 * Saves contents of a Twig file as API documentation.
 *
 * Callback for api_parse_file().
 *
 * @param array $docblock
 *   Array from api_parse_file() containing the file contents and information
 *   about the file, branch, etc.
 */
function api_parse_twig_file($docblock) {
  // Use the text file function, but escape HTML characters.
  api_parse_text_file($docblock, TRUE);
}

/**
 * Saves contents of a file as a single piece of text documentation.
 *
 * Callback for api_parse_file().
 *
 * @param array $docblock
 *   Array from api_parse_file() containing the file contents and information
 *   about the file, branch, etc.
 * @param bool $escape_html
 *   If TRUE, escpae HTML characters in the source code listing (for Twig
 *   files).
 */
function api_parse_text_file($docblock, $escape_html = FALSE) {
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
    // Avoid timeouts in Grammar Parser, but do not run forever in case it
    // has a major problem. 10 minutes should be plenty. Backdrop cron normally
    // has a limit of 4 minutes. This will reset timing counter to 0 also.
    backdrop_set_time_limit(600);
    $editor = PGPEditor::getInstance();
    $docblock['content'] = $editor->commentToString(array('value' => $matches[0]));
  }
  else {
    $tmp = array();
    $docblock['documentation'] = api_format_documentation($docblock['source'], FALSE, $tmp);
  }

  // Escape HTML and number the lines.
  $output = $docblock['source'];
  if ($escape_html) {
    $output = htmlspecialchars($docblock['source']);
  }
  $output = api_number_lines($output);
  $output = '<pre class="php"><code>' . $output . '</code></pre>';
  $docblock['code'] = $output;

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
  // Avoid timeouts in Grammar Parser, but do not run forever in case it
  // has a major problem. 10 minutes should be plenty. Backdrop cron normally
  // has a limit of 4 minutes. This will reset timing counter to 0 also.
  backdrop_set_time_limit(600);
  $editor = PGPEditor::getInstance();
  $reader = $editor->getReader();
  // Set up the reader to preserve the spacing of the original code.
  $reader->setPreserveWhitespace(FALSE);
  $reader->setPreserveNewline(TRUE);
  $reader->setPreserveArrayNewline(TRUE);
  $reader->setSnippet($docblock['source']);
  $reader->buildGrammar();

  // Retrieve items of interest.
  $statements = $reader->getStatements();
  if (!$statements) {

    // This is a text file or template file with no declarations of functions,
    // constants, etc. But we do want to track references in it.
    $docblock['code'] = api_format_php($docblock['source'], $docblock['references'], TRUE, TRUE, $docblock['is_backdrop']);

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
  $docblock['code'] = api_format_php($docblock['source'], $references, TRUE, TRUE, $docblock['is_backdrop']);
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
    'is_backdrop' => $docblock['is_backdrop'],
    'code' => '',
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
  $bare_docblock = $docblock;
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
  // could be callback function names, in addition to some array keys. The
  // level of keys we want depends on (a) if it's a Backdrop file, and (b) the
  // file extension.
  $key_level = 0;
  $is_services = FALSE;
  if ($docblock['is_backdrop']) {
    $basename = $docblock['basename'];
    $matches = array();
    if (preg_match('|\.([^.]+)\.yml$|', $basename, $matches)) {
      if ($matches[1] == 'services') {
        // We want to keep references to 2nd-level keys in services.yml files.
        $key_level = 2;
        $is_services = TRUE;
      }
      elseif ($matches[1] == 'routing' || $matches[1] == 'local_tasks' || $matches[1] == 'contextual_links') {
        // We want to keep references to 1st-level keys in routing.yml and
        // related files.
        $key_level = 1;
      }
    }
  }

  $references = _api_find_yaml_references($parsed, $key_level);
  // For services files, we do not want the YML strings references, because
  // really only the services are relevant.
  if ($is_services) {
    unset($references['yaml string']);
  }
  $docblock['references'] = $references;

  // Now we need to format the source of this file. Start by escaping
  // HTML tags and entities.
  $code = $docblock['source'];
  $code = htmlspecialchars($code, ENT_NOQUOTES, 'UTF-8');

  // Wrap each found callback reference string in a span. We have to use
  // preg_replace() to do this, because we only want to match whole strings,
  // enclosed in single quotes, double quotes, or whitespace. And we want to
  // do the whole thing in one replace, so it matches the longest possible
  // string in each case. So we also need to sort the strings by length,
  // longest first, because PHP regular expressions with alternatives take the
  // first matching one.
  $callbacks = $references['potential callback'];
  if (count($callbacks)) {
    $newstrings = array();
    usort($callbacks, '_api_length_compare');
    foreach ($callbacks as $string) {
      $newstrings[] = preg_quote($string, '/');
    }

    $regexp = '/(?<=[\'"\s])(' . implode('|', $newstrings) . ')(?=[\'"\s])/';
    $code = preg_replace($regexp, '<span class="yaml-reference">$1</span>', $code);
  }

  // Number the lines and put into a code block.
  $code = api_number_lines($code);
  $code = '<pre class="php"><code>' . $code . '</code></pre>';
  $docblock['code'] = $code;

  $docblocks = array($docblock);

  // For services files, make docblocks for each service.
  if ($is_services && isset($parsed['services'])) {
    module_load_include('php', 'api', 'yaml/Dumper');
    module_load_include('php', 'api', 'yaml/Escaper');
    $dumper = new \Symfony\Component\Yaml\Dumper();
    $dumper->setIndentation(2);

    foreach ($parsed['services'] as $name => $info) {
      try {
        $code = $dumper->dump($info, 2);
      }
      catch (Exception $e) {
        // We should be able to dump, but just in case, fall back to printing,
        // which is better than nothing.
        $code = print_r($info, TRUE);
      }
      $code = htmlspecialchars($code, ENT_NOQUOTES, 'UTF-8');
      if (count($callbacks)) {
        $code = preg_replace($regexp, '<span class="yaml-reference">$1</span>', $code);
      }
      $code = api_number_lines($code);
      $code = '<pre class="php"><code>' . $code . '</code></pre>';

      $references = array('service_tag' => array());
      $class = '';
      if (isset($info['class']) && $info['class']) {
        $class = $info['class'];
        // Make sure the class name starts with a backslash.
        $ref = $class;
        $pos = strpos($ref, '\\');
        if ($pos !== 0) {
          $ref = '\\' . $ref;
        }

        $references['service_class'] = array($ref);
      }
      $alias = (isset($info['alias']) && $info['alias']) ? $info['alias'] : '';
      $abstract = (isset($info['abstract']) && $info['abstract']);

      if (isset($info['tags'])) {
        foreach ($info['tags'] as $tag) {
          $tag_name = $tag['name'];
          if ($tag_name) {
            $references['service_tag'][$tag_name] = $tag_name;
          }
        }
      }

      $service = array(
        'object_name' => $name,
        'title' => $name,
        'object_type' => 'service',
        'code' => $code,
        'source' => '',
        'documentation' => $class ? $class : ($alias ? t('Alias of %alias', array('%alias' => $alias)) : ($abstract ? t('Abstract') : '')),
        'references' => $references,
      ) + $bare_docblock;

      $docblocks[] = $service;
    }
  }

  api_save_documentation($docblocks);
}

/**
 * Sort callback for usort in api_parse_yaml_file().
 *
 * Sorts with longest first.
 */
function _api_length_compare($a, $b) {
  $lena = strlen($a);
  $lenb = strlen($b);
  if ($lena < $lenb) {
    return 1;
  }
  return ($lena > $lenb) ? -1 : 0;
}

/**
 * Recursively finds potential references in a parsed YAML array.
 *
 * @param $yaml
 *   Parsed YAML object.
 * @param int $key_refs_level
 *   Store references to the keys on this level, where 1 is the current level.
 *
 * @return array
 *   Array of references suitable for use in $docblock['references'].
 */
function _api_find_yaml_references($yaml, $key_refs_level) {
  if (empty($yaml)) {
    return array('potential callback' => array());
  }

  $yaml = (array) $yaml;
  $references = array('potential callback' => array(), 'yaml string' => array());
  foreach ($yaml as $key => $value) {
    if ($key_refs_level == 1 && is_string($key)) {
      $key = trim($key);
      if ($key) {
        $references['yaml string'][$key] = $key;
      }
    }
    if (is_string($value)) {
      $matches = array();
      if (preg_match("|^['\"]*(" . API_RE_FUNCTION_IN_TEXT . ")['\"]*$|", $value, $matches)) {
        // Special case the commonly-found TRUE and FALSE.
        $val = trim($matches[1]);
        if ($val && $val != 'TRUE' && $val != 'FALSE' && !is_numeric($val)) {
          $references['potential callback'][$val] = $val;
        }
      }
    }
    elseif (is_array($value) || is_object($value)) {
      $references = _api_merge_references($references, _api_find_yaml_references($value, $key_refs_level - 1));
    }
  }

  return $references;
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
        // See if this is a use statement for the file, or a use statement
        // indicating using a trait inside a class/trait.
        if (!empty($default_block['class'])) {
          // This is inside a file, so it's a trait use statement.
          $trait_info = $statement->valuesToArray('alias');
          foreach ($trait_info as $key => $value) {
            // Should be only one array element...
            $trait_alias = $key;
            $trait_class = $value;
          }

          $details = array();
          if (!empty($statement->body)) {
            $details = $statement->bodyValuesToArray();
          }
          $references = array('use_trait' => array($trait_alias => array('class' => $trait_class, 'details' => $details)));
        }
        else {
          $references = array('use_alias' => $statement->valuesToArray('alias'));
        }
        $all_references = _api_merge_references($all_references, $references);
        break;

      case T_ASSIGNMENT: // Class property
        if (empty($default_block['class'])) {
          break;
        } // Explicit fallthrough.
      case T_INTERFACE:
      case T_TRAIT:
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
        $docblock['code'] = api_format_php("<?php\n" . $statement->toString() . "\n?" . ">", $references, FALSE, ($statement->type == T_FUNCTION), $docblock['is_backdrop']);

        if (isset($statement->modifiers) && !$statement->modifiers->isEmpty()) {
          $docblock['modifiers'] = $statement->modifiers->toString();
        }

        if ($statement->type == T_FUNCTION) {
          $docblock['signature'] = $editor->functionGetSignature($statement);
          $docblock['references'] = $references;
          $all_references = _api_merge_references($all_references, $references);
        }
        elseif ($statement->type == T_CLASS || $statement->type == T_INTERFACE || $statement->type == T_TRAIT) {
          $docblock['extends'] = is_object($statement->extends) ? explode(', ', $statement->extends->toString()) : array();
          $docblock['implements'] = is_object($statement->implements) ? explode(', ', $statement->implements->toString()) : array();
          $docblock['references'] = $references;
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


    // Additional recursive processing on statements with bodies that can
    // contain other doc objects.
    switch ($type) {
      case T_TRAIT:
      case T_INTERFACE:
      case T_CLASS:
        if (!empty($statement->body)) {
          // We need to find all the additional things in here (methods, etc.),
          // which will add items to the $docblocks array. And then we need to
          // add the references found to the parent docblock. So keep track of
          // which docblock we are adding to.
          $last_index = count($docblocks) - 1;
          $references = api_documentation_loop($statement->body, array_merge($default_block, array('class' => $docblock['object_name'])), $docblocks);
          $all_references = _api_merge_references($all_references, $references);
          $docblocks[$last_index]['references'] = $references;
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
        watchdog('api', 'Conflicting use declarations for %name found in file. Only first is used.', array('%name' => $alias), WATCHDOG_WARNING);
        unset($new['use_alias'][$alias]);
      }
    }
  }

  // Use array_replace_recursive here so we do not get duplicate references.
  return array_replace_recursive($master, $new);
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

  // Look for @file block first so $docblocks[0] gets filled in before it
  // is processed.
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

  // Take care of the (bad, but possible) case where the doc block for one
  // of the items in the file (class, function, etc.) has a @defgroup or
  // @mainpage in it, by separating the doc block from the item.
  $old_blocks = $docblocks;
  $docblocks = array();
  foreach ($old_blocks as $docblock) {
    if ($docblock['code'] && $docblock['content'] &&
      (preg_match('/' . API_RE_TAG_START . 'mainpage/', $docblock['content']) ||
       preg_match('/' . API_RE_TAG_START . 'defgroup/', $docblock['content']))) {
      $new_block = $docblock;
      // Make one block have just the code and the other, just the docs.
      $new_block['code'] = '';
      $docblock['content'] = '';
      $docblocks[] = $new_block;
      $docblocks[] = $docblock;
      watchdog('api', 'Item docblock containing @defgroup or @mainpage in %file at line %line. Separated into two blocks.', array('%file' => $docblocks[0]['file_name'], '%line' => $docblock['start_line']), WATCHDOG_WARNING);

    }
    else {
      $docblocks[] = $docblock;
    }
  }

  $need_to_reparse = FALSE;
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
    $annotation_matches = array();
    $docblock['annotation_class'] = FALSE;
    if (preg_match('/' . API_RE_TAG_START . 'Annotation' . API_RE_WORD_BOUNDARY_END . '/', $docblock['content'], $annotation_matches)) {
      if ($docblock['is_backdrop']) {
        $docblock['content'] = str_replace($annotation_matches[0], "\n@ingroup annotation\n", $docblock['content']);
      }
      $docblock['annotation_class'] = TRUE;
    }

    // Change @Event to @ingroup events.
    $event_matches = array();
    if (preg_match('/' . API_RE_TAG_START . 'Event' . API_RE_WORD_BOUNDARY_END . '/', $docblock['content'], $event_matches)) {
      $docblock['content'] = str_replace($event_matches[0], "\n@ingroup events\n", $docblock['content']);
    }

    // Deal with @mainpage.
    if (preg_match('/' . API_RE_TAG_START . 'mainpage/', $docblock['content'])) {
      preg_match('/' . API_RE_TAG_START . 'mainpage (.*?)\n/', $docblock['content'], $mainpage_matches);
      $docblock['title'] = (isset($mainpage_matches[1]) ? $mainpage_matches[1] : '');
      $docblock['content'] = preg_replace('/' . API_RE_TAG_START . 'mainpage.*?\n/', '', $docblock['content']);
      $docblock['object_type'] = 'mainpage';
      $docblock['object_name'] = $docblocks[0]['branch']->branch_name;
    }
    // Deal with @defgroup.
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
          // This is commonly because the defgroup was moved from one file
          // to another. So requeue both the found ID and the current file
          // we are working on.
          api_requeue_after_run($did);
          $need_to_reparse = TRUE;
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
    // Some vendor files use @inheritDoc instead.
    $docblock['content'] = str_replace('{@inheritDoc}', '', $docblock['content']);

    if ($docblock['content'] && trim($docblock['content'])) {
      // Find parameter definitions with @param.
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
      $tmp = array();
      $params = api_format_documentation($params, TRUE, $tmp);
      $params = str_replace('[strong]', '<strong>', $params);
      $params = str_replace('[/strong]', '</strong>', $params);
      $docblock['parameters'] = $params;

      // Find return value definitions with @return.
      $docblock['return_value'] = '';
      preg_match_all('/' . API_RE_TAG_START . 'return\s(.*?)(?=\n' . API_RE_TAG_START . '|$)/s', $docblock['content'], $return_matches, PREG_SET_ORDER);
      foreach ($return_matches as $return_match) {
        $docblock['content'] = str_replace($return_match[0], '', $docblock['content']);
        $docblock['return_value'] .= "\n\n" . $return_match[1];
      }
      $docblock['return_value'] = api_format_documentation($docblock['return_value'], TRUE, $tmp);

      // Find @see lines.
      $docblock['see'] = '';
      while (preg_match('/' . API_RE_TAG_START . 'see\s(.*?)\n/s', $docblock['content'], $match)) {
        $docblock['content'] = str_replace($match[0], '', $docblock['content']);
        $docblock['see'] .= "\n\n" . $match[1];
      }
      $docblock['see'] = api_format_documentation($docblock['see'], TRUE, $tmp);

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
      $docblock['throws'] = api_format_documentation($docblock['throws'], TRUE, $tmp);

      // Find @deprecated, a paragraph (could be more than one).
      $docblock['deprecated'] = '';
      while (preg_match('/' . API_RE_TAG_START . 'deprecated\s(.*?)(?=\n' . API_RE_TAG_START . '|$)/s', $docblock['content'], $match)) {
        $docblock['content'] = str_replace($match[0], '', $docblock['content']);
        $docblock['deprecated'] .= "\n\n" . $match[1];
      }
      $docblock['deprecated'] = api_format_documentation($docblock['deprecated'], TRUE, $tmp);

      // Format everything remaining as the main documentation.
      $docblock['documentation'] = api_format_documentation($docblock['content'], TRUE, $docblock['references']);
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
      backdrop_write_record('api_documentation', $docblock, 'did');
    }
    else {
      $nid = api_new_documentation_node();
      $docblock['did'] = $nid;
      $docblock['branch_id'] = $docblocks[0]['branch']->branch_id;
      _api_add_text_defaults($docblock, 'api_documentation');
      backdrop_write_record('api_documentation', $docblock);
    }

    // Keep track of class membership
    if ($docblock['object_type'] === 'class' || $docblock['object_type'] === 'interface' || $docblock['object_type'] == 'trait') {
      $class_dids[$docblock['object_name']] = $docblock['did'];
      api_touched($docblock['did']);
    }

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

      case 'service':
        api_replace_references($docblock['branch'], $docblock['did'], $docblock['object_name'], $docblock['references'], FALSE, $namespace, $use_aliases);
        break;

      case 'file':
        // Note that this will set the 'queued' field back to zero, since
        // $docblock['queued'] is set to 0 up at the top of api_parse_file().
        backdrop_write_record('api_file', $docblock, 'did');
        api_replace_references($docblock['branch'], $docblock['did'], $docblock['object_name'], $docblock['references'], TRUE, $namespace, $use_aliases);
        break;

      case 'interface':
      case 'class':
      case 'trait':
        db_delete('api_reference_storage')
          ->condition('branch_id', $docblocks[0]['branch']->branch_id)
          ->condition('object_type', array('class', 'interface', 'trait', 'annotation_class', 'annotation'))
          ->condition('from_did', $docblock['did'])
          ->execute();
        db_delete('api_namespace')
          ->condition('did', $docblock['did'])
          ->condition('object_type', array('trait_instead', 'trait_alias'))
          ->execute();

        foreach ($docblock['extends'] as $extend) {
          $refname = api_full_classname($extend, $namespace, $use_aliases);
          api_reference($docblocks[0]['branch'], 'class', $refname, $docblock['did']);
        }
        foreach ($docblock['implements'] as $implement) {
          $refname = api_full_classname($implement, $namespace, $use_aliases);
          api_reference($docblocks[0]['branch'], 'interface', $refname, $docblock['did']);
        }

        if (isset($docblock['references']['use_trait'])) {
          foreach ($docblock['references']['use_trait'] as $alias => $info) {
            $class = $info['class'];
            $refname = api_full_classname($class, $namespace, $use_aliases);
            $refalias = api_full_classname($alias, $namespace, $use_aliases);
            if ($refname != $refalias) {
              watchdog('api', 'Aliases for use statements for traits are not supported in %filename for %alias, %class', array('%filename' => $docblock['file_name'], '%alias' => $alias, '%class' => $class), WATCHDOG_WARNING);
            }
            api_reference($docblocks[0]['branch'], 'trait', $refname, $docblock['did']);
            // If there are insteadof/alias details for this trait, save them
            // in the namespaces table (because it has the right columns).
            if (isset($info['details'])) {
              foreach ($info['details'] as $type => $list) {
                foreach ($list as $name => $item) {
                  if ($type == 'insteadof') {
                    // @todo We could support insteadof here; see other todo
                    // for where it would be used.
                    watchdog('api', 'Trait inheritance with insteadof statements are not supported in %filename for %item', array('%filename' => $docblock['file_name'], '%item' => $name), WATCHDOG_WARNING);
                  }
                  elseif (in_array($name, array('public', 'protected', 'private'))) {
                    watchdog('api', 'Trait inheritance that changes visibility is not supported in %filename for %item', array('%filename' => $docblock['file_name'], '%item' => $item), WATCHDOG_WARNING);
                  }
                  else {
                    api_namespace($docblock['did'], 'trait_' . $type, $name, $refname . '::' . $item);
                  }
                }
              }
            }
          }
        }
        if (isset($docblock['references']['annotation'])) {
          foreach($docblock['references']['annotation'] as $class) {
            api_reference($docblocks[0]['branch'], 'annotation', $class, $docblock['did']);
          }
        }
        if ($docblock['annotation_class']) {
          api_reference($docblocks[0]['branch'], 'annotation_class', $docblock['object_name'], $docblock['did']);
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

  if ($need_to_reparse && count($dids)) {
    // Mark any DID found as the one to requeue -- they should all be from
    // the same file.
    api_requeue_after_run($dids[0]);
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

  // Invoke hook_api_updated for doc objects that have been updated.
  $changed_dids = array_intersect($dids, $old_dids);
  if (count($changed_dids)) {
    module_invoke_all('api_updated', $changed_dids);
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
  $types_to_remove =  array('function', 'potential hook', 'potential theme', 'potential alter', 'potential callback', 'potential file', 'constant', 'member-parent', 'member-self', 'member', 'member-class', 'yaml string', 'service_tag', 'service_class');

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

      case 'potential callback':
      case 'function':
      case 'member-self':
        // Don't save a reference to the item itself.
        unset($list[$shortname]);

        // Intentional fallthrough here!

      default:
        foreach ($list as $call) {
          // If the name contains a backslash, and the first occurrence is not
          // at the beginning, make sure it starts with a backslash
          // so it is a fully-namespaced reference.
          $pos = strpos($call, '\\');
          if ($pos !== FALSE && $pos !== 0) {
            $call = '\\' . $call;
          }

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
 * converts newlines into paragraphs. Note that this function does not do any
 * Backdrop-specific formatting, aside from formatting plugin annotation, which
 * should be fine for vendor files as well.
 *
 * @param string $documentation
 *   Documentation string to format.
 * @param bool $auto_p
 *   TRUE (default) to convert to paragraphs. FALSE to skip this conversion and
 *   put the documentation in PRE tags.
 * @param array $references
 *   Array of references. If this function finds references (only for plugin
 *   annotation), this array may be added to.
 *
 * @return string
 *   Formatted documentation.
 */
function api_format_documentation($documentation, $auto_p = TRUE, &$references) {
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
  $documentation = preg_replace('/' . API_RE_TAG_START . 'link ((http:\/\/|https:\/\/|ftp:\/\/|mailto:|smb:\/\/|afp:\/\/|file:\/\/|gopher:\/\/|news:\/\/|ssl:\/\/|sslv2:\/\/|sslv3:\/\/|tls:\/\/|tcp:\/\/|udp:\/\/)([a-zA-Z0-9@:%_+*!~#?&=.,\/;-]*[a-zA-Z0-9@:%_+*!~#&=\/;-])) (.*?) ' . API_RE_TAG_START . 'endlink/', '<a href="$1">$4</a>', $documentation);
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
  // at the very end of the documentation block. And add a reference.
  $annotation_matches = array();
  if (preg_match('/' . API_RE_TAG_START . '(' . API_RE_FUNCTION_CHARACTERS . ')(\(.*\))$/s', $documentation, $annotation_matches)) {
    $class = $annotation_matches[1];
    $references['annotation'][$class] = $class;
    $documentation = str_replace($annotation_matches[0], '<h3>' . t('Plugin annotation') . '</h3>' . "\n@code\n@" . $annotation_matches[1] . $annotation_matches[2]. "\n@endcode", $documentation);
  }

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
  return "\n" . api_format_php("<?php" . decode_entities($matches[1]) . "?" . ">", $references, FALSE, FALSE, FALSE) . "\n";
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
 * @param $is_backdrop
 *   TRUE (default) to use Backdrop-specific parsing; FALSE to skip Backdropisms.
 *
 * @return
 *   HTML-formatted code, with spans enclosing various PHP elements.
 */
function api_format_php($code, &$references, $number = FALSE, $track_references = TRUE, $is_backdrop = TRUE) {
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
  // - Are we in process of declaring a function, class, etc.?
  // - How many PHP start/end tags we found (will remove at end if there is
  //   only one set).
  // - Class members: $foo->bar or Foo::bar.
  // - Are we building up a namespaced class name?
  // - Was the previous token @ (for annotation classes)?

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

  // In process of declaring something.
  $declaring = FALSE;

  // Class members.
  $in_member = FALSE;
  $member_of = '';

  // Namespaces and annotations.
  $in_namespace = FALSE;
  $full_namespaced_name = '';
  $in_annotation = FALSE;

  // Split the code into tokens.
  $tokens = token_get_all($code);
  foreach ($tokens as $array_index => $token) {
    $close_size = 0;

    // Take care of double-quoted strings.
    if ($in_string) {
      $declaring = FALSE;
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
      $declaring = FALSE;
      $in_annotation = FALSE;
      continue;
    }

    if (is_array($token)) {
      $type = $token[0];
      $value = htmlspecialchars($token[1]);
      // See if this is possibly a constant.
      $could_be_constant = preg_match('|^[A-Z][A-Z0-9_]+$|', $value);

      switch ($type) {
        case T_OPEN_TAG:
          $in_annotation = FALSE;
          $declaring = FALSE;
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
          $in_annotation = FALSE;
          $declaring = FALSE;
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
          $in_annotation = FALSE;
          $declaring = FALSE;
          $in_member = FALSE;
          $member_of = '';
          $in_namespace = FALSE;
          $full_namespaced_name = '';
          $output .= '<span class="php-comment">' . $value . '</span>';
          break;

        case T_VARIABLE:
          // This is a PHP variable like $foo. It could be a member
          // variable like self::$foo.
          $class = 'php-variable';
          if ($in_member) {
            $class .= ' variable';
            if ($member_of == 'parent') {
              $class .= ' member-of-parent';
            }
            elseif ($member_of == '$this' || $member_of == 'self' || $member_of == 'static') {
              $class .= ' member-of-self';
            }
            elseif (strpos($member_of, '$') === 0) {
              $class .= ' member-of-variable';
            }
            elseif (strlen($member_of) > 0) {
              $class .= ' member-of-class-' . $member_of;
            }
            else {
              $class .= ' member-of-variable';
            }
          }
          $in_annotation = FALSE;
          $declaring = FALSE;
          $in_member = FALSE;
          $member_of = $value;
          $in_namespace = FALSE;
          $full_namespaced_name = '';
          $output .= '<span class="' . $class . '">' . $value . '</span>';
          break;

        case T_CONSTANT_ENCAPSED_STRING:
          $in_annotation = FALSE;
          $declaring = FALSE;
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
          if ($track_references) {
            if (preg_match("|^'" . BACKDROP_PHP_FUNCTION_PATTERN . "'$|", $value)) {
              $newname = str_replace("'", '', $value);
              $references['potential callback'][$newname] = $newname;
            }
            elseif (preg_match("|^'" . API_RE_FILENAME . "'$|", $value)) {
              // Some of these may be quite long, so truncate.
              $newname = str_replace("'", '', backdrop_substr($value, 0, 127));
              $references['potential file'][$newname] = $newname;
            }
          }

          $output .= '<span class="' . $class . '">' . $value . '</span>';
          break;

        case T_INLINE_HTML:
          $in_annotation = FALSE;
          $declaring = FALSE;
          $in_member = FALSE;
          $member_of = '';
          $in_namespace = FALSE;
          $full_namespaced_name = '';
          $output .= '<span class="php-string">' . $value . '</span>';
          break;

        case T_STATIC:
        case T_STRING:
          // This is a function, constant, etc. being declared or used.
          // Plus the special case of the static keyword, since this can
          // also be used as static::memberFunction().

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
          if ($declaring) {
            $class .= '-declared';
          }
          if ($in_annotation) {
            $class = 'class-annotation';
            $reference_type = NULL;
          }
          elseif ($could_be_constant) {
            $reference_type = 'constant';
            $reference = $value;
          }
          else {
            $reference_type = 'function';
            $reference = $value;
          }

          // See if this is a function call or not, by looking ahead to the
          // next token and seeing if it is an open paren.
          $is_function = (!$in_annotation && isset($tokens[$array_index + 1]) && !is_array($tokens[$array_index + 1]) && $tokens[$array_index + 1] == '(');

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

          // Save references -- for functions and constants, only if they're not
          // being declared.
          if ($track_references && ($is_function || $could_be_constant) && !$declaring && $reference_type) {
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
          $declaring = FALSE;

          // Save $invoke_type state.
          if ($is_backdrop) {
            if ($value == 'theme') {
              $invoke_type = 'theme';
              $target_paren_count = $paren_count + 1;
            }
            elseif (in_array($value, $invoke_function_names)) {
              $invoke_type = 'hook';
              $target_paren_count = $paren_count + 1;
            }
            elseif (in_array($value, array('backdrop_alter', 'alter', 'alterInfo'))) {
              $invoke_type = 'alter';
              $target_paren_count = $paren_count + 1;
            }
          }

          $in_annotation = FALSE;
          break;

        case T_NS_SEPARATOR:
          $in_annotation = FALSE;
          $declaring = FALSE;
          $in_namespace = TRUE;
          $full_namespaced_name .= $value;
          $in_member = FALSE;
          $member_of = '';
          break;

        case T_LNUMBER:
        case T_DNUMBER:
          $in_annotation = FALSE;
          $declaring = FALSE;
          $in_member = FALSE;
          $member_of = '';
          $in_namespace = FALSE;
          $full_namespaced_name = '';
          $output .= '<span class="php-constant">' . $value . '</span>';
          break;

        case T_CLASS:
        case T_INTERFACE:
        case T_TRAIT:
        case T_FUNCTION:
        case T_CONST:
        case T_PRIVATE:
        case T_PUBLIC:
        case T_PROTECTED:
        case T_ABSTRACT:
          $in_annotation = FALSE;
          $declaring = TRUE;
          $in_member = FALSE;
          $member_of = '';
          $in_namespace = FALSE;
          $full_namespaced_name = '';
          $output .= '<span class="php-keyword">' . $value . '</span>';
          break;

        case T_ARRAY_CAST:
        case T_ARRAY:
        case T_AS:
        case T_BOOL_CAST:
        case T_BREAK:
        case T_CASE:
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
        case T_STRING_CAST:
        case T_SWITCH:
        case T_UNSET_CAST:
        case T_UNSET:
        case T_USE:
        case T_VAR:
        case T_WHILE:
          $in_annotation = FALSE;
          $declaring = FALSE;
          $in_member = FALSE;
          $member_of = '';
          $in_namespace = FALSE;
          $full_namespaced_name = '';
          $output .= '<span class="php-keyword">' . $value . '</span>';
          break;

        case T_OBJECT_OPERATOR:
        case T_DOUBLE_COLON:
          $in_annotation = FALSE;
          $declaring = FALSE;
          $output .= $value;
          $in_member = TRUE;
          $in_namespace = FALSE;
          $full_namespaced_name = '';
          break;

        case T_WHITESPACE:
          $in_annotation = FALSE;
          $in_namespace = FALSE;
          $full_namespaced_name = '';
          $output .= $value;
          break;

        default:
          $in_annotation = FALSE;
          $declaring = FALSE;
          $in_member = FALSE;
          $member_of = '';
          $in_namespace = FALSE;
          $full_namespaced_name = '';
          $output .= $value;
      }
    }
    else {
      // Non-array (simple) tokens.
      $declaring = FALSE;
      $in_member = FALSE;
      $member_of = '';
      $in_namespace = FALSE;
      $full_namespaced_name = '';
      $in_annotation = FALSE;

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
      elseif ($token == '@') {
        $in_annotation = TRUE;
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
    $output = api_number_lines($output, $number);
  }

  $output = '<pre class="php"><code>' . $output . '</code></pre>';
  return $output;
}

/**
 * Numbers lines in code.
 *
 * @param string $code
 *   Code to number.
 * @param int $number
 *   Number to start with.
 *
 * @return string
 *   Numbered code. Uses OL list.
 */
function api_number_lines($code, $number = 1) {
  $start = (is_int($number)) ? $number : 1;
  $lines = explode("\n", $code);
  // If the last line is empty, omit it.
  $last = array_pop($lines);
  if ($last) {
    array_push($lines, $last);
  }
  $output = '<ol class="code-lines" start="' . $number . '"><li>' . implode("\n</li><li>", $lines) . "\n</li></ol>";

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

  // Avoid trying to save really long object names.
  $to_name = backdrop_substr($to_name, 0, 127);

  if ($to_type != 'function' || !$is_php_function[$to_name]) {
    if ($wait) {
      if (is_null($query_stored)) {
        $query_stored = db_insert('api_reference_storage')
          ->fields(array('object_name', 'branch_id', 'object_type', 'from_did', 'extends_did'));
      }
      $query_stored->values(array(
          'object_name' => $to_name,
          'branch_id' => $branch->branch_id,
          'object_type' => $to_type,
          'from_did' => $from_did,
          'extends_did' => 0,
        ));
    }
    else {
      db_insert('api_reference_storage')
        ->fields(array(
            'object_name' => $to_name,
            'branch_id' => $branch->branch_id,
            'object_type' => $to_type,
            'from_did' => $from_did,
            'extends_did' => 0,
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
  $scheduled = &backdrop_static(__FUNCTION__, FALSE);

  if (!$scheduled) {
    backdrop_register_shutdown_function('api_shutdown');
    $scheduled = TRUE;
  }
}

/**
 * Marks a class as "touched" for shutdown, and/or returns the touched list.
 *
 * @param int $class_did
 *   (optional) Documentation ID of the class that has been touched.
 *
 * @return int[]
 *   Array of the touched class IDs.
 */
function api_touched($class_did = 0) {
  $classes = &backdrop_static(__FUNCTION__, array());

  if ($class_did) {
    $classes[$class_did] = $class_did;
  }

  return $classes;
}

/**
 * Keeps track of files that need to be requeued after this cron run.
 *
 * @param int $did
 *   (optional) Documentation ID of a documentation item whose file needs
 *   to be requeued after the run.
 *
 * @return array
 *   Array of all documentation IDs that need to be requeued.
 */
function api_requeue_after_run($did = NULL) {
  static $dids = array();

  if ($did) {
    $dids[$did] = $did;
  }

  return $dids;
}

/**
 * Cleans up at the end of the cron job.
 *
 * Figures out class parents, class member lists, class member overrides, and
 * class member references.
 */
function api_shutdown() {
  // Requeue items that were marked for requeue.
  api_mark_items_for_reparse(api_requeue_after_run());

  // Figure out which classes and interfaces we need to update.
  $classes_touched = api_touched();

  // For each of these classes, and any that extend/implement them, we need
  // to recompute:
  // - The IDs for extended/implemented classes/interfaces.
  // - IDs for classes that extend/implement this class/interface.
  // - For traits, the traits and classes that use them.
  // - The {api_members} table, which figures out all class members including
  //   inherited members.
  // - The {api_overrides} table, which figures out for a class member if it
  //   is overriding another class's member, and where it is documented.
  // - The "computed-member" information in the {api_reference_storage} table.

  // So, the first step is to recompute class inheritance. This is stored by
  // name in {api_reference_storage}, and we need to find the actual IDs of
  // the classes. Do this one by one, since we need to compute some other
  // stuff anyway as we go.
  $classes_todo = $classes_touched;
  $classes_changed = $classes_touched;
  // Avoid infinite loops by keeping track of which classes we've already
  // checked.
  $classes_added = $classes_touched;
  // Keep track of the IDs of classes that extend/implement others, and the
  // members of each class.
  $class_parents = array();
  $class_members = array();
  while ($did = array_shift($classes_todo)) {
    $doc_object = _api_bare_object_load($did);
    if (!$doc_object) {
      continue;
    }

    // We're going to need the direct members of this class. Since methods,
    // properties, and constants can share names, do each separately.
    $result = db_select('api_documentation', 'd')
      ->fields('d', array('did', 'member_name', 'object_type'))
      ->condition('class_did', $did)
      ->condition('object_type', array('function', 'property', 'constant'))
      ->execute();
    $direct_members = array(
      'function' => array(),
      'property' => array(),
      'constant' => array(),
    );
    foreach ($result as $item) {
      $direct_members[$item->object_type][$item->member_name] = $item->did;
    }
    $class_members[$did] = $direct_members;

    // See if this class extends or inherits anything else, or uses traits,
    // and if so, calculate the IDs of the "parent" classes, traits, or
    // interfaces. Do this in the right order for inheritance: traits
    // trump class extends, and class extends trump interfaces.
    $parents = array();
    $parents_to_check = array();
    $types = array('trait', 'class', 'interface');
    foreach ($types as $type) {
      $results = db_select('api_reference_storage', 'ars')
        ->fields('ars')
        ->condition('ars.object_type', $type)
        ->condition('ars.from_did', $did)
        ->execute();
      if ($results) {
        foreach ($results as $result) {
          $parents_to_check[] = $result;
        }
      }
    }
    foreach ($parents_to_check as $parent) {
      // Figure out the ID of this parent.
      $new_id = api_best_class_id($parent->object_name, $parent->branch_id);
      $parents[$parent->object_name] = $new_id;
      // See if it's different from what we already had.
      if ($new_id != $parent->extends_did) {
        // Update the reference storage record with this new ID.
        db_update('api_reference_storage')
          ->fields(array('extends_did' => $new_id))
          ->condition('from_did', $did)
          ->condition('object_type', $parent->object_type)
          ->condition('object_name', $parent->object_name)
          ->execute();
        // This class changed.
        if (!in_array($did, $classes_changed)) {
          $classes_changed[] = $did;
        }
      }

      // If this class's parent is already marked as "changed", then this
      // class needs to be marked as "changed" also.
      if (in_array($new_id, $classes_changed)) {
        $classes_changed[] = $did;
      }

      // We also need to get the members of parent classes, so add it to the
      // to do list.
      if (($new_id) && !isset($classes_added[$new_id])) {
        $classes_todo[] = $new_id;
        $classes_added[$new_id] = $new_id;
      }
    }
    $class_parents[$did] = $parents;

    // See if anything else inherits from this class. If so, we need to check it
    // too. This could be a class that has already computed this as its
    // extends_did, or a class in the same core compatibility that extends a
    // class by this name, but has a blank extends_did because this class hadn't
    // been parsed yet. We won't worry about the possible edge case of a class
    // that had found another one previously and this one would now be better.
    $or = db_or()
      ->condition('extends_did', $did)
      ->condition('object_name', $doc_object->namespaced_name);
    $query = db_select('api_reference_storage', 'ars')
      ->fields('ars', array('from_did'))
      ->condition('object_type', array('class', 'interface', 'trait'))
      ->condition($or);
    $query->innerJoin('api_branch', 'b', 'b.branch_id = ars.branch_id');
    $query->condition('b.core_compatibility', $doc_object->core_compatibility);
    $results = $query->execute()->fetchCol();

    foreach ($results as $from_did) {
      if (!is_numeric($from_did)) {
        continue;
      }
      $from_did = (int) $from_did;
      if (!isset($classes_added[$from_did])) {
        $classes_todo[] = $from_did;
        $classes_added[$from_did] = $from_did;
      }
    }
  }

  // OK, at this point we have a list of all the classes that we need to
  // update member, override, and class parent information for. And we have
  // in hand a list of the parent IDs and the direct members for each one.
  // So go through this list and redo the members and overrides tables.

  foreach ($classes_changed as $did) {
    _api_update_class_reference_info($did, $class_parents, $class_members);
  }

  // Recompute all member-self and member-parent references.
  // @todo Maybe this could be moved to _api_update_class_reference_info()
  // and we could just update references in the affected classes, instead of
  // globally?
  db_delete('api_reference_storage')
    ->condition('object_type', 'computed-member')
    ->execute();

  // Figure out the member-self references in reference storage. Basically,
  // ChildClass::foo() calls $this->bar(). We need to figure out the full name
  // of $this->bar(), which could be ChildClass::bar() or
  // SomeParentClass::bar(). The {api_members} table keeps track of this.
  // Do not allow aliases here.
  $select = db_select('api_reference_storage', 'r')
    ->condition('r.object_type', 'member-self');
  $select->innerJoin('api_documentation', 'd', 'r.from_did = d.did');
  $select->innerJoin('api_members', 'm', 'd.class_did = m.class_did');
  $select->innerJoin('api_documentation', 'dm', 'm.did = dm.did AND dm.member_name = r.object_name');
  $select->condition('dm.object_type', 'function');
  $select->isNull('m.member_alias');
  $select->addField('dm', 'namespaced_name', 'object_name');
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
  // Do not allow aliases.
  $select = db_select('api_reference_storage', 'r')
    ->condition('r.object_type', 'member-parent');
  $select->innerJoin('api_documentation', 'cd', 'r.from_did = cd.did');
  $select->innerJoin('api_reference_storage', 'e', 'e.from_did = cd.class_did');
  $select
    ->condition('e.object_type', 'class')
    ->condition('e.extends_did', 0, '>');
  $select->innerJoin('api_members', 'pm', 'pm.class_did = e.extends_did');
  $select->isNull('pm.member_alias');
  $select->innerJoin('api_documentation', 'md', 'md.did = pm.did AND md.member_name = r.object_name');
  $select->condition('md.object_type', 'function');

  $select->addField('md', 'namespaced_name', 'object_name');
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
 * Calculates the current best guess as to the ID of a class.
 *
 * @param string $class_name
 *   The namespaced class name to find.
 * @param int $branch_id
 *   The branch ID this name was referenced in.
 *
 * @return int
 *   The documentation ID of a matching class in the current branch, the core
 *   branch for this core compatibility, or another branch in this core
 *   compatibility, or 0 if no match was found. The match has to be unique
 *   within the scope. If multiple matches are found, 0 will be returned.
 */
function api_best_class_id($class_name, $branch_id) {
  // Check current branch.
  $found = db_select('api_documentation', 'ad')
    ->condition('namespaced_name', $class_name)
    ->condition('branch_id', $branch_id)
    ->fields('ad', array('did'))
    ->execute()
    ->fetchCol();

  if (count($found) == 1) {
    return (int) array_shift($found);
  }
  elseif (count($found) > 1) {
    return 0;
  }

  // Check core branch.
  $branch = api_get_branch_by_id($branch_id);
  $core = api_find_core_branch($branch);
  if ($core) {
    $found = db_select('api_documentation', 'ad')
      ->condition('namespaced_name', $class_name)
      ->condition('branch_id', $core->branch_id)
      ->fields('ad', array('did'))
      ->execute()
      ->fetchCol();
    if (count($found) == 1) {
      return (int) array_shift($found);
    }
    elseif (count($found) > 1) {
      return 0;
    }
  }

  // See if there is a branch with matching core compatibility at least.
  if ($branch) {
    $query = db_select('api_documentation', 'ad');
    $query->innerJoin('api_branch', 'b', 'ad.branch_id = b.branch_id');
    $found = $query
      ->condition('ad.namespaced_name', $class_name)
      ->condition('b.core_compatibility', $branch->core_compatibility)
      ->fields('ad', array('did'))
      ->execute()
      ->fetchCol();
    if (count($found) == 1) {
      return (int) array_shift($found);
    }
  }

  // Didn't find a unique match.
  return 0;
}

/**
 * Updates all of the class reference information for a class.
 *
 * @param int $did
 *   Documentation ID for the class.
 * @param array $parent_info
 *   Array of collected parent information, calculated in api_shutdown().
 * @param array $member_info
 *   Array of collected direct member information, calculated in api_shutdown().
 */
function _api_update_class_reference_info($did, $parent_info, $member_info) {
  if (!$did) {
    return;
  }

  // Calculate the full list of class members, including inherited, and figure
  // out what each one is overriding and where each one is documented.
  $members = _api_calc_class_members($did, $parent_info, $member_info);

  // Save the overrides info, and if we are getting documentation from an
  // inherited member, update the summary on the main object.
  $ao_query = db_insert('api_overrides')
    ->fields(array('did', 'overrides_did', 'documented_did'));
  $new_dids = array();
  foreach ($members as $type => $list) {
    foreach ($list as $member) {
      $new_dids[] = $member['did'];
      if ($member['documented_did'] != $member['did']) {
        db_update('api_documentation')
          ->condition('did', $member['did'])
          ->fields(array('summary' => $member['summary']))
          ->execute();
      }
      unset($member['summary']);
      $ao_query->values($member);
    }
  }
  if (count($new_dids)) {
    // Delete the old information and insert new.
    db_delete('api_overrides')
      ->condition('did', $new_dids)
      ->execute();
    $ao_query->execute();
  }

  // Save the member list.
  db_delete('api_members')
    ->condition('class_did', $did)
    ->execute();
  $query = db_insert('api_members')
    ->fields(array('class_did', 'did', 'member_alias'));
  $has_some = FALSE;
  foreach ($members as $type => $list) {
    foreach ($list as $member) {
      $query->values(array('class_did' => $did, 'did' => $member['did'], 'member_alias' => ($member['alias'] != $member['member_name']) ? $member['alias'] : NULL));
      $has_some = TRUE;
    }
  }
  if ($has_some) {
    $query->execute();
  }
}

/**
 * Calculates class member information for a class, including inheritance.
 *
 * @param int $did
 *   Documentation ID of the class to calculate.
 * @param array $parent_info
 *   Array of collected parent information, calculated in api_shutdown().
 * @param array $member_info
 *   Array of collected direct member information, calculated in api_shutdown().
 */
function _api_calc_class_members($did, $parent_info, $member_info) {
  // We will most likely encounter a class multiple times while going up
  // the hierarchy, so cache results.
  $cache = &backdrop_static(__FUNCTION__, array());

  $members = array(
    'function' => array(),
    'property' => array(),
    'constant' => array(),
  );

  if (!$did || !isset($member_info[$did])) {
    return $members;
  }

  // See if we already calculated members for this class, during another
  // class traversal.
  if (isset($cache[$did])) {
    return $cache[$did];
  }

  // Add this class's direct members to the list, and put it in the
  // cache for now, to avoid loops.
  $members = _api_make_bare_member_list($did, $member_info);
  $cache[$did] = $members;

  // Add in the parents.
  if (!isset($parent_info[$did])) {
    return $members;
  }
  // See if there are any aliases for trait members.
  $aliases = _api_find_trait_aliases($did);

  foreach ($parent_info[$did] as $parent_name => $parent) {
    $parent_members = _api_calc_class_members($parent, $parent_info, $member_info);
    _api_member_list_merge($members, $parent_members, isset($aliases[$parent_name]) ? $aliases[$parent_name] : array());
  }

  // Save in the cache and return.
  $cache[$did] = $members;
  return $members;
}

/**
 * Reads trait method aliases from the database.
 */
function _api_find_trait_aliases($did) {
  $results = db_select('api_namespace', 'an')
    ->condition('did', $did)
    ->condition('object_type', array('trait_alias'))
    ->fields('an', array('class_alias', 'class_name'))
    ->execute();

  // @todo Consider the possibility of insteadof in traits -- right now Core
  // does not use this -- but it would mean excluding some trait methods.
  $return = array();
  foreach ($results as $item) {
    $alias = $item->class_alias;
    $name = $item->class_name;
    $pos = strpos($name, '::');
    if ($pos >= 1) {
      $class = substr($name, 0, $pos);
      $member = substr($name, $pos + 2);
      $return[$class][$member] = $alias;
    }
  }

  return $return;
}

/**
 * Calculates a bare list of this class's direct members.
 */
function _api_make_bare_member_list($did, $member_info) {
  // We will most likely encounter a class multiple times while going up
  // the hierarchy, so cache results.
  $cache = &backdrop_static(__FUNCTION__, array());

  if (isset($cache[$did])) {
    return $cache[$did];
  }

  $members = array(
    'function' => array(),
    'property' => array(),
    'constant' => array(),
  );

  foreach ($member_info[$did] as $type => $list) {
    foreach ($list as $member_did) {
      $item = _api_bare_object_load($member_did, $type);
      if (!$item) {
        continue;
      }
      $has_docs = _api_has_documentation($item);
      $member_name = $item->member_name;
      $members[$type][$member_name] = array(
        'did' => $item->did,
        'overrides_did' => 0,
        'documented_did' => ($has_docs) ? $item->did : 0,
        'summary' => $item->summary,
        'alias' => $member_name,
        'member_name' => $member_name,
      );
    }
  }

  $cache[$did] = $members;
  return $members;
}

/**
 * Merges a member list with parent member list.
 *
 * @param array $members
 *   List of member information, modified by reference.
 * @param array $parent_members
 *   List of parent member information to merge in.
 * @param array $aliases
 *   List of aliases for member names (functions only)
 */
function _api_member_list_merge(&$members, $parent_members, $aliases) {
  foreach ($parent_members as $type => $new_type_list) {
    foreach ($new_type_list as $member_name => $info) {
      $alias = (isset($aliases[$member_name]) && $type == 'function') ? $aliases[$member_name] : $member_name;
      if (isset($members[$type][$alias])) {
        // We already knew about this member. Save override info, if new.
        if (!$members[$type][$alias]['overrides_did']) {
          // We just found what the known member is overriding.
          $members[$type][$alias]['overrides_did'] = $info['did'];
        }
        if (!$members[$type][$alias]['documented_did']) {
          // The old member didn't have documentation, maybe this one does.
          $members[$type][$alias]['documented_did'] = $info['documented_did'];
        }
        if (!$members[$type][$alias]['summary']) {
          $members[$type][$alias]['summary'] = $info['summary'];
        }
      }
      else {
        // It's a new member, add it in.
        $info['alias'] = $alias;
        $members[$type][$alias] = $info;
      }
    }
  }
}

/**
 * Loads a bare documentation object, without overrides.
 *
 * @param int $did
 *   Documentation ID.
 * @param string|array $type
 *   Object type.
 *
 * @return object|null
 *   Documentation object, or NULL if not found.
 */
function _api_bare_object_load($did, $type = '') {
  $cache = &backdrop_static(__FUNCTION__, array());

  if (!is_numeric($did)) {
    return NULL;
  }
  $did = (int) $did;
  if (!$did) {
    return NULL;
  }

  if (isset($cache[$did])) {
    return $cache[$did];
  }

  $query = db_select('api_documentation', 'ad');
  $query->fields('ad');
  $query->condition('ad.did', $did);
  if ($type == 'function') {
    $query->leftJoin('api_function', 'afunc', 'afunc.did = ad.did');
    $query->fields('afunc', array('signature', 'parameters', 'return_value'));
  }
  else  if ($type == 'file') {
    $query->leftJoin('api_file', 'afile', 'afile.did = ad.did');
    $query->fields('afile', array('modified', 'queued'));
  }
  $query->leftJoin('api_branch', 'b', 'ad.branch_id = b.branch_id');
  $query->fields('b', array('project', 'core_compatibility'));

  $query->leftJoin('api_documentation', 'adfile', "adfile.file_name = ad.file_name AND adfile.object_type = 'file' AND adfile.branch_id = ad.branch_id");
  $query->addField('adfile', 'did', 'file_did');
  $query = $query->range(0, 1);
  $result = $query->execute()->fetchObject();

  $cache[$did] = $result;

  return $result;
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
function _api_has_documentation($object) {
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
  $queue = BackdropQueue::get('api_branch_update');

  // Check the files branches to see what needs updates.
  foreach (api_get_branches(TRUE) as $branch) {
    if (api_should_queue($branch)) {
      // Add to queue and mark as queued.
      $queue->createItem(array('type' => 'files', 'branch_id' => $branch->branch_id));
      $branch->queued = time();
      backdrop_write_record('api_branch', $branch, 'branch_id');
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
      backdrop_write_record('api_php_branch', $branch, 'branch_id');
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
  $response = backdrop_http_request($branch->summary);
  if ($response->code === '200') {
    $docblocks = array();
    // Note that backdrop_json_decode() passes TRUE to json_decode(), so that all
    // returned data is associative arrays and not objects.
    $data = backdrop_json_decode($response->data);
    foreach ($data as $name => $item) {
      $docblocks[] = array(
        'branch_id' => $branch->branch_id,
        'object_type' => 'function',
        'object_name' => $name,
        'documentation' => $item['prototype'] . "\n  " . $item['purpose'],
        'member_name' => '',
      );
    }

    if (count($docblocks)) {
      api_save_php_documentation($docblocks);
    }

    // Mark the branch as having been updated.
    $branch->last_updated = time();
    $branch->queued = 0;
    backdrop_write_record('api_php_branch', $branch, array('branch_id'));
    watchdog('api', 'Updated PHP branch %branch_name.', array('%branch_name' => $branch->title));
    return TRUE;
  }

  // We didn't get a good response.
  $branch->queued = 0;
  backdrop_write_record('api_php_branch', $branch, array('branch_id'));
  watchdog('api', 'Bad HTTP response from PHP branch %branch_name.', array('%branch_name' => $branch->title), WATCHDOG_WARNING);
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
  $limit = (int) (isset($branch->page_limit) ? $branch->page_limit : 0);
  $timeout = (int) (isset($branch->timeout) ? $branch->timeout : 0);
  if ($limit <= 0) {
    $limit = 2000;
  }
  if ($timeout <= 0) {
    $timeout = 30;
  }

  // Download JSON by page.
  $base_url = $branch->url . ((strpos($branch->url, '?') > 0) ? '&' : '?') . 'limit=' . $limit . '&page=';

  $docblocks = array();
  $done = array();
  $page = 0;
  $found = TRUE;

  // Continue in this loop as long as the last response was successful and
  // contained new data, but as a failsafe stop after 1000 pages.
  while ($found && $page < 1000) {
    $found = FALSE;
    $response = @backdrop_http_request($base_url . $page, array('timeout' => $timeout));
    $page++;
    if (isset($response) && ($response->code === '200')) {
      $data = backdrop_json_decode($response->data);
      foreach ($data as $item) {
        $item = (array) $item;
        // Only save new items.
        $key = $item['url'] . '..' . $item['object_type'] . '..' . $item['namespaced_name'];
        if (!isset($done[$key])) {
          $found = TRUE;
          $done[$key] = TRUE;
          $item['branch_id'] = $branch->branch_id;
          $docblocks[] = $item;
        }
      }
    }
  }

  if (count($docblocks)) {
    // We found at least one doc block, so save the data.
    api_save_api_documentation($docblocks);

    // Mark the branch as having been updated.
    $branch->last_updated = time();
    $branch->queued = 0;
    backdrop_write_record('api_php_branch', $branch, array('branch_id'));
    watchdog('api', 'Updated API branch %branch_name in %count page requests.', array('%branch_name' => $branch->title, '%count' => $page));
    return TRUE;
  }

  // We didn't get a good response.
  $branch->queued = 0;
  backdrop_write_record('api_php_branch', $branch, array('branch_id'));
  watchdog('api', 'No documentation was downloaded from API branch %branch_name.', array('%branch_name' => $branch->title), WATCHDOG_WARNING);
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
    'twig' => 'api_parse_twig_file',

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
    backdrop_write_record('api_branch', $branch, array('branch_id'));
    return FALSE;
  }

  $parse_functions = api_parse_functions();

  // List all documented files for the branch.
  $files = array();
  $query = db_select('api_documentation', 'd')
    ->fields('d', array('object_name', 'did'));
  $f = $query->leftJoin('api_file', 'f', 'd.did = f.did');
  $query->fields($f, array('modified', 'queued'))
    ->condition('d.branch_id', $branch->branch_id)
    ->condition('d.object_type', 'file');
  $result = $query->execute();
  foreach ($result as $file) {
    $files[$file->object_name] = $file;
  }

  // Check all the files. If they need an update, add them to the queue.
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
        $queue = BackdropQueue::get('api_parse');
        $queue->createItem(array('parser' => $parse_functions[$matches[1]], 'path' => $path, 'branch' => $branch, 'file' => $file_name, 'branch_name' => $branch->branch_name, 'timestamp' => time()));

        // Make sure this doesn't get added to the parse queue again.

        if ($new) {
          // Add stub to the {api_documentation} and {api_file} table for this
          // file so it doesn't get added to the parse queue again.
          $nid = api_new_documentation_node();
          $doc = array(
            'object_name' => $file_name,
            'file_name' => $file_name,
            'title' => backdrop_basename($file_name),
            'object_type' => 'file',
            'branch_id' => $branch->branch_id,
            'queued' => time(),
            'did' => $nid,
          );
          _api_add_text_defaults($doc, 'api_documentation');
          backdrop_write_record('api_documentation', $doc);
          backdrop_write_record('api_file', $doc);
        }
        else {
          // Update the 'queued' field in the {api_file} table so this does not
          // get queued again.
          $file->queued = time();
          backdrop_write_record('api_file', $file, 'did');
        }
      }
    }
  }

  // Remove outdated files.
  foreach (array_keys($files) as $file_name) {
    watchdog('api', 'Removing %file from branch %branch.', array('%file' => $file_name, '%branch' => $branch->branch_name));
    $dids = db_select('api_documentation', 'ad')
      ->fields('ad', array('did'))
      ->condition('file_name', $file_name)
      ->condition('branch_id', $branch->branch_id)
      ->execute()
      ->fetchCol();
    api_delete_items($dids, FALSE);
  }

  // Mark the branch as having been updated.
  $branch->last_updated = time();
  $branch->queued = 0;
  backdrop_write_record('api_branch', $branch, array('branch_id'));
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
 *   Record about to be written to the database with backdrop_write_record().
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
  $node = new Node(array(
    'type' => 'api',
    'uid' => 0,
    'status' => 1,
  ));
  $node->save();

  return $node->nid;
}
