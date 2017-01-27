<?php if ($site_name || $site_slogan): ?>
  <div class="name-and-slogan">
    <?php if ($site_name): ?>
      <?php if (!$is_front): ?>
        <div class="site-name"><strong>
            <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home"><span><?php print $site_name; ?></span></a>
          </strong></div>
      <?php else: /* Use h1 when the content title is empty */ ?>
        <h1 class="site-name">
          <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home"><span><?php print $site_name; ?></span></a>
        </h1>
      <?php endif; ?>
    <?php endif; ?>
    <?php if ($site_slogan): ?>
      <div class="site-slogan"><?php print $site_slogan; ?></div>
    <?php endif; ?>
  </div> <!-- /#name-and-slogan -->
<?php endif; ?>

<?php if ($logo): ?>
  <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home" class="logo">
    <img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" />
  </a>
<?php endif; ?>

<?php if ($menu): ?>
  <nav class="header-menu">
    <?php print $menu; ?>
  </nav>
<?php endif; ?>
