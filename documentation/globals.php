<?php

/**
 * @file
 * These are the global variables that Drupal uses.
 */

/**
 * Timers that have been created by timer_start().
 *
 * @see timer_start()
 * @see timer_read()
 * @see timer_stop()
 */
global $timers;

/**
 * The base URL of the Drupal installation.
 *
 * @see drupal_settings_initialize()
 */
global $base_url;

/**
 * The base path of the Drupal installation.
 *
 * This will at least default to '/'.
 *
 * @see drupal_settings_initialize()
 */
global $base_path;

/**
 * The root URL of the host, excluding the path.
 *
 * @see drupal_settings_initialize()
 */
global $base_root;

/**
 * Array of database connections.
 *
 * @see default.settings.php
 */
global $databases;

/**
 * The domain to be used for session cookies.
 *
 * Cookie domains must contain at least one dot other than the first (RFC 2109).
 * For hosts such as 'localhost' or an IP Addresses the cookie domain will not
 * be set.
 *
 * @see default.settings.php
 */
global $cookie_domain;

/**
 * Array of persistent variables stored in 'variable' table.
 *
 * @see variable_get()
 * @see variable_set()
 * @see variable_del()
 */
global $conf;

/**
 * The name of the profile that has just been installed.
 */
global $installed_profile;

/**
 * Allows the update.php script to be run when not logged in as administrator.
 *
 * @see default.settings.php
 */
global $update_free_access;

/**
 * An object representing the user currently visiting the site.
 *
 * Contains preferences and other account information for logged-in users. When
 * a user is not logged-in, the $user->uid property is set to 0.
 */
global $user;

/**
 * An object containing the information for the active interface language.
 *
 * It represents the language the user interface textual elements such as
 * titles, labels or messages, are to be displayed in.
 *
 * Example values:
 *  - 'language' => 'en',
 *  - 'name' => 'English',
 *  - 'native' => 'English',
 *  - 'direction' => 0,
 *  - 'enabled' => 1,
 *  - 'plurals' => 0,
 *  - 'formula' => '',
 *  - 'domain' => '',
 *  - 'prefix' => '',
 *  - 'weight' => 0,
 *  - 'javascript' => ''
 *
 * @see LANGUAGE_TYPE_INTERFACE
 * @see drupal_language_initialize()
 */
global $language;

/**
 * An object containing the information for the active content language.
 *
 * It is used by the Field API as a default value when no language is specified
 * to select the field translation to be displayed.
 *
 * @see LANGUAGE_TYPE_CONTENT
 * @see drupal_language_initialize()
 */
global $language_content;

/**
 * An object containing the information for the active URL language.
 *
 * It is used as a default value by URL-related functions such as l() when no
 * language is explicitly specified.
 *
 * @see LANGUAGE_TYPE_URL
 * @see drupal_language_initialize()
 */
global $language_url;

/**
 * Array of current page numbers for each pager.
 *
 * @see PagerDefault
 */
global $pager_page_array;

/**
 * Array of the total number of pages for each pager.
 *
 * The array index is the pager element index (0 by default).
 *
 * @see PagerDefault
 */
global $pager_total;

/**
 * Array of the total number of items for each pager.
 *
 * The array index is the pager element index (0 by default).
 *
 * @see PagerDefault
 */
global $pager_total_items;

/**
 * Array of the number of items per page for each pager.
 *
 * The array index is the pager element index (0 by default).
 *
 * @see PagerDefault
 */
global $pager_limits;

/**
 * Name of the active theme.
 */
global $theme;

/**
 * Name of the active theme.
 *
 * @see init_theme()
 */
global $theme_key;

/**
 * Active theme object.
 *
 * @see _drupal_theme_initialize()
 */
global $theme_info;

/**
 * An array of objects that represent the base theme.
 *
 * @see _drupal_theme_initialize()
 */
global $base_theme_info;

/**
 * The theme engine related to the active theme.
 */
global $theme_engine;

/**
 * The path to the active theme.
 */
global $theme_path;

/**
 * The current multibyte mode.
 *
 * Possible values: UNICODE_ERROR, UNICODE_SINGLEBYTE, UNICODE_MULTIBYTE.
 */
global $multibyte;

/**
 * General string or array.
 *
 * @see aggregator_element_start()
 */
global $item;

/**
 * Structured array describing the data to be rendered.
 *
 * @see aggregator_element_start()
 */
global $element;

/**
 * Active tag name.
 *
 * @see aggregator_element_start()
 */
global $tag;

/**
 * Array of items used by aggregator.
 *
 * @see aggregator_element_start()
 */
global $items;

/**
 * An associative array containing title, link, description and other keys.
 *
 * The link should be an absolute URL.
 *
 * @see aggregator_element_start()
 */
global $channel;

/**
 * Current image tag used by aggregator parsing.
 *
 * @see aggregator_aggregator_parse()
 */
global $image;

/**
 * An array of forum topic header information.
 */
global $forum_topic_list_header;

/**
 * Boolean indicating that a menu administrator is running a menu access check.
 */
global $menu_admin;

/**
 * Boolean indicating whether or not the current request is secured by HTTPS.
 */
global $is_https;
