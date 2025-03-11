<?php
/**
 * Remove the title from all menu blocks.
 */
?>
<div class="<?php print implode(' ', $classes); ?>">
  <?php if ($delta == 'main-menu'): ?>
    <?php print render($title_prefix); ?>
    <?php if ($title): ?>
      <h2 class="block-title"><?php print $title; ?></h2>
    <?php endif; ?>
    <?php print render($title_suffix); ?>
  <?php endif; ?>

  <?php print render($content); ?>
</div>
