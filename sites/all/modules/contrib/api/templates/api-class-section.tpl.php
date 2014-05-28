<?php
/**
 * @file
 * Displays the documentation for a class containing a member.
 *
 * Available variables:
 * - $class_link: Themed link to the class.
 * - $class_info: Summary line for the class.
 * - $class: Object with information about the class.
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
 *
 * @see api_preprocess_api_class_section()
 *
 * @ingroup themeable
 */
?>
<p class="api-class-section">
<dl>
  <dt><?php print $class_link; ?> </dt>
  <dd><?php print $class_info; ?></dd>
</dl>
</p>
