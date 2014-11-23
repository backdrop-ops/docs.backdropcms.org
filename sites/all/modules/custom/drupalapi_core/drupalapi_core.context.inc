<?php
/**
 * @file
 * drupalapi_core.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function drupalapi_core_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'layout-global-all';
  $context->description = '';
  $context->tag = 'DrupalAPI';
  $context->conditions = array(
    'sitewide' => array(
      'values' => array(
        1 => 1,
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'api-api-search' => array(
          'module' => 'api',
          'delta' => 'api-search',
          'region' => 'sidebar_second',
          'weight' => '-10',
        ),
        'api-navigation' => array(
          'module' => 'api',
          'delta' => 'navigation',
          'region' => 'sidebar_second',
          'weight' => '-9',
        ),
        'drupalapi_core-realityloop' => array(
          'module' => 'drupalapi_core',
          'delta' => 'realityloop',
          'region' => 'footer',
          'weight' => '-10',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('DrupalAPI');
  $export['layout-global-all'] = $context;

  return $export;
}
