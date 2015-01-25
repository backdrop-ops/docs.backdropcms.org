<?php

/**
 * @file
 * Pages (except administrative) for the API module.
 */

module_load_include('inc', 'api', 'api.formatting');

/**
 * Page callback: Redirects to an appropriate search page.
 *
 * @param ...
 *   Search terms.
 *
 * @see api_menu()
 */
function api_search_redirect() {
  // See what we should search for.
  $args = func_get_args();
  if (count($args) === 0 && isset($_GET['destination']) && strpos($_GET['destination'], 'apis/') !== 0) {
    // Handling 404: search for the unknown page path.
    $tail = $_GET['destination'];
  }
  else {
    // Called directly: search for whatever was after apis/ in the path.
    $tail = implode('/', $args);
  }

  $tail = trim($tail);

  // Find the first branch with an exact match to this term.
  $query = db_select('api_documentation', 'ad');
  $query->innerJoin('api_branch', 'b', 'ad.branch_id = b.branch_id');
  $results = $query
    ->condition('ad.title', $tail)
    ->orderBy('b.weight')
    ->range(0,1)
    ->fields('b', array('project', 'branch_name'))
    ->execute();
  foreach ($results as $branch) {
    // If we found an exact result, redirect to that branch's search page.
    $_GET['destination'] = 'api/' . $branch->project . '/' . $branch->branch_name . '/search/' . $tail;
    backdrop_goto();
  }

  // If we get here, there was not an exact match, so look for a partial match.
  $query = db_select('api_documentation', 'ad');
  $query->innerJoin('api_branch', 'b', 'ad.branch_id = b.branch_id');
  $results = $query
    ->condition('ad.title', '%' . $tail . '%', 'LIKE')
    ->orderBy('b.weight')
    ->range(0,1)
    ->fields('b', array('project', 'branch_name'))
    ->execute();
  foreach ($results as $branch) {
    // We found a partial match, so redirect to that branch's search page.
    $_GET['destination'] = 'api/' . $branch->project . '/' . $branch->branch_name . '/search/' . $tail;
    backdrop_goto();
  }

  // If we get here, there was not a match.
  return t('Sorry, %term cannot be found', array('%term' => $tail));
}

/**
 * Page callback: Prints out OpenSearch plugin discovery XML output.
 *
 * @see https://developer.mozilla.org/en/Creating_OpenSearch_plugins_for_Firefox
 * @see api_menu()
 */
function api_opensearch() {
  backdrop_add_http_header('Content-Type', 'text/xml; charset=utf-8');

  $short_name = config_get('api.settings', 'opensearch_name');
  $description = config_get('api.settings', 'opensearch_description');
  $search_url = url('apis', array('absolute' => TRUE)) . '/{searchTerms}';
  $suggest_url = url('api/suggest', array('absolute' => TRUE)) . '/{searchTerms}';
  $self_url = url($_GET['q'], array('absolute' => TRUE));

  // We need to use a theme function, so initialize the theme system.
  backdrop_theme_initialize();
  if (!($image = theme_get_setting('favicon'))) {
    // Fall back on default favicon if the theme didn't provide one.
    $image = 'misc/favicon.ico';
  }
  $image = url($image, array('absolute' => TRUE));

  print <<<EOD
<?xml version="1.0" encoding="UTF-8"?>
<OpenSearchDescription xmlns="http://a9.com/-/spec/opensearch/1.1/">
  <ShortName>$short_name</ShortName>
  <Description>$description</Description>
  <Image width="16" height="16" type="image/x-icon">$image</Image>
  <Url type="text/html" rel="results" method="GET" template="$search_url" />
  <Url type="application/x-suggestions+json" rel="suggestions" template="$suggest_url"/>
  <Url type="application/opensearchdescription+xml" rel="self" template="$self_url" />
</OpenSearchDescription>
EOD;
}

/**
 * Page callback: Prints JSON-formatted potential matches for OpenSearch.
 *
 * @param $search
 *   The string to search for.
 *
 * @see http://www.opensearch.org/Specifications/OpenSearch/Extensions/Suggestions/1.0
 * @see api_menu()
 */
function api_suggest($search = '') {
  $query = db_select('api_documentation', 'd')
    ->fields('d', array('title'));
  $matches = $query->condition('d.title', '%' . db_like($search) . '%', 'LIKE')
    ->range(0, 10)
    ->execute()
    ->fetchCol();
  print backdrop_json_output(array($search, array_values(array_unique($matches))));
}

/**
 * Page callback: Prints a list of functions in text format, for use in IDEs.
 *
 * @param $branch_name
 *   Name of the branch to list functions for.
 *
 * @see api_menu()
 */
function api_page_function_dump($branch_name) {
  $query = db_select('api_documentation', 'd')
    ->fields('d', array('summary'))
    ->orderBy('d.title');
  $f = $query->innerJoin('api_function', 'f', 'd.did = f.did');
  $query->fields('f', array('signature'));
  $query->innerJoin('api_branch', 'b', 'd.branch_id = b.branch_id');
  $query->condition('b.branch_name', $branch_name)
    ->condition('d.class_did', 0)
    ->condition('d.object_type', 'function');
  $result = $query->execute();
  foreach ($result as $object) {
    print($object->signature);
    // Make sure the summary is free of HTML tags and newlines.
    $summary = $object->summary;
    $summary = strip_tags($summary);
    $summary = preg_replace('|\s+|', ' ', $summary);
    print(' ### ' . $summary . "\n");
  }
}

/**
 * Page callback: Outputs a list of all documentation objects in JSON format.
 *
 * Note that this function outputs the JSON data directly rather than returning
 * it. Also, query parameters 'limit' and 'page' can be used to limit to a
 * certain number of results, and to get subsequent pages (both are integers and
 * page numbering starts at 0; you can omit 'page' on the first page as well).
 *
 * @param object $branch
 *   Object representing the branch to dump objects for.
 *
 * @see api_menu()
 */
function api_dump_full_list($branch) {
  $query = db_select('api_documentation', 'd')
    ->fields('d', array('object_name', 'namespaced_name', 'member_name', 'title', 'summary', 'object_type', 'branch_id', 'file_name'))
    ->condition('d.branch_id', $branch->branch_id)
    ->condition('d.object_type', 'mainpage', '<>')
    ->orderBy('d.object_name');

  // See if there is a limit or a page number.
  $params = backdrop_get_query_parameters();
  if (isset($params['limit'])) {
    $page = isset($params['page']) ? (int) $params['page'] : 0;
    $limit = (int) $params['limit'];
    $start = $page * $limit;

    $query->range($start, $limit);
  }

  $result = $query->execute();

  $objs = array();
  foreach ($result as $doc) {
    $objs[] = array(
      'object_name' => $doc->object_name,
      'namespaced_name' => $doc->namespaced_name,
      'title' => $doc->title,
      'member_name' => $doc->member_name,
      'summary' => $doc->summary,
      'object_type' => $doc->object_type,
      'url' => url(api_url($doc), array('absolute' => TRUE)),
    );
  }

  backdrop_json_output($objs);
}

/**
 * Page callback: Performs a search.
 *
 * @param $branch
 *   Object representing the branch to search.
 * @param ...
 *   The string to search for.
 *
 * @return
 *   HTML output for the search results. If there is exactly one result,
 *   redirects to that page instead of displaying the results page.
 *
 * @see api_menu()
 */
function api_search_listing($branch) {
  // Read the search text from the ... part of the arguments.
  $search_text = func_get_args();
  array_shift($search_text);
  $search_text = trim(implode('/', $search_text));

  // If there is exactly one exact match, go directly to that page.
  $query = db_select('api_documentation', 'ad');
  $query
    ->fields('ad')
    ->condition('branch_id', $branch->branch_id)
    ->condition('title', $search_text);

  // Unfortunately, MySQL is not case-sensitive. So, to figure out if there
  // is exactly one case-sensitive match, we need to look at the results.
  $results = $query->execute();
  $exact = NULL;
  foreach ($results as $item) {
    if ($item->title == $search_text) {
      if ($exact) {
        // We have now found multiple results. On to the next step.
        $exact = NULL;
        break;
      }
      else {
        // This is an exact match. Store it.
        $exact = $item;
      }
    }
  }

  if ($exact) {
    // We found one exact match.
    backdrop_goto(api_url($exact));
  }

  // If we get here, there were multiple or zero matches.
  $title = t('Search for !search', array('!search' => $search_text));
  backdrop_set_title($title);
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('API reference'), 'api/' . $branch->project),
    l($branch->title, 'api/' . $branch->project . '/' . $branch->branch_name),
  );
  backdrop_set_breadcrumb($breadcrumb);
  $plain_title = t('Search for @search', array('@search' => $search_text));
  $page_title = array(
    check_plain($plain_title),
    check_plain($branch->title),
    check_plain(config_get('system.core', 'site_name')),
  );
  api_set_html_page_title(implode(' | ', $page_title));

  $output = api_display_view('api_search', 'block_search_results', array($branch->branch_id, $search_text));

  return api_search_links($branch, $search_text) . $output;
}

