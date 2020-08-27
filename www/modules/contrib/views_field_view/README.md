Views Field View
================

This module allows you to embed a View as a field in another View.

The View in the view-field can accept argument values from other fields of the
parent View, using tokens.


Installation
------------

- Install this module using the official Backdrop CMS instructions at
 https://backdropcms.org/guide/modules.

How to use
----------

1. Before you can add a view-field to a "parent" view, you must create a
   "child" view.

2. Add contextual filters to that child view. The parent view will be passing
   contextual filter values to the child so the child knows what to display. No
   other settings are necessary, but validators and "contextual filter not
   present" could be set.

3. Create a "parent" view, if not already existing.

4. Add child view (Global:View field). The field must be toward the bottom of
   the field list, or at least underneath the fields that are going to be used
   as contextual filters. (E.g., "node id" might be used as an match between
   the parent and child views, so put the "node id" field before your
   Global:View field in the list.)

5. Select which View and Display to use for the child data (will require
   doing this in 2 steps - the field must be saved before the display selection
   is available).

6. Find which tokens are available by looking at the "Replacement patterns"
   list right below the contextual filters setting. Type the token replacements,
   in order, separated by a comma, as the contextual filters for that view
   field. These values will be passed to the child. Make sure each field you
   are passing as an contextual filter is completely clean, as in: no label,
   no formatting, nothing that would pass into the contextual filter other than
   the desired text or number.

Documentation
-------------

Additional documentation is located in the Wiki:
https://github.com/backdrop-contrib/views_field_view/wiki/Documentation.

Issues
------

Bugs and Feature requests should be reported in the Issue Queue:
https://github.com/backdrop-contrib/views_field_view/issues.

Current Maintainers
-------------------

- Jen Lanmpton (https://github.com/jenlampton).
- Seeking additional maintainers

Credits
-------

- Ported to Backdrop CMS by [Matthew Connerton](https://github.com/mrconnerton).
- Maintained for Drupal by [Daniel Wehner](https://www.drupal.org/u/dawehner).
- Maintained for Drupal by [Jibran Ijaz](https://www.drupal.org/u/jibran).
- Maintained for Drupal by [Rohit Joshi](https://www.drupal.org/u/joshirohit100).
- Originally written for Drupal by [Howard Tyson](https://www.drupal.org/u/tizzo).

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
