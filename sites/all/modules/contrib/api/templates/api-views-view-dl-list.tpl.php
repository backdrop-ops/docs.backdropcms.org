<?php

/**
 * @file
 * View style template to display a DL list.
 *
 * - $title : The title of this group of rows.  May be empty.
 * - $rows: The rows to print. The DT and DD tags must be included.
 */
?>
<?php print $wrapper_prefix; ?>
  <?php if (!empty($title)) : ?>
    <h3><?php print $title; ?></h3>
  <?php endif; ?>
  <dl <?php print $list_class; ?>>
    <?php foreach ($rows as $id => $row): ?>
      <?php print $row; ?>
    <?php endforeach; ?>
  </dl>
<?php print $wrapper_suffix; ?>