/**
 * Renders a list of links to search other branches for the same text.
 *
 * @param $branch
 *   The current branch being searched.
 * @param $search_text
 *   The text being searched for.
 *
 * @return
 *   HTML for links to search other branches for the same text, and a link to
 *   the Projects page.
 */
function api_search_links($branch, $search_text) {
  $search_text = check_plain($search_text);
  $others = db_select('api_branch', 'b')
    ->fields('b')
    ->condition('b.project', $branch->project)
    ->condition('b.branch_id', $branch->branch_id, '<>')
    ->orderBy('b.branch_name')
    ->execute();

  $links = array();
  foreach ($others as $other) {
    $links[] = '<li>' . l(t('Search @branch_name for !search_text', array('@branch_name' => $other->title, '!search_text' => $search_text)), 'api/' . $other->project . '/' . $other->branch_name . '/search/' . $search_text) . '</li>';
  }

  $other = api_other_projects_link(FALSE);
  if (strlen($other)) {
    $links[] = '<li>' . $other . '</li>';
  }

  if (!count($links)) {
    return '';
  }

  return '<ol id="api-alternatives">' . implode("\n", $links) . '</ol>';
}

/**
 * Page callback: Displays a list of links to projects using a pager.
 *
 * @return
 *   Themed HTML output for the page.
 */
function api_page_projects() {
  $output = array();

  $output['project_form'] = backdrop_get_form('api_project_form');

  $links = api_display_view('api_projects', 'block_project_list', array());
  if ($links) {
    $output['project_list'] = array(
      '#type' => 'markup',
      '#markup' => $links,
    );
  }

  return $output;
}

/**
 * Form constructor for the project select autocomplete form.
 */
function api_project_form($form, $form_state) {
  $form['project'] = array(
    '#type' => 'textfield',
    '#autocomplete_path' => 'api/project/autocomplete',
    '#title' => t('Project short name'),
    '#default_value' => !empty($form_state['values']['project']) ? $form_state['values']['project'] : '',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Go'),
  );

  return $form;
}

/**
 * Form submission handler for api_project_form().
 */
function api_project_form_submit($form, &$form_state) {
  $form_state['redirect'] = 'api/' . $form_state['values']['project'];
}

/**
 * Page callback: Displays documentation for a file.
 *
 * @param $file
 *   Documentation object representing the file to display.
 *
 * @return
 *   HTML output for the file documentation page.
 *
 * @see api_menu()
 */
function api_page_file($file) {
  $branch = api_get_branch_by_id($file->branch_id);

  api_object_title_and_breadcrumb($branch, $file);

  $documentation = api_link_documentation($file->documentation, $branch, $file->did, NULL, FALSE, FALSE, $file->is_backdrop);
  $see = api_link_documentation($file->see, $branch, $file->did, NULL, TRUE, TRUE, $file->is_backdrop);
  $deprecated = api_link_documentation($file->deprecated, $branch, $file->did, NULL, TRUE, FALSE, $file->is_backdrop);
  $related_topics = api_display_view('api_references', 'block_related_topics', array($file->did));

  $code = api_link_code($file->code, $branch, $file->did, NULL, $file->is_backdrop);
  $defined = $file->file_name;

  $types = array(
    'function' => t('Functions'),
    'constant' => t('Constants'),
    'global' => t('Globals'),
    'class' => t('Classes'),
    'interface' => t('Interfaces'),
    'trait' => t('Traits'),
    'service' => t('Services'),
  );

  $objects = array();
  foreach ($types as $type => $label) {
    // Render the view of this type of object in this file.
    $out = api_display_view('api_listings', 'block_items_file', array($file->object_name, $type, $file->branch_id));

    // Only display if there was something there.
    if ($out) {
      $objects[$type] = array(
        '#type' => 'markup',
        '#markup' => '<h3>' . $label . '</h3>' . $out,
      );
    }
  }

  $objects = backdrop_render($objects);

  // If this is a theme template or YML file, make reference link.
  $links = api_build_references(array('theme_invokes', 'theme_references', 'yml_config', 'yml_keys'), $file, $branch);

  $output = theme('api_file_page', array('object' => $file, 'documentation' => $documentation, 'objects' => $objects, 'code' => $code, 'see' => $see, 'deprecated' => $deprecated, 'related_topics' => $related_topics, 'defined' => $defined, 'call_links' => $links));
  $output .= _api_add_comments($file);
  return $output;
}

/**
 * Page callback: Generates the default documentation page for a branch.
 *
 * @param $branch
 *   Branch object giving the branch to display documentation for.
 *
 * @return
 *   HTML output for the page.
 *
 * @see api_menu()
 */
function api_page_branch($branch) {
  // Set HTML page title.
  $title = t('API reference');
  $page_title = array(
    check_plain($branch->title),
    check_plain($title),
    check_plain(config_get('system.core', 'site_name')),
  );
  api_set_html_page_title(implode(' | ', $page_title));

  $docs = db_select('api_documentation', 'd')
    ->fields('d', array('documentation'))
    ->condition('branch_id', $branch->branch_id)
    ->condition('object_type', 'mainpage')
    ->execute()
    ->fetchObject();
  if ($docs) {
    return api_link_documentation($docs->documentation, $branch);
  }
  else {
    return theme('api_branch_default_page', array('branch' => $branch));
  }
}

/**
 * Page callback: Generates a listing page for an object type.
 *
 * @param $branch
 *   Object representing branch to generate the listing for.
 * @param $object_type
 *   Type of object to list ('file', 'group', etc.), or 'deprecated' for the
 *   Deprecated page.
 * @param $is_page
 *   TRUE if this is on its own page, FALSE if it is included in another page.
 *
 * @return
 *   HTML for the listing page.
 */
function api_page_listing($branch, $object_type, $is_page = TRUE) {
  // Set the HTML page title and breadcrumb.
  if ($is_page) {
    $breadcrumb = array(
      l(t('Home'), '<front>'),
      l(t('API reference'), 'api/' . $branch->project),
      l($branch->title, 'api/' . $branch->project . '/' . $branch->branch_name),
    );
    backdrop_set_breadcrumb($breadcrumb);

    $titles = array(
      'class' => t('Classes, traits, and interfaces'),
      'constant' => t('Constants'),
      'file' => t('Files'),
      'function' => t('Functions'),
      'global' => t('Globals'),
      'group' => t('Topics'),
      'namespace' => t('Namespaces'),
      'deprecated' => t('Deprecated'),
      'service' => t('Services'),
    );
    $title = $titles[$object_type];
    backdrop_set_title($title);

    $page_title = array(
      check_plain($title),
      check_plain($branch->title),
      check_plain(config_get('system.core', 'site_name')),
    );
    api_set_html_page_title(implode(' | ', $page_title));
  }

  $output = array();

  if ($object_type === 'function' || $object_type == 'constant' || $object_type == 'global' ) {
    $output['listing'] = array(
      '#type' => 'markup',
      '#markup' => api_display_view('api_listings', 'block_listing', array($object_type, $branch->branch_id)),
    );
  }
  elseif ($object_type === 'group' ) {
    $output['listing'] = array(
      '#type' => 'markup',
      '#markup' => api_display_view('api_listings', 'block_groups', array($branch->branch_id)),
    );
  }
  elseif ($object_type === 'service' ) {
    $output['listing'] = array(
      '#type' => 'markup',
      '#markup' => api_display_view('api_listings', 'block_services', array($branch->branch_id)),
    );
  }
  elseif ( $object_type === 'file') {
    $output['listing'] = array(
      '#type' => 'markup',
      '#markup' => api_display_view('api_listings', 'block_files', array($branch->branch_id)),
    );
  }
  elseif ($object_type === 'class') {
    $output['listing'] = array(
      '#type' => 'markup',
      '#markup' => api_display_view('api_listings', 'block_class', array($branch->branch_id)),
    );
  }
  elseif ($object_type === 'namespace') {
    $output['listing'] = array(
      '#type' => 'markup',
      '#markup' => api_display_view('api_namespaces', 'block_namespace_list', array($branch->branch_id)),
    );
  }
  elseif ($object_type === 'deprecated' ) {
    $output['listing'] = array(
      '#type' => 'markup',
      '#markup' => api_display_view('api_listings', 'block_deprecated', array($branch->branch_id)),
    );
  }

  if ($is_page) {
    $output['suffix'] = array(
      '#type' => 'markup',
      '#markup' => api_other_projects_link(),
    );
  }

  return $output;
}

