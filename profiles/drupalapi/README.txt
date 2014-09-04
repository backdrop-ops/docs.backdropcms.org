The DrupalAPI Installation profile sets up a DrupalAPI reference site,
automatically indexing all files and setting up the site for instant use.

This is particularly useful for developers who work offline or in areas with
spotty connectivity, but still want access to API docs.

The DrupalAPI Installation profile was written and is maintained by
Stuart Clark (deciphered) and Brian Gilbert (realityloop) of Realityloop Pty Ltd
- http://www.realityloop.com
- http://twitter.com/realityloop



FEATURES
--------------------------------------------------------------------------------

- Core projects:
  - Drupal 6.26
  - Drupal 7.15



INSTALLATION
--------------------------------------------------------------------------------

Ensure your PHP memory_limit is set high enough, we've confimed that the following works:
memory_limit = 256M

Install as per a normal Drupal site, being sure to use the 'DrupalAPI'
installation profile.



UPDATING API REFERENCE SOURCES
--------------------------------------------------------------------------------

Note: The first time this is run after a fresh install or upgrade it will take a
      while as the DrupalAPI reference sources need to be redownloaded and
      linked to their GIT repository.


1. Navigate to the 'DrupalAPI' profile directory from command line and run the
   following command:

      ./update.sh



TODO / ROADMAP
--------------------------------------------------------------------------------

- Add ability to configure branches during installation.
- Add ability for branches to be downloaded during installation.
- Add more core and contrib projects as default (or configurable).
