<?php

/**
 * @file
 * Feeds plugins declarations.
 *
 */

/**
 * @see feeds_tamper_feeds_tamper_plugins().
 *
 * The format of the plugin info:
 *
 *   $info['MACHINE_NAME'] = array(
 *   'form' => 'feeds_tamper_MACHINE_NAME_form',
 *   'callback' => 'feeds_tamper_MACHINE_NAME_callback',
 *   'name' => 'NAME',
 *   'multi' => 'loop', // loop or direct
 *   'single' => 'skip', // optional
 *   'category' => 'CATEGORY',
 *   'default description' => 'Description here', // optional
 *   'handler' => array(
 *     'file_type' => 'inc',
 *     'file_name' => 'MACHINE_NAME',
 *     'path' => 'plugins/', // usually plugins/ directory
 *     'module' => MODULE_NAME,
 *   ),
 * );
 */
function _feeds_tamper_feeds_tamper_plugins() {
  $info = array();
  $info['absolute_url'] = array(
    'form' => 'feeds_tamper_absolute_url_form',
    'callback' => 'feeds_tamper_absolute_url_callback',
    'name' => 'Make URLs absolute',
    'category' => 'HTML',
  );
  $info['array_filter'] = array(
    'form' => 'feeds_tamper_array_filter_form',
    'callback' => 'feeds_tamper_array_filter_callback',
    'name' => 'Filter empty items',
    'multi' => 'direct',
    'category' => 'List',
  );
  $info['cast_to_int'] = array(
    'form' => 'feeds_tamper_cast_to_int_form',
    'callback' => 'feeds_tamper_cast_to_int_callback',
    'name' => 'Cast to integer',
    'multi' => 'loop',
    'category' => 'Number',
  );
  $info['convert_boolean'] = array(
    'form' => 'feeds_tamper_convert_boolean_form',
    'callback' => 'feeds_tamper_convert_boolean_callback',
    'validate' => 'feeds_tamper_convert_boolean_validate',
    'name' => 'Convert to boolean',
    'multi' => 'loop',
    'category' => 'Text',
  );
  $info['convert_case'] = array(
    'form' => 'feeds_tamper_convert_case_form',
    'callback' => 'feeds_tamper_convert_case_callback',
    'name' => 'Convert case',
    'multi' => 'loop',
    'category' => 'Text',
  );
  $info['copy'] = array(
    'form' => 'feeds_tamper_copy_form',
    'callback' => 'feeds_tamper_copy_callback',
    'name' => 'Copy source value',
    'category' => 'Other',
    'multi' => 'direct',
  );
  $info['country_to_code'] = array(
    'form' => 'feeds_tamper_country_to_code_form',
    'callback' => 'feeds_tamper_country_to_code_callback',
    'name' => 'Country to ISO code',
    'multi' => 'loop',
    'category' => 'Other',
    'default description' => 'Convert country to ISO code',
  );
  $info['default_value'] = array(
    'form' => 'feeds_tamper_default_value_form',
    'callback' => 'feeds_tamper_default_value_callback',
    'name' => 'Set value or default value',
    'multi' => 'direct',
    'category' => 'Other',
  );
  $info['efq_finder'] = array(
    'form' => 'feeds_tamper_efq_finder_form',
    'callback' => 'feeds_tamper_efq_finder_callback',
    'validate' => 'feeds_tamper_efq_finder_validate',
    'name' => 'Entity Field Query finder',
    'multi' => 'loop',
    'category' => 'Other',
  );
  $info['encode'] = array(
    'form' => 'feeds_tamper_encode_form',
    'callback' => 'feeds_tamper_encode_callback',
    'name' => 'Encode/Decode Plugin',
    'multi' => 'direct',
    'category' => 'Text',
  );
  $info['explode'] = array(
    'form' => 'feeds_tamper_explode_form',
    'callback' => 'feeds_tamper_explode_callback',
    'validate' => 'feeds_tamper_explode_validate',
    'name' => 'Explode',
    'multi' => 'direct',
    'category' => 'List',
  );
  $info['find_replace_regex'] = array(
    'form' => 'feeds_tamper_find_replace_regex_form',
    'callback' => 'feeds_tamper_find_replace_regex_callback',
    'validate' => 'feeds_tamper_find_replace_regex_validate',
    'name' => 'Find replace REGEX',
    'multi' => 'direct',
    'category' => 'Text',
  );
  $info['find_replace'] = array(
    'form' => 'feeds_tamper_find_replace_form',
    'callback' => 'feeds_tamper_find_replace_callback',
    'validate' => 'feeds_tamper_find_replace_validate',
    'name' => 'Find replace',
    'multi' => 'loop',
    'category' => 'Text',
  );
  $info['hash'] = array(
    'form' => 'feeds_tamper_hash_form',
    'callback' => 'feeds_tamper_hash_callback',
    'name' => 'Calculate hash',
    'multi' => 'direct',
    'category' => 'Other',
  );
  $info['html_entity_decode'] = array(
    'form' => 'feeds_tamper_html_entity_decode_form',
    'callback' => 'feeds_tamper_html_entity_decode_callback',
    'name' => 'HTML entity decode',
    'multi' => 'loop',
    'category' => 'HTML',
  );
  $info['html_entity_encode'] = array(
    'form' => 'feeds_tamper_html_entity_encode_form',
    'callback' => 'feeds_tamper_html_entity_encode_callback',
    'name' => 'HTML entity encode',
    'multi' => 'loop',
    'category' => 'HTML',
  );
  $info['implode'] = array(
    'form' => 'feeds_tamper_implode_form',
    'callback' => 'feeds_tamper_implode_callback',
    'validate' => 'feeds_tamper_implode_validate',
    'name' => 'Implode',
    'multi' => 'direct',
    'single' => 'skip',
    'category' => 'List',
  );
  $info['keyword_filter'] = array(
    'form' => 'feeds_tamper_keyword_filter_form',
    'callback' => 'feeds_tamper_keyword_filter_callback',
    'validate' => 'feeds_tamper_keyword_filter_validate',
    'name' => 'Keyword filter',
    'multi' => 'direct',
    'category' => 'Filter',
  );
  $info['math'] = array(
    'form' => 'feeds_tamper_math_form',
    'validate' => 'feeds_tamper_math_validate',
    'callback' => 'feeds_tamper_math_callback',
    'name' => 'Mathematical operation',
    'category' => 'Number',
    'multi' => 'loop',
  );
  $info['number_format'] = array(
    'form' => 'feeds_tamper_number_format_form',
    'callback' => 'feeds_tamper_number_format_callback',
    'validate' => 'feeds_tamper_number_format_validate',
    'name' => 'Format a number',
    'multi' => 'loop',
    'category' => 'Number',
  );
  $info['required'] = array(
    'form' => 'feeds_tamper_required_form',
    'callback' => 'feeds_tamper_required_callback',
    'name' => 'Required field',
    'multi' => 'direct',
    'category' => 'Filter',
  );
  $info['rewrite'] = array(
    'form' => 'feeds_tamper_rewrite_form',
    'callback' => 'feeds_tamper_rewrite_callback',
    'name' => 'Rewrite',
    'multi' => 'skip',
    'category' => 'Other',
  );
  $info['sprintf'] = array(
    'form' => 'feeds_tamper_sprintf_form',
    'callback' => 'feeds_tamper_sprintf_callback',
    'name' => 'Format string',
    'multi' => 'loop',
    'category' => 'Text',
  );
  $info['state_to_abbrev'] = array(
    'form' => 'feeds_tamper_state_to_abbrev_form',
    'callback' => 'feeds_tamper_state_to_abbrev_callback',
    'name' => 'Full U.S. state name to abbrev.',
    'multi' => 'loop',
    'category' => 'Other',
    'default description' => 'Full state name to abbrev',
  );
  $info['str_pad'] = array(
    'form' => 'feeds_tamper_str_pad_form',
    'callback' => 'feeds_tamper_str_pad_callback',
    'validate' => 'feeds_tamper_str_pad_validate',
    'name' => 'Pad a string',
    'multi' => 'loop',
    'category' => 'Text',
  );
  $info['strip_tags'] = array(
    'form' => 'feeds_tamper_strip_tags_form',
    'callback' => 'feeds_tamper_strip_tags_callback',
    'validate' => 'feeds_tamper_strip_tags_validate',
    'name' => 'Strip tags',
    'multi' => 'loop',
    'category' => 'HTML',
  );
  $info['strtotime'] = array(
    'form' => 'feeds_tamper_strtotime_form',
    'callback' => 'feeds_tamper_strtotime_callback',
    'name' => 'String to Unix timestamp',
    'multi' => 'loop',
    'category' => 'Date/time',
  );
  $info['timetodate'] = array(
    'form' => 'feeds_tamper_timetodate_form',
    'callback' => 'feeds_tamper_timetodate_callback',
    'validate' => 'feeds_tamper_timetodate_validate',
    'name' => 'Unix timestamp to Date',
    'multi' => 'loop',
    'category' => 'Date/time',
  );
  $info['trim'] = array(
    'form' => 'feeds_tamper_trim_form',
    'callback' => 'feeds_tamper_trim_callback',
    'name' => 'Trim',
    'multi' => 'loop',
    'category' => 'Text',
  );
  $info['truncate_text'] = array(
    'form' => 'feeds_tamper_truncate_text_form',
    'callback' => 'feeds_tamper_truncate_text_callback',
    'validate' => 'feeds_tamper_truncate_text_validate',
    'name' => 'Truncate',
    'multi' => 'loop',
    'category' => 'Text',
  );
  $info['unique'] = array(
    'form' => 'feeds_tamper_unique_form',
    'callback' => 'feeds_tamper_unique_callback',
    'name' => 'Unique',
    'multi' => 'direct',
    'single' => 'skip',
    'category' => 'List',
  );
  $info['urldecode'] = array(
    'form' => 'feeds_tamper_urldecode_form',
    'callback' => 'feeds_tamper_urldecode_callback',
    'validate' => 'feeds_tamper_urldecode_validate',
    'multi' => 'loop',
    'name' => 'URL Decode',
    'category' => 'Text',
  );
  $info['urlencode'] = array(
    'form' => 'feeds_tamper_urlencode_form',
    'callback' => 'feeds_tamper_urlencode_callback',
    'validate' => 'feeds_tamper_urlencode_validate',
    'multi' => 'loop',
    'name' => 'URL Encode',
    'category' => 'Text',
  );

  foreach($info as $k => $v) {
    $info[$k]['handler'] = array(
      'file_type' => 'inc',
      'file_name' => $k,
      'path' => 'plugins/',
      'module' => 'feeds_tamper',
    );
  }

  return $info;
}