/**
 * Page callback: Generates a namespace page.
 *
 * @param $project
 *   Short name of project to generate page for.
 * @param $branch_name
 *   Short name of branch to generate page for.
 * @param $namespace
 *   Namespace to generate page for (with substitution characters).
 *
 * @return
 *   HTML for the namespace page.
 */
function api_page_namespace($project, $branch_name, $namespace) {

  $branch = api_get_branch_by_name($project, $branch_name);
  if (is_null($branch)) {
    return MENU_NOT_FOUND;
  }

  $namespace = str_replace(API_FILEPATH_SEPARATOR_REPLACEMENT, API_NAMESPACE_SEPARATOR, $namespace);

  // Set the HTML page title and breadcrumb.
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('API reference'), 'api/' . $branch->project),
    l($branch->title, 'api/' . $branch->project . '/' . $branch->branch_name),
  );
  backdrop_set_breadcrumb($breadcrumb);

  // Do not use t() here since this is a PHP construct.
  $title = 'namespace ' . $namespace;
  backdrop_set_title($title);

  $page_title = array(
    check_plain($title),
    check_plain($branch->title),
    check_plain(config_get('system.core', 'site_name')),
  );
  api_set_html_page_title(implode(' | ', $page_title));

  $output = array();

  $output['listing'] = array(
    '#type' => 'markup',
    '#markup' => api_display_view('api_namespaces', 'block_items_namespace', array($branch->branch_id, $namespace)),
  );

  return $output;
}

/**
 * Generates a link to the projects page if there is more than one project.
 *
 * @param $wrap_in_p
 *   TRUE to wrap in a P tag; FALSE to not do that.
 * @return
 *   Link to the API projects page, maybe inside a P tag, or an empty string if
 *   there would only be one project to display on that page.
 */
function api_other_projects_link($wrap_in_p = TRUE) {
  // Figure out how many different projects there are.
  $count = db_select('api_project', 'p')
    ->countQuery()
    ->execute()
    ->fetchField();
  if ($count > 1) {
    $text = '';
    if ($wrap_in_p) {
      $text .= '<p class="api-switch"><strong>';
    }
    $text .= l(t('Other projects'), 'api/projects');
    if ($wrap_in_p) {
      $text .= '</strong></p>';
    }

    return $text;
  }

  return '';
}

/**
 * Page callback: Displays documentation for a function.
 *
 * @param $function
 *   Documentation object representing the function to display.
 *
 * @return
 *   HTML output for the page.
 *
 * @see api_menu()
 */
function api_page_function($function) {
  $branches = api_get_branches();
  $branch = $branches[$function->branch_id];

  api_object_title_and_breadcrumb($branch, $function);

  // Make the signatures section, which lists all the other function signatures
  // (branches and duplicate function names) that we can find. Limit this to
  // the current project.
  $last_signature = '';
  $signatures = array();
  $n = 0;
  $query = db_select('api_documentation', 'd')
    ->fields('d', array('branch_id', 'file_name', 'object_type', 'object_name', 'class_did'));
  $f = $query->innerJoin('api_function', 'f', 'f.did = d.did');
  $query->fields($f, array('signature'));
  $b = $query->innerJoin('api_branch', 'b', 'd.branch_id = b.branch_id');
  $query->condition('d.object_type', 'function')
    ->condition('d.title', $function->title)
    ->condition('b.project', $branch->project)
    ->orderBy($b . '.weight');
  $result = $query->execute();
  foreach ($result as $signature) {
    $signature_branch = $branches[$signature->branch_id];
    $branch_label = $signature_branch->branch_name . ' ' . basename($signature->file_name);
    $active = ($signature->branch_id === $function->branch_id && $signature->file_name == $function->file_name);

    // Build up the function prefix/class/name. Note that token_get_all() does
    // not work unless the code starts with the open PHP token.
    $tokens = token_get_all('<' . '?php ' . $signature->signature);

    // Remove open PHP token.
    array_shift($tokens);

    // Get the prefix -- everything up to the function name, such as "public",
    // etc.
    $name = '';
    while ($tokens[1] !== '(') {
      if (is_array($tokens[0])) {
        $name .= $tokens[0][1];
      }
      else {
        $name .= $tokens[0];
      }
      array_shift($tokens);
    }

    // Add the class name, if there is one. This is not part of the signature.
    if ($signature->class_did !== '0') {
      $class = api_object_load((int) $signature->class_did, $signature_branch, array('interface', 'class', 'trait'), $signature->file_name);
      if (isset($class)) {
        $name .= l($class->object_name, api_url($class)) . '::';
      }
    }

    // Add the actual function name, plain (active branch) or link (not
    // active branch).
    if ($active) {
      $name .= $tokens[0][1];
    }
    else {
      $name .= l($tokens[0][1], api_url($signature));
    }

    // Store this much for the second pass.
    $signature_info[$n] = array(
      'branch_label' => $branch_label,
      'object' => $signature,
      'active' => $active,
      'arguments' => array(),
      'other' => array(),
      'name' => $name,
    );

    // Collect the argument list, separating it out into the argument names
    // and other info (such as default values).
    $start = TRUE;
    $depth = 0;
    $arg_index = -1;
    $signature_info[$n]['other'][$arg_index] = '';
    foreach ($tokens as $token) {
      $depth += in_array($token, array('(', '{', '[')) - in_array($token, array(')', '}', ']'));
      if ($depth == 1 && $start && is_array($token) && $token[0] == T_VARIABLE) {
        // New argument
        $arg_index += 1;
        $signature_info[$n]['arguments'][$arg_index] = $token[1];
        $signature_info[$n]['other'][$arg_index] = '';
        $start = FALSE;
      }
      elseif ($depth >= 1 && is_array($token)) {
        // Non-arg other information.
        $signature_info[$n]['other'][$arg_index] .= $token[1];
      }
      elseif ($depth >= 1) {
        // Non-arg other information.
        $signature_info[$n]['other'][$arg_index] .= $token;
        // Start looking for a new argument if we see a comma.
        $start = $start || ($depth == 1 && $token == ',');
      }
    }
    $last_signature = $signature->signature;
    $n++;
  }

  // Format the argument lists, keeping track of when arguments come and go
  // through the branches.
  foreach ($signature_info as $n => $info) {
    $new = array();
    if (isset($signature_info[$n - 1])) {
      $new = array_diff($info['arguments'], $signature_info[$n - 1]['arguments']);
    }
    $old = array();
    if (isset($signature_info[$n + 1])) {
      $old = array_diff($info['arguments'], $signature_info[$n + 1]['arguments']);
    }

    $signature = $info['name'] . $info['other'][-1];
    foreach ($signature_info[$n]['arguments'] as $key => $argument) {
      if (in_array($argument, $old)) {
        $signature .= '<del>' . $argument . '</del>';
      }
      elseif (in_array($argument, $new)) {
        $signature .= '<ins>' . $argument . '</ins>';
      }
      else {
        $signature .= $argument;
      }
      $signature .= $info['other'][$key];
    }
    $signature .= ')';
    $signatures[$n] = array(
      'label' => $info['branch_label'],
      'signature' => $signature,
      'active' => $info['active'],
      'url' => api_url($info['object']),
    );
  }

  // Build the page sections.
  $documentation = api_link_documentation($function->documentation, $branch, $function->file_did, $function->class_did, FALSE, FALSE, $function->is_backdrop);

  $parameters = api_link_documentation($function->parameters, $branch, $function->file_did, $function->class_did, TRUE, FALSE, $function->is_backdrop);

  $return = api_link_documentation($function->return_value, $branch, $function->file_did, $function->class_did, TRUE, FALSE, $function->is_backdrop);
  $see = api_link_documentation($function->see, $branch, $function->file_did, $function->class_did, TRUE, TRUE, $function->is_backdrop);
  $deprecated = api_link_documentation($function->deprecated, $branch, $function->file_did, $function->class_did, TRUE, FALSE, $function->is_backdrop);
  $throws = api_link_documentation($function->throws, $branch, $function->file_did, $function->class_did, TRUE, FALSE, $function->is_backdrop);
  $code = api_link_code($function->code, $branch, $function->file_did, $function->class_did, $function->is_backdrop);
  $related_topics = api_display_view('api_references', 'block_related_topics', array($function->did));
  $class = api_class_section($function, $branch);

  // Build reference links.
  $links = api_build_references(array('calls', 'references', 'implementations', 'invokes', 'theme_invokes', 'theme_references', 'overrides'), $function, $branch);

  // Put it all together and theme the output.
  $output = theme('api_function_page', array('branch' => $branch, 'object' => $function, 'signatures' => $signatures, 'documentation' => $documentation, 'parameters' => $parameters, 'return' => $return, 'related_topics' => $related_topics, 'call_links' => $links, 'code' => $code, 'see' => $see, 'deprecated' => $deprecated, 'throws' => $throws, 'class' => $class));
  $output .= _api_add_comments($function);
  return $output;
}

