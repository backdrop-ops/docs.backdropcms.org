<?php

/**
 * @file
 * Displays an API page for a topic/group, with list of items in that group.
 *
 * Available variables:
 * - $alternatives: List of alternate versions (branches) of this group.
 * - $documentation: Documentation from the comment header of the constant.
 * - $see: Related API objects.
 * - $objects: Formatted list of member objects, if any.
 * - $defined: HTML reference to file that defines this group.
 *
 * @ingroup themeable
 */
?>

<?php print $alternatives; ?>

<?php print $documentation ?>

<?php if (!empty($see)) { ?>
  <h3><?php print t('See also') ?></h3>
  <?php print $see ?>
<?php } ?>

<?php print $objects; ?>

<h3><?php print t('File'); ?></h3>
<?php print $defined; ?>
