<?php
/**
 * @file
 * Displays the documentation for a file and line number where an object is.
 *
 * Available variables:
 * - $file_link: Themed link to the file the object is in.
 * - $file_summary: Summary line for the file the object is in.
 * - $object: Object with information about the object being referenced.
 * - $branch: Object with information about the branch.
 *
 * Available variables in the $branch object:
 * - $branch->project: The machine name of the branch.
 * - $branch->title: A proper title for the branch.
 * - $branch->directories: The local included directories.
 * - $branch->excluded_directories: The local excluded directories.
 *
 * Available variables in the $object object:
 * - $object->title: Display name.
 * - $object->summary: Short summary.
 * - $object->documentation: HTML formatted comments.
 * - $object->code: HTML formatted source code.
 * - $object->start_line: The line number where the definition for this object
 *   starts in the containing file.
 *
 * @see api_preprocess_api_defined()
 *
 * @ingroup themeable
 */
?>
<p class="api-defined">
<dl>
  <dt><?php print t('!file, line @start_line', array('!file' => $file_link, '@start_line' => (($startline = $object->start_line) == NULL ? 0 : $startline) ) ); ?></dt>
  <dd><?php print $file_summary; ?></dd>
</dl>
</p>