/**
 * Counts or lists references to a function or file.
 *
 * @param $name
 *   Name of the function/file to check (see $type), or the object itself.
 * @param $branch
 *   Object representing the branch to check.
 * @param $type
 *   Type of reference. One of:
 *   - 'calls': $name is the name of a function, find function calls.
 *   - 'implementations': $name is the name of a hook, find implementations.
 *   - 'invokes': $name is the name of a hook, find invocations.
 *   - 'theme_invokes': $name is the name of a theme function or file, find
 *     theme calls.
 *   - 'theme_references': $name is the name of a theme function or file, find
 *     string references to the theme hook name.
 *   - 'references': $name is the name of a function, find string references.
 *   - 'overrides': $exclude_did is the ID of a method, find overriding methods.
 *   - 'constants': $name is the name of a constant, find uses.
 *   - 'uses': $name is the fully-namespaced name of a class, find files that
 *     have use declarations.
 *   - 'annotations': $exclude_did is the ID of an annotation class. Find
 *     classes using it as annotation.
 *   - 'yml_config': $name is a YML config file name, find string references
 *     to the file name without .yml.
 *   - 'yml_keys': $name is a YML config file name, and $exclude_did is its
 *     documentation ID. Find references to keys in this file.
 *   - 'services': $name is a class, find services that reference it.
 *   - 'use': $name is a service, find functions that reference its name.
 * @param $count
 *   If TRUE (default), return a count. If FALSE, return an array.
 * @param $exclude_did
 *   (optional) Document ID to exclude from the query, or to use as the ID for
 *   overrides.
 * @param $limit
 *   (optional) Limit the number of references returned to this amount.
 * @param bool $is_backdrop
 *   (optional) If set to FALSE, and $type is a Backdrop-specific type (related
 *   to themes, hooks, or YAML), just return an empty list or zero count.
 *
 * @return
 *   The number of references or an array of references.
 */
