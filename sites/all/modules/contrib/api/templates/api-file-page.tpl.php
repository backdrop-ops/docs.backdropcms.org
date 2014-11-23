<?php

/**
 * @file
 * Displays an API page for a file and the objects defined in it.
 *
 * Available variables:
 * - $alternatives: List of alternate versions (branches) of this file.
 * - $documentation: Documentation from the comment header of the file.
 * - $see: See also documentation.
 * - $deprecated: Deprecated documentation.
 * - $namespace: Name of the namespace for this function, if any.
 * - $objects: List of functions, classes, etc. defined in the file.
 * - $call_links: Links to calling functions (for theme templates).
 * - $code: Source code for the file.
 * - $related_topics: List of related groups/topics.
 * - $defined: Location of the file.
 * - $branch: Object with information about the branch.
 * - $object: Object with information about the file.
 *
 * Available variables in the $branch object:
 * - $branch->project: The machine name of the branch.
 * - $branch->title: A proper title for the branch.
 * - $branch->directories: The local included directories.
 * - $branch->excluded_directories: The local excluded directories.
 *
 * Available variables in the $object object:
 * - $object->title: Display name.
 * - $object->object_type: For this template it will be 'file'.
 * - $object->branch_id: An identifier for the branch.
 * - $object->file_name: The path to the file in the source.
 * - $object->summary: A one-line summary of the object.
 * - $object->code: Escaped source code.
 * - $object->see: HTML-formatted additional references.
 *
 * @see api_preprocess_api_object_page()
 *
 * @ingroup themeable
 */
?>

<?php print $alternatives; ?>

<?php print $documentation ?>

<?php if ($namespace) : ?>
  <h3><?php print t('Namespace'); ?></h3>
  <?php print $namespace; ?>
<?php endif; ?>

<?php if (!empty($deprecated)) { ?>
<div class="api-deprecated">
  <h3><?php print t('Deprecated') ?></h3>
  <?php print $deprecated ?>
</div>
<?php } ?>

<?php if (!empty($see)) { ?>
  <h3><?php print t('See also') ?></h3>
  <?php print $see ?>
<?php } ?>

<?php print $objects; ?>

<?php
foreach ($call_links as $link) {
  print $link;
}
?>

<h3><?php print t('File'); ?></h3>
 <?php print $defined; ?>

<?php print theme('ctools_collapsible', array('handle' => t('View source'), 'content' => $code, 'collapsed' => TRUE)); ?>

<?php if (!empty($related_topics)) { ?>
  <h3><?php print t('Related topics') ?></h3>
  <?php print $related_topics ?>
<?php } ?>
