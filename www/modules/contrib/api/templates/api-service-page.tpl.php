<?php

/**
 * @file
 * Displays an API page for a service, including a list of tags.
 *
 * Available variables:
 * - $alternatives: List of alternate versions (branches) of this class.
 * - $defined: HTML reference to file that defines this service.
 * - $code: HTML-formatted declaration and code for this service.
 * - $tags: List of tags for this service.
 * - $branch: Object with information about the branch.
 * - $object: Object with information about the service.
 * - $call_links: Links to references.
 *
 * Available variables in the $branch object:
 * - $branch->project: The machine name of the branch.
 * - $branch->title: A proper title for the branch.
 * - $branch->directories: The local included directories.
 * - $branch->excluded_directories: The local excluded directories.
 *
 * Available variables in the $object object:
 * - $object->title: Display name.
 * - $object->object_type: For this template it will be 'service'.
 * - $object->branch_id: An identifier for the branch.
 * - $object->file_name: The path to the file in the source.
 * - $object->summary: A one-line summary of the object.
 * - $object->code: Escaped source code.
 *
 * @see api_preprocess_api_object_page()
 *
 * @ingroup themeable
 */
?>

<?php print $alternatives; ?>

<h3><?php print t('Class'); ?></h3>
<?php print $class ?>

<?php
  if ($tags) {
?>
     <h3><?php print t('Tags'); ?></h3>
<?php
     print $tags;
  }
?>

<?php
foreach ($call_links as $link) {
  print $link;
} ?>

<h3><?php print t('File'); ?></h3>
<?php print $defined; ?>

<?php print api_collapsible(t('View source'), $code); ?>