function api_find_references($name, $branch, $type, $count = TRUE, $exclude_did = 0, $limit = 0, $is_backdrop = TRUE) {

  // Early return: if this is a Backdrop type and $is_backdrop is FALSE.
  $backdrop_types = array('implementations', 'theme_invokes', 'theme_references', 'invokes', 'yml_config', 'yml_keys');
  if (!$is_backdrop && in_array($type, $backdrop_types)) {
    return ($count) ? 0 : array();
  }

  if (!is_string($name)) {
    // It should be an object. Derive the name.
    $obj = $name;
    $name = ($obj->class_did > 0 && ($type == 'calls' || $type == 'constants')) ? $obj->namespaced_name : $obj->title;
    // In a few cases, we want to look for either the base name or the
    // namespaced name.
    $altname = $obj->namespaced_name;
    if ($type == 'uses' || $type == 'services') {
      $name = $altname;
    }
  }

  $references_type = 'potential callback';
  $group = FALSE;
  if ($type == 'yml_config') {
    // Verify this is a *.yml file. If not, just return.
    if (strpos($name, '.yml') != strlen($name) - 4) {
      return ($count) ? 0 : array();
    }
    // Strip off the .yml and look for string references.
    $name = substr($name, 0, strlen($name) - 4);
    $type = 'references';
    $references_type = 'potential file';
  }

  $matches = array();
  $branch_id = $branch->branch_id;
  $base_query = NULL;
  $args = array();

  if ($type == 'calls' || $type == 'constants') {
    // Use reference storage to find functions that call this one, or
    // use this constant.
    $base_query = db_select('api_reference_storage', 'r');
    $d = $base_query->leftJoin('api_documentation', 'd', 'r.from_did = d.did');
    $base_query->condition('d.branch_id', $branch_id)
      ->condition('d.did', $exclude_did, '<>')
      ->condition('r.object_type', array('function', 'constant', 'member-class', 'computed-member'))
      ->condition('r.object_name', $name);
  }
  elseif ($type == 'overrides') {
    // Use overrides storage to find overrides of the ID passed in as "exclude".
    $did = $exclude_did;
    $base_query = db_select('api_overrides', 'o');
    $d = $base_query->leftJoin('api_documentation', 'd', 'o.did = d.did');
    $base_query->condition('d.branch_id', $branch_id)
      ->condition('o.overrides_did', $exclude_did);
  }
  elseif ($type == 'references') {
    // Use reference storage to find functions that use this name as a string.
    $base_query = db_select('api_reference_storage', 'r');
    $d = $base_query->innerJoin('api_documentation', 'd', 'r.from_did = d.did');
    $base_query->condition($d . '.branch_id', $branch_id)
      ->condition($d . '.did', $exclude_did, '<>')
      ->condition('r.object_type', $references_type);
    if (isset($altname)) {
      $base_query->condition('r.object_name', array($name, $altname));
    }
    else {
      $base_query->condition('r.object_name', $name);
    }
  }
  elseif ($type == 'use') {
    // Use reference storage to find functions that reference a service. It
    // could have a . in it, so it is either stored as 'potential file' or
    // 'potential callback'.
    $base_query = db_select('api_reference_storage', 'r');
    $d = $base_query->innerJoin('api_documentation', 'd', 'r.from_did = d.did');
    $base_query->condition($d . '.branch_id', $branch_id)
      ->condition($d . '.did', $exclude_did, '<>')
      ->condition('r.object_type', array('potential file', 'potential callback'));
    $base_query->condition('r.object_name', $name);
  }
  if ($type == 'services') {
    // Use reference storage to find services using this class.
    $base_query = db_select('api_reference_storage', 'r');
    $d = $base_query->leftJoin('api_documentation', 'd', 'r.from_did = d.did');
    $base_query->condition('d.branch_id', $branch_id)
      ->condition('d.did', $exclude_did, '<>')
      ->condition('r.object_type', array('service_class'))
      ->condition('r.object_name', $name);
  }
  elseif ($type == 'annotations') {
    // In this case, $exclude_did is the documentation ID of a class, presumably
    // with an 'annotation_class' reference in the table. Find other classes
    // that annotated with this same class name, which will have 'annotation'
    // references.
    $base_query = db_select('api_reference_storage', 'r_annotation')
      ->condition('r_annotation.from_did', $exclude_did)
      ->condition('r_annotation.object_type', 'annotation_class');
    $base_query->innerJoin('api_reference_storage', 'r_uses', "r_annotation.object_name = r_uses.object_name AND r_uses.object_type = 'annotation'");
    $base_query->innerJoin('api_documentation', 'd', 'r_uses.from_did = d.did');
    $base_query
      ->condition('d.branch_id', $branch_id);
    $group = TRUE;
  }
  elseif ($type == 'yml_keys') {
    // In this case, $exclude_did is the documentation ID of a file. We
    // want to find the string keys that file has in reference storage, and
    // then match those with 'potential callback' or 'potential file'
    // references.
    $base_query = db_select('api_reference_storage', 'r');
    $base_query->innerJoin('api_documentation', 'd', 'r.from_did = d.did');
    $base_query
      ->condition('d.branch_id', $branch_id)
      ->condition('r.object_type', array('potential callback', 'potential file'));
    $base_query->innerJoin('api_reference_storage', 'rstrings', 'r.object_name = rstrings.object_name');
    $base_query
      ->condition('rstrings.from_did', $exclude_did)
      ->condition('rstrings.object_type', 'yaml string');
    // A given function/method might have multiple matching YML keys, so we
    // need to group this query.
    $group = TRUE;
  }
  elseif ($type == 'uses') {
    // Use namespace storage to find files with use declarations for this class.
    // Note that $name probably starts with a backslash, while use declarations
    // do not.
    $name = substr(api_full_classname($name), 1);
    $base_query = db_select('api_namespace', 'n');
    $d = $base_query->leftJoin('api_documentation', 'd', 'n.did = d.did');
    $base_query->condition('d.branch_id', $branch_id)
      ->condition('d.did', $exclude_did, '<>')
      ->condition('n.object_type', 'use_alias')
      ->condition('n.class_name', $name);
  }
  elseif ($type == 'implementations') {
    // Use pattern matching to find functions that look like implementations of
    // this one. i.e. something_hookname, where "something" doesn't start with
    // an underscore. Note that LIKE uses _ as "match any one character", so
    // _ has to be escaped in this query. Limit this to Backdrop functions.
    if (strpos($name, 'hook_') === 0) {
      $hook_name = substr($name, 5);
      // If the hook has an UPPER_CASE_SECTION, allow that to match anything,
      // for cases like hook_update_N() and hook_form_FORM_ID_alter(). Note
      // that the upper-case section could be in the middle or end.
      $hook_name = preg_replace('/_[A-Z][A-Z_]*_/', '\_%\_', $hook_name);
      $hook_name = preg_replace('/_[A-Z][A-Z_]*$/', '\_%', $hook_name);
      $base_query = db_select('api_documentation', 'd')
        ->condition('d.object_name', '%\_' . $hook_name, 'LIKE')
        ->condition('d.object_name', '\_%', 'NOT LIKE')
        ->condition('d.object_name', 'hook\_%', 'NOT LIKE')
        ->condition('d.object_type', 'function')
        ->condition('d.branch_id', $branch_id)
        ->condition('d.did', $exclude_did, '<>')
        ->condition('d.is_backdrop', 1);
    }
  }
  elseif ($type == 'theme_invokes' || $type == 'theme_references') {
    // Use reference storage to find functions that call this theme, or
    // string references to this theme. Limit this to Backdrop functions on the
    // calls.
    $theme_name = '';
    if (strpos($name, 'theme_') === 0) {
      // It's presumably a theme function.
      $theme_name = substr($name, 6);
    }
    elseif (strpos($name, '.tpl.php') == strlen($name) - 8) {
      // It's presumably a theme template file.
      $name = basename($name);
      $theme_name = str_replace('-', '_', substr($name, 0, strlen($name) - 8));
    }
    elseif (preg_match('|^(.*)\.[^.]*\.twig$|', $name, $matches)) {
      // It's a *.*.twig file.
      $name = basename($name);
      $theme_name = str_replace('-', '_', basename($matches[1]));
    }

    if (strlen($theme_name) > 0) {
      // We could have calls to things like theme('name') or theme('name__sub')
      // (or the corresponding strings) recorded in the references. Match
      // either. Note _ escaping for LIKE queries.
      $base_query = db_select('api_reference_storage', 'r');
      $d = $base_query->innerJoin('api_documentation', 'd', 'r.from_did = d.did');
      $base_query->condition($d . '.branch_id', $branch_id)
        ->condition($d . '.did', $exclude_did, '<>')
        ->condition('d.is_backdrop', 1)
        ->condition(db_or()->condition('r.object_name', $theme_name)->condition('r.object_name', $theme_name . '\_\_%', 'LIKE'));
      if ($type == 'theme_invokes') {
        $base_query->condition('r.object_type', 'potential theme');
      }
      else {
        $base_query->condition('r.object_type', 'potential callback');
      }
    }
  }
  elseif ($type == 'invokes') {
    // Use reference storage to find functions that invoke this hook.
    // The reference storage holds the string that was actually found inside
    // the invoking function. So $name could be one of:
    // - "hook_$string",
    // - "hook_field_$string"
    // - "field_default_$string"
    // - "hook_user_$string"
    // - "hook_entity_$string"
    // - "hook_entity_bundle_$string"
    // - "hook_node_$string"
    // - "hook_$string_alter"
    // In all cases but the last, {reference_storage} has object_type
    // 'potential hook'; in the last case it is 'potential alter'.
    // So, we need to build a query that will find these matches between
    // $string (field object_name) and $name (passed into this function).
    // And only do this for Backdrop functions.
    $or_clauses = db_or();
    $found = FALSE;

    if (strpos($name, 'hook_') === 0) {
      $hook_name = substr($name, 5);
      $or_clauses->condition(db_and()->condition('r.object_type', 'potential hook')->condition('r.object_name', $hook_name));
      $found = TRUE;
      if (strpos($hook_name, 'user_') === 0) {
        $sub_hook_name = substr($hook_name, 5);
        $or_clauses->condition(db_and()->condition('r.object_type', 'potential hook')->condition('r.object_name', $sub_hook_name));
      }
      elseif (strpos($hook_name, 'entity_') === 0) {
        $sub_hook_name = substr($hook_name, 7);
        $or_clauses->condition(db_and()->condition('r.object_type', 'potential hook')->condition('r.object_name', $sub_hook_name));
        if (strpos($sub_hook_name, 'bundle_') === 0) {
          $sub_sub_hook_name = substr($sub_hook_name, 7);
          $or_clauses->condition(db_and()->condition('r.object_type', 'potential hook')->condition('r.object_name', $sub_sub_hook_name));
        }
      }
      elseif (strpos($hook_name, 'field_') === 0) {
        $sub_hook_name = substr($hook_name, 6);
        $or_clauses->condition(db_and()->condition('r.object_type', 'potential hook')->condition('r.object_name', $sub_hook_name));
      }
      elseif (strpos($hook_name, 'node_') === 0) {
        $sub_hook_name = substr($hook_name, 5);
        $or_clauses->condition(db_and()->condition('r.object_type', 'potential hook')->condition('r.object_name', $sub_hook_name));
      }
      elseif (strrpos($hook_name, '_alter') === strlen($hook_name) - 6) {
        $sub_hook_name = substr($hook_name, 0, strlen($hook_name) - 6);
        $or_clauses->condition(db_and()->condition('r.object_type', 'potential alter')->condition('r.object_name', $sub_hook_name));
      }
    }
    elseif (strpos($name, 'field_default_') === 0) {
      $hook_name = substr($name, 14);
      $or_clauses->condition(db_and()->condition('r.object_type', 'potential hook')->condition('r.object_name', $hook_name));
      $found = TRUE;
    }

    // If we found at least one match, run this query we've built.
    if ($found) {
      $base_query = db_select('api_reference_storage', 'r');
      $d = $base_query->innerJoin('api_documentation', 'd', 'r.from_did = d.did');
      $base_query->condition('d.branch_id', $branch_id)
        ->condition('d.did', $exclude_did, '<>')
        ->condition('d.is_backdrop', 1)
        ->condition($or_clauses);
    }
  }

  // See if we built a query to execute.
  if (is_null($base_query)) {
    return ($count) ? 0 : array();
  }

  // Execute the query.
  if ($count) {
    // We're looking for a count here.
    if ($group) {
      $base_query->addExpression('COUNT(DISTINCT d.did)', 'num');
    }
    else {
      $base_query->addExpression('COUNT(d.did)', 'num');
    }
    return $base_query->execute()->fetchField();
  }

  // If we get here, we want to return a list.
  if ($group) {
    $base_query->groupBy('d.did');
  }
  $base_query->fields('d', array('branch_id', 'object_name', 'title', 'summary', 'file_name', 'object_type', 'class_did', 'is_backdrop'))
    ->orderBy('d.title', 'ASC');
  if ($limit > 0) {
    $base_query->range(0, $limit);
  }

  $result = $base_query->execute();
  $list = array();
  foreach ($result as $object) {
    $list[] = array(
      'function' => l($object->title, api_url($object)),
      'file' => api_file_link($object),
      'description' => api_link_documentation($object->summary, $branch, NULL, $object->class_did, FALSE, FALSE, $object->is_backdrop),
    );
  }

  return $list;
}

/**
 * Builds and returns a references section for a documentation object.
 *
 * @param $types
 *   Types of references to make (see api_find_references() for the list).
 * @param $object
 *   Documentation object to find references for.
 * @param $branch
 *   Branch to find references in.
 *
 * @return
 *   Array of HTML strings for the references section. Each one is a
 *   collapsible list of a few references, with a link to the full references
 *   page. Empty sections are omitted.
 */
function api_build_references($types, $object, $branch) {
  $output = array();
  foreach ($types as $type) {
    $count = api_find_references($object, $branch, $type, TRUE, $object->did, 0, $object->is_backdrop);
    if ($count > 0) {
      $limit = config_get('api.settings', 'reference_limit');
      $limit = isset($limit) ? $limit : 5;
      $section = '';
      $section .= api_list_references($object, $branch, $type, $limit, $object->is_backdrop);
      if ($count > $limit) {
        $section .= '<p>' . theme('api_function_reference_link', array('type' => $type, 'count' => $count, 'function' => $object, 'override_text' => t('... See full list'))) . '</p>';
      }
      $title = api_reference_text($type, $count, $object);
      $output[] = api_collapsible($title, $section);
    }
  }

  return $output;
}

/**
 * Page callback: Displays a list of instances where a function is called.
 *
 * @param $function
 *   Documentation object representing the function to display.
 * @param $type
 *   Type of page to generate. See api_find_references for list.
 *
 * @return
 *   HTML output for the page.
 */
