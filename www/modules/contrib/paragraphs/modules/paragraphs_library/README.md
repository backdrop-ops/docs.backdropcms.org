# Paragraphs Library

The Paragraphs Library module provides support for reusing paragraphs.
This has been achieved through a Library Item entity type that can be later
referenced as a normal pargraph.

Paragraphs that can be reused are displayed on the "Paragraphs library"
overview page (i.e. admin/content/paragraphs).

## Limitations
The module provides an access control handler with limited functionality.
Since Library Item entity has no concept of the parent, the access check
is forwarded to the referenced entity which only ensures if the referenced
paragraph is enabled.

Be aware when using paragraph library items that need to depend on the host's
entity access as its limited accessibility is not guaranteed.

When a Library item is referenced, labels from referenced paragraph and library
item itself are displayed. For the Paragraph type 'From library', which is
provided in Paragraphs Library module by default, those labels are removed in a
preprocess hook. If you want the same behavior for some custom paragraph type
with Library item, you need to implement your own hook or change the existing
one.

## Current Maintainers

- [Laryn Kragt Bakker](https://github.com/laryn).

## Credits

- This module made heavy use of various Entity-related examples provided by
  @argiepiano as a shortcut / starting point. See
  - [Basic Entity Plus](https://github.com/backdrop-contrib/basic_entity_plus) and
  - [Entity Demo](https://github.com/argiepiano/ed)
- Although not a direct port, much inspiration was taken from the
  [modern Drupal version](https://git.drupalcode.org/project/paragraphs/-/tree/8.x-1.x/modules/paragraphs_library?ref_type=heads) of this module.

## License

This project is GPL v2 software.
See the LICENSE.txt file included with this module for complete text.

