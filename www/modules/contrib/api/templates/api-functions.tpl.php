<?php

/**
 * @file
 * Displays a list of function/file references.
 *
 * Available variables:
 * - $functions: Array of items to display. Each is an array with the following
 *   elements:
 *   - function: HTML-formatted link to the item.
 *   - file: HTML-formatted link to the file is in.
 *   - description: Description of the item.
 *
 * @ingroup themeable
 */
?>
<dl class="api-functions">
<?php foreach ($functions as $function) { ?>
  <dt><?php print $function['function'] ?> <small>in <?php print $function['file'] ?></small></dt>
  <dd><?php print $function['description'] ?></dd>
<?php } ?>
</dl>