function api_page_function_calls($function, $type) {
  $branch = api_get_branch_by_id($function->branch_id);

  $call_count = api_find_references($function, $branch, $type, TRUE, $function->did, 0, $function->is_backdrop);
  $call_title = api_reference_text($type, $call_count, $function);
  api_object_title_and_breadcrumb($branch, $function, $call_title, TRUE);

  return api_list_references($function, $branch, $type, 0, $function->is_backdrop);
}

/**
 * Generates a list of references as HTML.
 *
 * @param $object
 *   Object to list references for.
 * @param $branch
 *   Branch the object is in.
 * @param $type
 *   Type of references: see api_find_references() for list.
 * @param $limit
 *   (optional) Limit list to a few references.
 * @param bool $is_backdrop
 *   (optional) If set to FALSE, and $type is a Backdrop-specific type (such as
 *   theme- or hook- related), just return an empty list or zero count.
 *
 * @return string
 *   HTML for a list of references.
 */
function api_list_references($object, $branch, $type, $limit = 0, $is_backdrop = FALSE) {
  $call_functions = api_find_references($object, $branch, $type, FALSE, $object->did, $limit, $is_backdrop);

  $note = '';
  if ($type == 'implementations') {
    $note = '<p>' . t('Note: this list is generated by pattern matching, so it may include some functions that are not actually implementations of this hook.') . '</p>';
  }
  elseif ($type == 'theme_references') {
    $note = '<p>' . t('Note: this list is generated by looking for the string for this theme hook, so it may include some references that are not actually using this theme hook.') . '</p>';
  }

  return $note . theme('api_functions', array('functions' => $call_functions));
}

/**
 * Page callback: Displays a full class hierarchy.
 *
 * @param $class
 *   Documentation object representing the class to display.
 *
 * @return
 *   HTML output for the page.
 */
function api_page_class_hierarchy($class) {
  $branch = api_get_branch_by_id($class->branch_id);

  $call_title = api_reference_text('hierarchy', 0, $class);
  api_object_title_and_breadcrumb($branch, $class, $call_title, TRUE);

  return api_class_hierarchy($class, 'full');
}

/**
 * Page callback: Displays documentation for an item (constant, global, etc.).
 *
 * This function can be used to display "simple" items (items without listings,
 * and functions are a special case as well, because they display signatures
 * and variations).
 *
 * @param $item
 *   Documentation object representing the item to display.
 * @param $type
 *   Type of item, one of: 'constant', 'global', 'property'.
 *
 * @return
 *   HTML output for the page.
 *
 * @see api_menu()
 */
function api_page_simple_item($item, $type) {
  $branch = api_get_branch_by_id($item->branch_id);

  api_object_title_and_breadcrumb($branch, $item);

  $documentation = api_link_documentation($item->documentation, $branch, $item->file_did, $item->class_did, FALSE, FALSE, $item->is_backdrop);
  $code = api_link_code($item->code, $branch, $item->file_did, $item->class_did, $item->is_backdrop);
  $related_topics = api_display_view('api_references', 'block_related_topics', array($item->did));
  $see = api_link_documentation($item->see, $branch, $item->file_did, $item->class_did, TRUE, TRUE, $item->is_backdrop);
  $deprecated = api_link_documentation($item->deprecated, $branch, $item->file_did, $item->class_did, TRUE, FALSE, $item->is_backdrop);
  $var = '';
  if ($type == 'property') {
    $var = api_link_name($item->var, $branch, '', '', $item->file_did, $item->class_did, NULL, FALSE, TRUE, NULL, NULL, 'class');
  }

  $class = api_class_section($item, $branch);

  $links = ($type == 'constant') ? api_build_references(array('constants'), $item, $branch) : array();

  $theme_hooks = array(
    'constant' => 'api_constant_page',
    'global' => 'api_global_page',
    'property' => 'api_property_page',
  );

  $output = theme($theme_hooks[$type], array('branch' => $branch, 'object' => $item, 'documentation' => $documentation, 'code' => $code, 'related_topics' => $related_topics, 'see' => $see, 'deprecated' => $deprecated, 'var' => $var, 'class' => $class, 'call_links' => $links));

  $output .= _api_add_comments($item);
  return $output;
}

/**
 * Page callback: Displays documentation for a class.
 *
 * @param $class
 *   Documentation object representing the class to display.
 *
 * @return
 *   HTML output for the page.
 *
 * @see api_menu()
 */
function api_page_class($class) {
  $branch = api_get_branch_by_id($class->branch_id);

  api_object_title_and_breadcrumb($branch, $class);

  $documentation = api_link_documentation($class->documentation, $branch, $class->file_did, $class->did, FALSE, FALSE, $class->is_backdrop);
  $related_topics = api_display_view('api_references', 'block_related_topics', array($class->did));
  $code = api_link_code($class->code, $branch, $class->file_did, $class->did, $class->is_backdrop);
  $see = api_link_documentation($class->see, $branch, $class->file_did, $class->did, TRUE, TRUE, $class->is_backdrop);
  $deprecated = api_link_documentation($class->deprecated, $branch, $class->file_did, $class->did, TRUE, FALSE, $class->is_backdrop);

  // Figure out the class hierarchy.
  $hierarchy = api_class_hierarchy($class);

  // Figure out what classes implement if this is an interface.
  $implements = array();
  if ($class->object_type === 'interface') {
    // Make a list of all interfaces that extend this one in this branch.
    $to_check = array($class->did);
    $extend_list = array($class->did);
    while (count($to_check) > 0) {
      $this_did = array_shift($to_check);
      $result = db_select('api_reference_storage', 'ars')
        ->condition('ars.object_type', 'class')
        ->condition('ars.extends_did', $this_did)
        ->condition('ars.branch_id', $branch->branch_id)
        ->fields('ars', array('from_did'))
        ->execute()
        ->fetchCol();
      foreach ($result as $new_did) {
        if (!in_array($new_did, $extend_list)) {
          // Avoid loops!
          $to_check[] = $new_did;
        }
        $extend_list[] = $new_did;
      }
    }

    // Now find which classes implement either the base interface or
    // any that extend it. Use DISTINCT on the query, because some results
    // could be for the same class, due to somewhere in its extends hierarchy,
    // implementing this interface twice.
    $query = db_select('api_reference_storage', 'ars');
    $ad = $query->innerJoin('api_documentation', 'ad', 'ad.did = ars.from_did');
    $query->fields($ad, array('did', 'branch_id', 'file_name', 'object_type', 'object_name'))
      ->condition('ars.extends_did', $extend_list)
      ->condition('ars.branch_id', $branch->branch_id)
      ->condition('ars.object_type', 'interface')
      ->orderBy($ad . '.object_name')
      ->distinct();
    $result = $query->execute();

    foreach ($result as $object) {
      $implements[] = l($object->object_name, api_url($object));
    }
    if (count($implements) > 0) {
      $implements = theme('item_list', array('items' => $implements));
    }
  }

  // Find and render all the class members.
  $objects = array();
  $out = api_display_view('api_members', 'block_member_list', array($class->did));
  // Only display if there was something there.
  if ($out) {
    $objects['members'] = array(
      '#type' => 'markup',
      '#markup' => '<h3>' . t('Members') . '</h3>' . $out,
    );
  }

  $objects = backdrop_render($objects);

  $reference_types = array('uses', 'references', 'services');
  if ($class->object_type == 'class') {
    $reference_types[] = 'annotations';
  }
  $links = api_build_references($reference_types, $class, $branch);

  // Put it all together.
  $output = theme('api_class_page', array('branch' => $branch, 'object' => $class, 'documentation' => $documentation, 'implements' => $implements, 'hierarchy' => $hierarchy, 'objects' => $objects, 'code' => $code, 'related_topics' => $related_topics, 'see' => $see, 'deprecated' => $deprecated, 'call_links' => $links));
  $output .= _api_add_comments($class);

  return $output;
}

/**
 * Page callback: Displays documentation for a service.
 *
 * @param $service
 *   Documentation object representing the service to display.
 *
 * @return
 *   HTML output for the page.
 *
 * @see api_menu()
 */
function api_page_service($service) {
  $branch = api_get_branch_by_id($service->branch_id);

  api_object_title_and_breadcrumb($branch, $service);

  // The class name (if there is one), or some other information is stored
  // in the documentation field.
  $class = api_link_documentation($service->documentation, $branch, $service->file_did, 0, TRUE, FALSE, $service->is_backdrop);
  $code = api_link_code($service->code, $branch, $service->file_did, $service->did, $service->is_backdrop);

  // Make a list of the service tags. These are in the api_references table.
  $results = db_select('api_reference_storage', 'r')
    ->condition('r.object_type', 'service_tag')
    ->condition('r.from_did', $service->did)
    ->fields('r', array('object_name'))
    ->execute()
    ->fetchCol();
  if (count($results)) {
    $tags = theme('item_list', array('items' => $results));
  }
  else {
    $tags = FALSE;
  }

  $links = api_build_references(array('use'), $service, $branch);

  // Put it all together.
  $output = theme('api_service_page', array('branch' => $branch, 'object' => $service, 'class' => $class, 'code' => $code, 'tags' => $tags, 'call_links' => $links));
  $output .= _api_add_comments($service);

  return $output;
}

