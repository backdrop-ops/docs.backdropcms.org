<?php

/**
 * @file
 * Displays an API page for a class member property.
 *
 * Available variables:
 * - $alternatives: List of alternate versions (branches) of this property.
 * - $override: If this is an override, the text to show for that.
 * - $var: The data type of the property.
 * - $documentation: Documentation from the comment header of the property.
 * - $see: See also documentation.
 * - $deprecated: Deprecated documentation.
 * - $namespace: Name of the namespace for this function, if any.
 * - $defined: HTML reference to file that defines this property.
 * - $class: The text for the class section.
 * - $code: HTML-formatted declaration of this property.
 * - $related_topics: List of related groups/topics.
 * - $branch: Object with information about the branch.
 * - $object: Object with information about the property.
 *
 * Available variables in the $branch object:
 * - $branch->project: The machine name of the branch.
 * - $branch->title: A proper title for the branch.
 * - $branch->directories: The local included directories.
 * - $branch->excluded_directories: The local excluded directories.
 *
 * Available variables in the $object object:
 * - $object->title: Display name.
 * - $object->related_topics: Related information about the function.
 * - $object->object_type: For this template it will be 'property'.
 * - $object->branch_id: An identifier for the branch.
 * - $object->file_name: The path to the file in the source.
 * - $object->summary: A one-line summary of the object.
 * - $object->code: Escaped source code.
 * - $object->see: HTML index of additional references.
 * - $object->var: Type of property.
 *
 * @see api_preprocess_api_object_page()
 *
 * @ingroup themeable
 */
?>

<?php print $alternatives; ?>

<?php print $documentation ?>

<?php if (!empty($var)) { ?>
  <p><strong><?php print t('Type') ?>:</strong> <?php print $var; ?></p>
<?php } ?>

<?php print $override; ?>

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

<h3><?php print t('File'); ?></h3>
 <?php print $defined; ?>

<?php if ($class) : ?>
  <h3><?php print t('Class'); ?></h3>
  <?php print $class; ?>
<?php endif; ?>

<h3><?php print t('Code'); ?></h3>
<?php print $code; ?>

<?php if (!empty($related_topics)) { ?>
  <h3><?php print t('Related topics') ?></h3>
  <?php print $related_topics ?>
<?php } ?>
