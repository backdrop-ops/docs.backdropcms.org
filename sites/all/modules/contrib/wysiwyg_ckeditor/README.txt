CKEditor for WYSIWYG Module
===========================

This module integrates CKEditor 4.x with the WYSIWYG module for Drupal 7. It's
implementation is similar to that of Drupal 8's built-in WYSIWYG editor.

Features
--------

- Bundled lightweight CKEditor 4.x version, optimized for Drupal integration
  (no unneeded plugins makes for faster loading). No need to download the editor
  separately.
- Additional add-on plugins that tightly integrate Drupal with CKEditor.
- Dependency on WYSIWYG module keeps compatibility with other WYSIWYG editors.
- Uses absolutely no inline styles. All buttons and cleanup uses a consistent
  set of classes which can be themed.

Installation
------------

If you have installed the "CKEditor" module in the past from
http://drupal.org/project/ckeditor, you'll need to uninstall that module before
using this module.

This module bundles CKEditor with the code, so no separate download is required.
Enable this module and WYSIWYG module. Visit the "WYSIWYG Profiles" setting page
and configure a text format to use the "CKEditor (extended)" editor.

Theming
-------

Themes may specify iframe-specific CSS files for use with CKEditor by including
a "ckeditor_stylesheets" key in the theme .info file.

ckeditor_stylesheets[] = css/wysiwyg.css

These CSS files will automatically be included when the editor is loaded.
