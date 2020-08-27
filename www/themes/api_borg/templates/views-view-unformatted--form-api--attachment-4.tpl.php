<?php
/**
 * @file
 * Default simple view template to display a list of rows.
 *
 * - $title: The title of this group of rows.  May be empty.
 * - $rows: An array of row items. Each row is an array of content.
 *   $rows are indexed by row number.
 * - $row_classes: An array of classes to apply to each row, indexed by row
 *   number. This matches the index in $rows.
 *
 * @ingroup views_templates
 */
foreach ($rows as $i => $row) {
  $rows[$i] = trim($row);
}
?>

<?php print implode(', ', $rows); ?>
