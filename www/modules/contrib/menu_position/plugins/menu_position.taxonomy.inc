<?php
/**
 * @file
 * Provides the "Taxonomy" rule plugin for the Menu Position module.
 */

/**
 * Checks if a specific taxonomy term is set in the node.
 *
 * @param array $variables
 *   An array containing each of the variables saved in the database necessary
 *   to evaluate this condition of the rule.
 *
 * @return bool
 *   TRUE if condition applies successfully. Otherwise FALSE.
 */
function menu_position_menu_position_condition_taxonomy(array $variables) {
  // We can only check for taxonomy terms on an "entity" page.
  if ($variables['context']['entity_type']) {

    // Fallback for legacy configuration.
    if (empty($variables['match_types'])) {
      $variables['match_types'] = array('terms' => '', 'content' => 'content');
    }
    $match_types = $variables['match_types'];

    // Grab the variables stored statically in the rule.
    $vocabulary = $variables['vocabulary'];
    $tid = $variables['tid'];

    // Determine what kind of entity page this is.
    $entity_type = $variables['context']['entity_type'];
    $bundle_name = $variables['context']['bundle_name'];
    $entity = $variables['context'][$entity_type];

    if ($match_types['content']) {
      // Build a list of each taxonomy reference fields.
      $taxonomy_fields = &backdrop_static(__FUNCTION__, NULL);
      if (!isset($taxonomy_fields)) {
        $taxonomy_fields = array();
        $field_info = field_info_fields();
        foreach (array_keys(field_info_instances($entity_type, $bundle_name)) as $key) {
          if (!empty($field_info[$key]['type']) && $field_info[$key]['type'] == 'taxonomy_term_reference') {
            $taxonomy_fields[$key] = $field_info[$key]['translatable'];
          }
        }
      }

      $tids = array();
      foreach ($taxonomy_fields as $field => $translatable) {
        $langcode = $translatable ? $entity->langcode : LANGUAGE_NONE;
        if (!empty($entity->{$field}[$langcode])) {
          // Check for matching terms.
          if (!empty($tid)) {
            foreach ($entity->{$field}[$langcode] as $term) {
              if (in_array($term['tid'], $tid)) {
                return TRUE;
              }
            }
          }
          // Check for any terms from a matching vocabulary.
          else {
            foreach ($entity->{$field}[$langcode] as $term) {
              $tids[] = $term['tid'];
            }
          }
        }
      }
      if (count($tids)) {
        $count = db_query("SELECT count(tid) FROM {taxonomy_term_data} WHERE tid IN (:tids) AND vocabulary = :vocabulary", array(
          ':tids' => $tids,
          ':vocabulary' => $vocabulary,
        ))->fetchField();
        if ($count > 0) {
          return TRUE;
        }
      }
    }

    // Taxonomy terms.
    if ($entity_type == 'taxonomy_term' && $match_types['terms']) {
      // Check for matching terms.
      if (!empty($tid)) {
        if (isset($entity->tid) && in_array($entity->tid, $tid)) {
          return TRUE;
        }
      }
      // Check for any terms from a matching vocabulary.
      else {
        if (isset($entity->vocabulary) && $entity->vocabulary === $vocabulary) {
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

/**
 * Adds form elements for the "taxonomy" plugin to the rule configuration form.
 *
 * @param array $form
 *   A reference to the "add/edit rule" form array. New form elements should be
 *   added directly to this array.
 * @param array $form_state
 *   A reference to the current form state.
 */
function menu_position_menu_position_rule_taxonomy_form(array &$form, array &$form_state) {
  // If this is an existing rule, load the variables stored in the rule for this
  // plugin.
  $variables = !empty($form_state['#menu-position-rule']['conditions']['taxonomy']) ? $form_state['#menu-position-rule']['conditions']['taxonomy'] : array();

  // Fallback for legacy configuration.
  if (empty($variables['match_types'])) {
    $variables['match_types'] = array('terms' => '', 'content' => 'content');
  }

  // Load the taxonomy terms.
  if (!empty($variables['tid'])) {
    $terms = array();
    foreach (taxonomy_term_load_multiple($variables['tid']) as $term) {
      $terms[] = $term->name;
    }
    $terms = implode(', ', $terms);
  }
  else {
    $terms = '';
  }

  $form['conditions']['taxonomy'] = array(
    '#type' => 'fieldset',
    '#title' => t('Taxonomy'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#attached' => array(
      'js' => array(backdrop_get_path('module', 'menu_position') . '/plugins/menu_position.taxonomy.js'),
    ),
  );
  $form['conditions']['taxonomy']['description'] = array(
    '#type' => 'item',
    '#description' => t('Apply this rule only on pages that display content having the given vocabulary or taxonomy term.'),
  );
  $form['conditions']['taxonomy']['match_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Type of match'),
    '#default_value' => !empty($variables['match_types']) ? $variables['match_types'] : array('terms' => '', 'content' => 'content'),
    '#options' => array(
      'terms' => t('Match when viewing term pages'),
      'content' => t('Match when viewing content tagged with the term'),
    ),
  );
  $vocabularies = taxonomy_vocabulary_load_multiple(FALSE);
  $vocabulary_options = array();
  foreach ($vocabularies as $vocabulary) {
    $vocabulary_options[$vocabulary->machine_name] = $vocabulary->name;
  }
  $form['conditions']['taxonomy']['vocabulary'] = array(
    '#type' => 'select',
    '#title' => t('Vocabulary'),
    '#default_value' => !empty($variables['vocabulary']) ? $variables['vocabulary'] : '',
    '#options' => array('0' => t('- None -')) + $vocabulary_options,
  );
  $form['conditions']['taxonomy']['vocabulary_or_tid'] = array(
    '#type' => 'radios',
    '#title' => t('In this vocabulary, match:'),
    '#default_value' => !empty($variables['tid']) ? 'tid' : 'vocabulary',
    '#options' => array(
      'vocabulary' => t('any taxonomy term'),
      'tid' => t('only a selected taxonomy term'),
    ),
    '#states' => array(
      'invisible' => array(
        ':input[name=vocabulary]' => array('value' => '0'),
      ),
    ),
  );
  $form['#attached']['js'][] = array(
    'data' => array('menu_position_taxonomy_url' => url('menu-position/taxonomy/autocomplete')),
    'type' => 'setting',
  );
  $form['conditions']['taxonomy']['term'] = array(
    '#type' => 'textfield',
    '#maxlength' => 1024,
    '#title' => t('Taxonomy term'),
    '#default_value' => $terms,
    '#autocomplete_path' => 'menu-position/taxonomy/autocomplete/' . (isset($form_state['input']['vocabulary']) ? $form_state['input']['vocabulary'] : $form['conditions']['taxonomy']['vocabulary']['#default_value']),
    '#element_validate' => array('menu_position_taxonomy_autocomplete_validate'),
    '#description' => t('Match at least one of these taxonomy terms.'),
    '#states' => array(
      'visible' => array(
        ':input[name=vocabulary_or_tid]' => array('value' => 'tid'),
      ),
    ),
  );

  // Add a submit handler.
  $form['#submit'][] = 'menu_position_menu_position_rule_taxonomy_form_submit';
}

/**
 * Prepares the "taxonomy" variables to be stored in the rule.
 *
 * @param array $form
 *   A reference to the "add/edit rule" form array.
 * @param array $form_state
 *   A reference to the current form state, including submitted values.
 */
function menu_position_menu_position_rule_taxonomy_form_submit(array &$form, array &$form_state) {
  if (!empty($form_state['values']['term']) || !empty($form_state['values']['vocabulary'])) {
    $variables = array(
      'vocabulary' => $form_state['values']['vocabulary'],
      'tid' => array(),
      'match_types' => $form_state['values']['match_types'],
    );
    // Determine if a taxonomy term has been selected.
    if ($form_state['values']['vocabulary_or_tid'] == 'tid' && !empty($form_state['values']['term'])) {
      foreach ($form_state['values']['term'] as $term) {
        $variables['tid'][] = $term['tid'];
      }
    }
    $form_state['values']['conditions']['taxonomy'] = $variables;
  }
}

/**
 * Helper function for auto-completion.
 */
function menu_position_taxonomy_autocomplete($vocabulary, $tags_typed = '') {
  // Don't query for terms if no vocabulary is selected.
  if (empty($vocabulary)) {
    backdrop_json_output(array());
    return;
  }

  // The user enters a comma-separated list of tags. We only auto-complete the
  // last tag.
  $tags_typed = backdrop_explode_tags($tags_typed);
  $tag_last = backdrop_strtolower(array_pop($tags_typed));

  $term_matches = array();
  if ($tag_last != '') {

    $query = db_select('taxonomy_term_data', 't');
    $query->addTag('translatable');
    $query->addTag('term_access');

    // Do not select already entered terms.
    if (!empty($tags_typed)) {
      $query->condition('t.name', $tags_typed, 'NOT IN');
    }
    // Select rows that match by term name.
    $tags_return = $query
      ->fields('t', array('tid', 'name'))
      ->condition('t.vocabulary', array($vocabulary))
      ->condition('t.name', '%' . db_like($tag_last) . '%', 'LIKE')
      ->range(0, 10)
      ->execute()
      ->fetchAllKeyed();

    $prefix = count($tags_typed) ? implode(', ', $tags_typed) . ', ' : '';

    foreach ($tags_return as $tid => $name) {
      $n = $name;
      // Term names containing commas or quotes must be wrapped in quotes.
      if (strpos($name, ',') !== FALSE || strpos($name, '"') !== FALSE) {
        $n = '"' . str_replace('"', '""', $name) . '"';
      }
      $term_matches[$prefix . $n] = check_plain($name);
    }
  }

  backdrop_json_output($term_matches);
}

/**
 * Form element validate handler for taxonomy term autocomplete element.
 */
function menu_position_taxonomy_autocomplete_validate($element, &$form_state) {
  // Autocomplete widgets do not send their tids in the form, so we must detect
  // them here and process them independently.
  $value = array();

  if ($tags = $element['#value']) {
    // Translate term names into actual terms.
    $typed_terms = backdrop_explode_tags($tags);
    foreach ($typed_terms as $typed_term) {
      // See if the term exists in the chosen vocabulary and return the tid;
      // otherwise, create a new 'autocreate' term for insert/update.
      if ($possibilities = taxonomy_term_load_multiple(array(), array('name' => trim($typed_term), 'vocabulary' => array($form_state['values']['vocabulary'])))) {
        $term = array_pop($possibilities);
        $value[] = (array) $term;
      }
      else {
        form_set_error('term', t('%term is not a valid taxonomy term.', array(
          '%term' => $typed_term,
        )));
      }
    }
  }

  if (!empty($value)) {
    form_set_value($element, $value, $form_state);
  }
}