/**
 * Page callback: Displays documentation for a group (topic).
 *
 * @param $group
 *   Documentation object representing the group (topic) to display.
 *
 * @return
 *   HTML output for the page.
 *
 * @see api_menu()
 */
function api_page_group($group) {
  $branch = api_get_branch_by_id($group->branch_id);

  api_object_title_and_breadcrumb($branch, $group);

  $documentation = api_link_documentation($group->documentation, $branch, $group->file_did, NULL, FALSE, FALSE, $group->is_backdrop);
  $see = api_link_documentation($group->see, $branch, $group->file_did, NULL, TRUE, TRUE, $group->is_backdrop);
  $related_topics = api_display_view('api_references', 'block_related_topics', array($group->did));

  // Find and render all items in this group.
  $types = array(
    'function' => t('Functions'),
    'constant' => t('Constants'),
    'global' => t('Globals'),
    'class' => t('Classes'),
    'interface' => t('Interfaces'),
    'trait' => t('Traits'),
    'file' => t('Files'),
    'group' => t('Sub-Topics'),
  );

  $objects = array();
  foreach ($types as $type => $label) {
    // Render the view of this type of object in this group.
    $out = api_display_view('api_references', 'block_items_in_group', array($branch->branch_id, $group->object_name, $type));
    // Only display if there was something there.
    if ($out) {
      $objects[$type] = array(
        '#type' => 'markup',
        '#markup' => '<h3>' . $label . '</h3>' . $out,
      );
    }
  }

  $objects = backdrop_render($objects);

  $output = theme('api_group_page', array('branch' => $branch, 'object' => $group, 'documentation' => $documentation, 'objects' => $objects, 'see' => $see, 'related_topics' => $related_topics));
  $output .= _api_add_comments($group);
  return $output;
}

/**
 * Sets the page title and breadcrumb for an object display page.
 *
 * @param $branch
 *   Object representing the branch.
 * @param $object
 *   Object representing the documentation item on the current page.
 * @param $title
 *   A string to be used as a replacement for the title (optional). Assumed to
 *   already be sanitized.
 * @param $object_name_in_breadcrumb
 *   A boolean indicating whether to add the object name to the breadcrumb.
 */
function api_object_title_and_breadcrumb($branch, $object, $title = NULL, $object_name_in_breadcrumb = FALSE) {
  // Allow the title to be overridden.
  if (empty($title)) {
    $title = check_plain($object->title);
    if ($object->object_type == 'group' || $object->object_type == 'file') {
      $h1title = $title;
    }
    else {
      $h1title = (empty($object->modifiers)) ? '' : check_plain($object->modifiers) . ' ';
      $h1title .= check_plain($object->object_type) . ' ' . $title;
    }
  }
  else {
    $h1title = $title;
  }
  backdrop_set_title($h1title, PASS_THROUGH);

  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('API reference'), 'api/' . $branch->project),
    l($branch->title, 'api/' . $branch->project . '/' . $branch->branch_name),
  );
  $page_title = array(
    check_plain(config_get('system.core', 'site_name')),
    check_plain($branch->title),
  );

  if ($object->object_type !== 'file') {
    $breadcrumb[] = l(basename($object->file_name), api_url($object, TRUE));
    $page_title[] = check_plain(basename($object->file_name));
  }
  if (!empty($object->class_did)) {
    $branch = api_get_branch_by_id($object->branch_id);
    $class = api_object_load((int) $object->class_did, $branch, array('interface', 'class', 'trait'), $object->file_name);
    if (isset($class)) {
      $breadcrumb[] = l($class->object_name, api_url($class));
    }
    // Note that this is not needed in the page title, since the object name
    // includes the class.
  }

  // Add the object name to the breadcrumb. This is used for function calls and
  // hook implementations listing pages.
  if ($object_name_in_breadcrumb) {
    $breadcrumb[] = l($object->object_name, api_url($object));
  }

  $page_title[] = $title;

  backdrop_set_breadcrumb($breadcrumb);
  api_set_html_page_title(implode(' | ', array_reverse($page_title)));
}

/**
 * Renders a class hierarchy, either full or partial.
 *
 * @param $class
 *   Class object with at least did, branch_id, file_name, object_type, and
 *   object_name properties.
 * @param $type
 *   One of the following strings to indicate what type of hierarchy:
 *   - 'full': Full hierarchy, showing all parents, siblings, children, etc.
 *   - 'ancestors': Only direct ancestors of this class.
 *
 * @return
 *   HTML string containing the class hierarchy, or an empty string if the
 *   only thing to display would be the class itself.
 */
function api_class_hierarchy($class, $type = 'ancestors') {
  // Avoid inheritance loops.
  $processed = array();

  $branch = api_get_branch_by_id($class->branch_id);

  // See if this class has any children. Note that this is only in the same
  // branch.
  $child_output = '';
  if ($type == 'full') {
    $children = api_class_children($class, $class->did);
    if (count($children)) {
      $child_output = theme('item_list', array('items' => $children));
    }
  }

  // Find the direct-line extends ancestors of this class, only in the same
  // branch.
  $parent = $class;
  $current = $class;

  while ($parent) {
    $processed[] = $parent->did;

    // Query to find the next ancestor.
    $query = db_select('api_reference_storage', 'ars');
    $ad = $query->innerJoin('api_documentation', 'ad', 'ad.did = ars.extends_did');
    $query
      ->fields($ad, array('did', 'branch_id', 'file_name', 'object_type', 'object_name', 'namespaced_name'))
      ->condition('ars.from_did', $parent->did)
      ->condition('ars.object_type', 'class')
      ->condition('ad.object_type', array('class', 'interface'));
    $results = $query->execute()->fetchAll();
    $count = count($results);
    if ($count < 1) {
      $parent = api_class_hierarchy_line($current, $class->did);
      break;
    }
    $parent = array_pop($results);
    if ($count == 1 && $type == 'full') {
      $siblings = api_class_children($parent, $class->did);
    }
    else {
      $siblings = array($current->did => api_class_hierarchy_line($current, $class->did));
    }

    $siblings[$current->did] .= $child_output;
    $child_output = theme('item_list', array('items' => $siblings));
    $current = $parent;

    if ($count > 1) {
      // If we found more than one result, that means that either there were
      // multiple results with the same class name, or this was an interface
      // with multiple inheritance (hopefully not both!). So, if the results
      // have the same name, add a line indicating a search. If they have
      // different names, list them all. In either case, don't attempt any more
      // lines of hierarchy.
      $same = TRUE;
      foreach ($results as $item) {
        if ($item->object_name != $parent->object_name) {
          $same = FALSE;
          break;
        }
      }

      if ($same) {
        $parent = l(t('Multiple classes named @classname', array('@classname' => $parent->object_name)), 'api/' . $branch->project . '/' . $branch->branch_name . '/search/' . check_plain($parent->object_name));
      }
      else {
        $this_line = array();
        array_push($results, $parent);
        foreach ($results as $item) {
          $this_line[] = api_class_hierarchy_line($item, $class->did);
        }
        $parent = implode('; ', $this_line);
      }
      break;
    }

    // If we get here, make sure we're not in an infinite loop.
    if (in_array($parent->did, $processed)) {
      $parent = api_class_hierarchy_line($current, $class->did);
      break;
    }
  }

  $output = theme('item_list', array('items' => array($parent . $child_output)));

  if ($type != 'full' && $class->object_type != 'trait') {
    $output .= '<p>' . theme('api_function_reference_link', array('type' => 'hierarchy', 'count' => 0, 'function' => $class)) . '</p>';
  }

  return $output;
}

/**
 * Finds and renders the children of a class within the same branch.
 *
 * @param $class
 *   Documentation object for the class.
 * @param $current_did
 *   Documentation ID of the class on the current page.
 *
 * @return
 *   Array of children. Keys are class names, and values are output
 *   of api_class_hierarchy_line().
 */
function api_class_children($class, $current_did = 0) {
  $children = array();
  $query = db_select('api_reference_storage', 'ars');
  $ad = $query->innerJoin('api_documentation', 'ad', 'ad.did = ars.from_did');
  $query
    ->fields($ad, array('did', 'branch_id', 'file_name', 'object_type', 'object_name'))
    ->condition('ars.extends_did', $class->did)
    ->condition('ad.object_type', array('class', 'interface'))
    ->condition('ars.object_type', 'class')
    ->orderBy('ad.object_name');
  $result = $query->execute();
  foreach ($result as $object) {
    $children[$object->did] = api_class_hierarchy_line($object, $current_did);
  }

  return $children;
}

