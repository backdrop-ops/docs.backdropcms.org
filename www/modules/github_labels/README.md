GitHub Labels
=============

The GitHub Labels module adds tokens and CSS to a site that allows the relatively easy inclusion of colored GitHub labels from one or more repositories in page content. A typical use case would be a website that contains documentation of Github-based workflow(s),

GitHub Labels is used to provide the labels on the Backdrop CMS documentation site [https://docs.backdropcms.org](https://docs.backdropcms.org).

Installation
------------

- Install this module using [the official Backdrop CMS instruction](https://backdropcms.org/guide/modules).

- Visit the GitHub Labels configuration page at **Administration > Configuration >
  Content > GitHub labels** (admin/config/content/github_labels) and set the repositories to pull label definitions from.

- If you will be using the tokens in body text, install and enable the [Token Filter module](https://backdropcms.org/project/token_filter).

- Alter text formats (at admin/config/content/formats) if needed to allow authorized users to enter HTML of the form specified below and/or to use tokens in filtered text.

Usage
-------------------------

GitHub labels are incorporated into HTML by creating empty `<span...></span>` tag pairs with special classes, similar to how the popular icon package [Font Awesome](https://fontawesome.com) supports its icons. The GitHub label HTML will be rendered with the text and color defined for the label in the specified GitHub repositories.

A typical example is:

`<span class="gh-label gh-label-pr-needs-work"></span>` for a label titled "PR - Needs Work".

If you install and enable the Token Filter module, then you can use tokens in filtered text, which are simpler:

`[gh-label:pr-needs-work]` for a label titled "PR - Needs Work".

Tokens are compatible with WYSIWYG editors like CKEditor, but be aware that if you give users the ability to use tokens in filtered text, they can enter _any_ defined token and it will be rendered.

One possible use case is to hyperlink the label to another page (for example, the GitHub issue queue filtered to that label). Since themed hyperlinks often have special text color and/or text-decoration defined that would override the formatting defined by GitHub labels, you should wrap GitHub Labels HTML with an `<a>` tag with the `gh-label-wrapper` class, like this:

`<a class="gh-label-wrapper" href="..."><span class="gh-label gh-label-pr-needs-work"></span></a>`

Or you can use a token that links to the filtered issue queue:

`[gh-label-link:pr-needs-work]` for a label titled "PR - Needs Work" that links to the repository issue queue, filtered to the "PR - needs work" label.

Be aware that if you have labels in multiple repositories with the same name, the label HTML will use the coloration of the first repository that contains that label, and the link HTML token will link to the issue queue of the first repository that contains that label.

A complete list of labels, tokens, and the label HTML is provided at admin/reports/github_labels.

Issues
------

Bugs and feature requests should be reported in [the issue queue](https://github.com/backdrop-contrib/github_labels/issues).

Current Maintainers
-------------------

- [Robert J. Lang](https://github.com/bugfolder).

Credits
-------

- Written for Backdrop CMS by [Robert J. Lang](https://github.com/bugfolder).
- Approach suggested by [klonos](https://github.org/klonos).

License
-------

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.

