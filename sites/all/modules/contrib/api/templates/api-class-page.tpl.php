<?php

/**
 * @file
 * Displays an API page for a class or interface, including list of members.
 *
 * Available variables:
 * - $alternatives: List of alternate versions (branches) of this class.
 * - $documentation: Documentation from the comment header of the class.
 * - $see: See also documentation.
 * - $deprecated: Deprecated documentation.
 * - $namespace: Name of the namespace for this function, if any.
 * - $implements: List of classes that implements this interface, if any.
 * - $hierarchy: Class hierarchy, if any.
 * - $objects: Listing of member variables, constants, and functions.
 * - $defined: HTML reference to file that defines this class.
 * - $code: HTML-formatted declaration and code for this class.
 * - $related_topics: List of related groups/topics.
 * - $call_links: Links to uses of this class, etc.
 * - $branch: Object with information about the branch.
 * - $object: Object with information about the class.
 *
 * Available variables in the $branch object:
 * - $branch->project: The machine name of the branch.
 * - $branch->title: A proper title for the branch.
 * - $branch->directories: The local included directories.
 * - $branch->excluded_directories: The local excluded directories.
 *
 * Available variables in the $object object:
 * - $object->title: Display name.
 * - $object->object_type: For this template it will be 'class'.
 * - $object->branch_id: An identifier for the branch.
 * - $object->file_name: The path to the file in the source.
 * - $object->summary: A one-line summary of the object.
 * - $object->code: Escaped source code.
 * - $object->see: HTML index of additional references.
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

<?php if (!empty($implements)) { ?>
  <h3><?php print t('Implemented by') ?></h3>
  <?php print $implements ?>
<?php } ?>

<?php if (!empty($hierarchy)) { ?>
  <h3><?php print t('Hierarchy') ?></h3>
  <?php print $hierarchy ?>
<?php } ?>

<?php print $objects; ?>

<h3><?php print t('File'); ?></h3>
<?php print $defined; ?>

<?php print theme('ctools_collapsible', array('handle' => t('View source'), 'content' => $code, 'collapsed' => TRUE)); ?>

<?php if (!empty($related_topics)) { ?>
  <h3><?php print t('Related topics') ?></h3>
  <?php print $related_topics ?>
<?php } ?>

<?php
foreach ($call_links as $link) {
  print $link;
} ?>
