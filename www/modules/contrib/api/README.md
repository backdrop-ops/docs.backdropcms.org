API Module
==========

Generates and displays API documentation pages.

This is an implementation of a subset of the Doxygen documentation generator
specification, tuned to produce output that best benefits the Backdrop code base.
It is designed to assume the code it documents follows Backdrop coding
conventions, and supports Doxygen constructs as documented on
https://api.backdropcms.org/doc-standards.

In addition to standard Doxygen syntax requirements, the following restrictions
are made on the code format. These are all Backdrop coding conventions (see
https://api.backdropcms.org/php-standards for more details and suggestions).

1. All documentation blocks must use the syntax:
   ```
   /**
     * Documentation here.
     */
   ```
   A leading space before the asterisk on subsequent lines is required.

2. When documenting a function, constant, class, etc., the documentation block
   must immediately precede the item it documents, with no intervening blank
   lines.

3. There may be no intervening spaces between a function name and the left
   parenthesis that follows it.

Besides the Doxygen features that are supported, this module also provides the
following features:

1. Functions may be in multiple groups (Doxygen ignores all but the first
   group). This allows, for example, theme_menu_tree() to be marked as both
   "themeable" and part of the "menu system".

2. Function calls to PHP library functions are linked to the PHP manual.

3. Function calls have tooltips briefly describing the called function.

4. Documentation pages have non-volatile, predictable URLs, so links to
   individual functions will not be invalidated when the number of functions in
   a document changes.


Installation and setup
----------------------

See http://drupal.org/node/1516558 for information on how to install and set up
this module.


Views Integration
-----------------

The API documentation is integrated with Views. Actually, most of the
listing pages for the API module use Views, and you should be able to clone
and modify these pages to make your own views if you want something different.


Search Integration
------------------

If you enable the included "API Search Integration" module (machine name:
api_search), as well as either the Backdrop Core "Search" module or the
contributed Apache Solr search module, you can perform full-text searches
on API documentation, just like your regular site content.


Configuring Core Search
-----------------------

1. Enable the Core Search module and the included "API Search Integration"
module.

2. Visit the API search configuration page, and choose which Core
Compatibilities to index (on admin/config/development/api/search).

3. If you already had core Search and the API module installed and running on a
site, and then enabled the "API Search Integration" module later, you will
either need to run a reparse of the API documentation, or a reindex of Search,
in order to get the full text of the API documentation into the search index.
But if you already had Search but not API, or API and not Search, and then
enable the other one plus the search integration module at the same time, the
API documentation will automatically be indexed for you over the next cron
runs.

In either case, once the indexing is complete, you'll be able to do full-text
searches of API documentation under the "Content" search tab (along with other
node content on your site).


Configuring Solr Search
-----------------------

This is a little more involved. Assuming you already have the Apache Solr
module configured and working, and optionally the Facet API module,
here are the steps:

1. Install/enable the included "API Search Integration" module.

2. On your Solr configuration page (admin/config/search/apachesolr), at the
bottom under Configuration, check "API reference entries" under Node, to
make Solr index API reference information.

3. Visit the API configuration page, and choose which Core
Compatibilities to index (on admin/config/development/api/search).

4. Run cron until the index is complete.

5. If you want to do faceted search, on
  admin/config/search/apachesolr/settings/solr/facets
set up the Branch, Project, Core Compatibility, and Object Type facets (or
some subset). If your search is mixing API items with other nodes on the
site, you may also want the "Content type" facet enabled. API nodes will
show up as content type "API Documentation".

6. On the Blocks page, turn on blocks for your search box and facets. If you
also have the API module's "Search" block enabled, you may want to change
the Search block title to distinguish between full-text full-site search and the
search for exact object names in the API module.

7. You will probably also want to do the following, on
admin/config/search/settings :
- Make Apache Solr search the only search module
- Make Apache Solr search the default search module

8. You may need to set up permission "Use search" for appropriate roles.

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.

Maintainers
-----------

This module is currently seeking maintainers.

Credits
-------

Ported to Backdrop CMS by Nate Haug (https://github.com/quicksketch).

The bulk of this module was written and is maintained in the Drupal project
(https://www.drupal.org/project/api) by:

- Neil Drumm (https://www.drupal.org/u/drumm)
- Jennifer Hodgdon (https://www.drupal.org/u/jhodgdon)

This module also currently bundles the "grammar_parser" Drupal library
(https://www.drupal.org/project/grammar_parser) that was written and is
maintained by:

- Jim Berry (https://www.drupal.org/u/solotandem)
