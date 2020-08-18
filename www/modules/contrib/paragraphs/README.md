Paragraphs
==========

Paragraph is a module to create paragraphs in your content.
You can create bundles(with own display and fields) as paragraph types.

When you add a paragraph field to your node/entity, you can select the allowed bundles, and when using the field, you can select a paragraph type from the allowed bundles to use different fields/display per paragraph.


Installation and usage
------------

1. Copy  to your modules directory
2. Enable the module at the module administration page
3. Create a Paragraph Bundle
  - Go to Structure->Paragraph Bundles (admin/structure/paragraphs) and click 'Add Paragraph Bundle'. (Alternatively you can navigate directly to admin/structure/paragraphs/add)
  - Enter 'Content text' as Name, and click 'Save Paragraph bundle'
4. Add a field to a Paragraph Bundle
  - Example for adding a text field which can be used for adding content:
    - Click 'manage fields'
    - At 'Add new field'-> enter:
      - Label: 'Text'
      - Field type: 'Long text' 
    - and click the 'Save' button
    - click 'Save field settings'
    - at the bottom of that next page click 'Save settings'
  - Hide field labels
    - If you want to hide the label of the field you created from the page visitor:
      - click on tab 'Manage display'
      - Set the label of 'Paragraphs' to 'hidden' and click 'save'
5. Set permissions for your Paragraph Bundle
  - Go to /admin/people/permissions
  - Under "Paragraphs Bundle Permissions" set appropriate permissions for user roles to view, create, edit or delete your Paragraph Bundle
  - Note that if you don't set at least one user role to be able to view the Bundle, it will be invisible to everyone on the front end, including administrator role
  - Click 'Save permissions'
6. Add a Paragraphs field to your Content Type
  Go to Structure->Content types (admin/structure/types)
  - click 'manage fields' at the content type you want to add the Paragraph field to
  - At 'Add new field' enter:
    - Label: 'Paragraphs'
    - Field type: 'Paragraphs'
    - Leave widget on 'Embedded'
  - and click the 'Save' button
  - at 'Paragraphs field settings' set 'Number of values' to 'Unlimited'
  - click 'Save settings'
  - Hide field labels
  - If you want to hide the label of the field you created from the front-end page visitor:
    - click on tab 'Manage display'
    - Set the label of 'Paragraphs' to 'hidden' and click 'save'
7. Result
  - The content editor of your Backdrop site will now be able to add Paragraphs to the content type you selected.
  - The Paragraph content will display the same way as regular body content does after a default Backdrop installation.

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.

Current Maintainers
-------------------

 - [Laryn Kragt Bakker](https://github.com/laryn) - [CEDC.org](https://cedc.org)

Credits
-------

 - Ported to BackdropCMS by [@docwilmot](https://github.com/docwilmot)

 - Maintainers on drupal.org
    - [berdir](https://www.drupal.org/u/berdir)
    - [miro_dietiker](https://www.drupal.org/u/miro_dietiker)
    - [primsi](https://www.drupal.org/u/primsi)
    - [jeroenb](https://www.drupal.org/u/jeroenb)