/**
 * Renders the class hierarchy line for a single class.
 *
 * @param $class
 *   Documentation object for the class to render.
 * @param $current_did
 *   Documentation ID of the class that the current page is showing. If this
 *   class is being shown, it will have an 'active' class on the link.
 *
 * @return
 *   HTML for this class in the class hierarchy.
 */
function api_class_hierarchy_line($class, $current_did = 0) {
  $branch = api_get_branch_by_id($class->branch_id);
  $classes = array();
  if ($class->did == $current_did) {
    $classes[] = 'active';
  }

  // See if this class implements any interfaces or uses traits.
  $interfaces = array();
  $traits = array();

  $query = db_select('api_reference_storage', 'ars');
  $ref_name = $query->addField('ars', 'object_name', 'ref_name');
  $ad = $query->leftJoin('api_documentation', 'ad', 'ad.did = ars.extends_did');
  $obj_name = $query->addField($ad, 'object_name', 'object_name');
  $query
    ->fields($ad, array('did', 'branch_id', 'file_name', 'object_type'))
    ->condition('ars.from_did', $class->did)
    ->condition('ars.object_type', array('interface', 'trait'))
    ->orderBy('ad.object_name');
  $query->addField('ars', 'object_type', 'ars_object_type');
  $result = $query->execute();
  foreach ($result as $object) {
    if (isset($object->did) && $object->did) {
      // Interface/trait exists as an object in the database, make a link.
      if ($object->ars_object_type == 'interface') {
        if (!isset($interfaces[$object->object_name])) {
          $interfaces[$object->object_name] = l($object->object_name, api_url($object));
        }
      }
      else {
        if (!isset($traits[$object->object_name])) {
          $traits[$object->object_name] = l($object->object_name, api_url($object));
        }
      }
    }
    else {
      // This class was declared to implement this interface, or use this
      // trait, but it didn't get defined (probably a built-in PHP object
      // or from another project). So just display the name.
      if ($object->ars_object_type == 'interface') {
        $interfaces[] = $object->ref_name;
      }
      else {
        $traits[] = $object->ref_name;
      }
    }
  }

  // See if this class extends another class that isn't defined in our DB.
  $extends = '';
  $query = db_select('api_reference_storage', 'ars')
    ->fields('ars', array('object_name'))
    ->condition('ars.from_did', $class->did)
    ->condition('ars.object_type', 'class')
    ->condition('ars.extends_did', 0)
    ->range(0, 1);
  $parent = $query->execute()->fetchObject();
  if (isset($parent) && is_object($parent)) {
    $extends = ' extends ' . $parent->object_name;
  }

  $output = $class->object_type . ' ' . l($class->object_name, api_url($class), array('attributes' => array('class' => $classes))) . $extends;
  if (count($interfaces) > 0) {
    $output .= ' implements ' . implode(', ', $interfaces);
  }
  if (count($traits) > 0) {
    $output .= ' uses ' . implode(', ', $traits);
  }

  return $output;
}

/**
 * Renders comments for a documentation object.
 *
 * @param $documentation_object
 *   Object to render comments for.
 *
 * @return
 *   Rendered comments to display with the object.
 */
function _api_add_comments($documentation_object) {
  $output = '';
  $node_type = node_type_get_type('api');
  if (module_exists('comment') && user_access('access comments') && $node_type->settings['comment_default'] != COMMENT_NODE_HIDDEN) {
    // Let the Comment module output the comments and comment form.
    $additions = comment_node_page_additions(node_load($documentation_object->did));
    // If the comment form wasn't included, see if authenticated users can post
    // comments, and add a link saying to log in to post.
    if (!isset($additions['comment_form'])) {
      $roles = user_roles(TRUE, 'post comments');
      if (array_key_exists(BACKDROP_AUTHENTICATED_ROLE, $roles)) {
        $options = array(
          'query' => backdrop_get_destination(),
          'fragment' => 'comment-form',
        );
        if (config_get('system.core', 'user_register') !== USER_REGISTER_ADMINISTRATORS_ONLY) {
          // Users can register themselves.
          $additions['login_link'] = array(
            '#type' => 'markup',
            '#value' => t('<a href="@login">Log in</a> or <a href="@register">register</a> to post comments', array('@login' => url('user/login', $options), '@register' => url('user/register', $options))),
          );
        }
        else {
          // Only admins can add new users, no public registration.
          $additions['login_link'] = array(
            '#type' => 'markup',
            '#value' => t('<a href="@login">Log in</a> to post comments', array('@login' => url('user/login', $options))),
          );
        }
      }
    }
    $output .= backdrop_render($additions);
  }

  return $output;
}

/**
 * Returns a link to the file a documentation object is in.
 *
 * @param $object
 *   Documentation object.
 *
 * @return
 *   Formatted link to the file the object is in.
 */
function api_file_link($object) {
  return str_replace('/', '/<wbr>', dirname($object->file_name)) . '/<wbr>' . l(basename($object->file_name), api_url($object, TRUE));
}

/**
 * Returns the page title or link text for a references page/link.
 *
 * @param $type
 *   The type of reference link. This is either one of the values listed in
 *   api_find_references(), or:
 *   - 'hierarchy': Class hierarchy ($count is ignored).
 * @param $count
 *   The number of referenced items.
 * @param $function
 *   The function, class, or file object being referenced.
 *
 * @return
 *   Text to be used as the link to the reference listing page, or the title
 *   of the page.
 */
function api_reference_text($type, $count, $function) {
  $name_to_use = check_plain($function->title);
  if ($type == 'references') {
    $name_to_use = "'" . $name_to_use . "'";
  }
  elseif ($type == 'yml_config') {
    $name_to_use = "'" . substr($name_to_use, 0, strlen($name_to_use) - 4) . "'";
    $type = 'references';
  }
  elseif ($function->object_type == 'function') {
    $name_to_use .= '()';
  }

  if ($type == 'calls') {
    return format_plural($count, '1 call to !name', '@count calls to !name', array('!name' => $name_to_use));
  }
  if ($type == 'constants') {
    return format_plural($count, '1 use of !name', '@count uses of !name', array('!name' => $name_to_use));
  }
  if ($type == 'implementations') {
    return format_plural($count, '1 function implements !name', '@count functions implement !name', array('!name' => $name_to_use));
  }
  if ($type == 'uses') {
    return format_plural($count, '1 file declares its use of !name', '@count files declare their use of !name', array('!name' => $name_to_use));
  }
  if ($type == 'overrides') {
    return format_plural($count, '1 method overrides !name', '@count methods override !name', array('!name' => $name_to_use));
  }
  if ($type == 'theme_invokes') {
    return format_plural($count, '1 theme call to !name', '@count theme calls to !name', array('!name' => $name_to_use));
  }
  if ($type == 'theme_references') {
    return format_plural($count, '1 string reference to the theme hook from !name', '@count string references to the theme hook from !name', array('!name' => $name_to_use));
  }
  if ($type == 'invokes') {
    return format_plural($count, '1 invocation of !name', '@count invocations of !name', array('!name' => $name_to_use));
  }
  if ($type == 'references' || $type == 'use') {
    return format_plural($count, '1 string reference to !name', '@count string references to !name', array('!name' => $name_to_use));
  }
  if ($type == 'annotations') {
    return format_plural($count, '1 class is annotated with !name', '@count classes are annotated with !name', array('!name' => $name_to_use));
  }
  if ($type == 'services') {
    return format_plural($count, '1 service uses !name', '@count services use !name', array('!name' => $name_to_use));
  }
  if ($type == 'yml_keys') {
    return format_plural($count, '1 string reference to YAML keys in !name', '@count string references to YAML keys in !name', array('!name' => $name_to_use));
  }
  if ($type == 'hierarchy') {
    return t('Expanded class hierarchy of !name', array('!name' => $name_to_use));
  }

  return '';
}

/**
 * Creates a section documenting which class a member is from.
 *
 * @param $item
 *   Documentation item for the member.
 * @param $branch
 *   Branch the item is in.
 *
 * @return
 *   HTML for the section, or FALSE if it is not a class member.
 */
function api_class_section($item, $branch) {
  if (!isset($item->class_did) || !$item->class_did) {
    return FALSE;
  }

  $class = api_object_load((int) $item->class_did, $branch, array('class', 'interface', 'trait'));

  if (is_null($class)) {
    return FALSE;
  }

  return theme('api_class_section', array('class' => $class, 'branch' => $branch));
}
