Menu Pager
==========

Menu Pager adds blocks for each menu that display previous/next or child links,
based on the current page.

For example, given the following menu hierarchy, if you're on the 'Installing
software' page...

```
- Setting up your OS
  - Installing
  - Configuring
- Installing software
  - App store introduction
  - Installing an app
  - Finding good-quality apps
- Getting help & support
```

The 'Menu pager' block will show a 'previous' link pointing to 'Configuring',
and a 'next' link pointing to 'App store introduction'.

The 'Menu pager (children)' block will show the following links:

```
- App store introduction
- Installing an app
- Finding good-quality apps
```

Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules.

- Add a Menu Pager block to a layout to display previous/next links, or a Menu
  Pager (Children) block to display child links (or both!).

Issues
------

Bugs and Feature requests should be reported in the Issue Queue:
https://github.com/backdrop-contrib/menu_pager/issues.

Current Maintainers
-------------------

- [Peter Anderson](https://github.com/BWPanda)

Credits
-------

- Ported to Backdrop CMS by [Peter Anderson](https://github.com/BWPanda).
- Originally written for Drupal by [Joel Stein](https://www.drupal.org/u/joelstein).

License
-------

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.
