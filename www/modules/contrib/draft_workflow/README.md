Draft Workflow
========

This module expands on Backdrop's core revision handling to support a draft workflow —
the ability to create and manage unpublished draft versions of content that is already
published.

![image](https://github.com/user-attachments/assets/15faed0d-d8cd-4e0b-8404-444a61d773ed)

1) This "Current" revision is the version that site visitors will see when they visit your
website.

2) This version contains changes to the "Current" version that are not yet visible to
site visitors. When this revision is ready, you can make it or any other revision your
"Current" revision.

![image](https://github.com/user-attachments/assets/a832d7fd-0ac2-49b5-aaef-20bf52547524)

Using this module
-----------------

Once installed, navigate to the **Revisions** tab on any node to manage its revisions.
From there you can:

- **Publish or unpublish** the content using the buttons at the top of the page.
- **Copy the current revision** to create a new draft without affecting what is
  currently live.
- **View, edit, or copy any revision** using the action links in the revisions table.
- **Make any revision the current revision** — the version site visitors will see.

When editing or copying a non-current revision, the publishing options are hidden to
avoid confusion. Instead, a single "Make this the current revision" checkbox is
available in the Revision information section.

Pending Review view
-------------------

The module provides a **Pending Review** view at `admin/content/drafts` (also
available as a tab alongside the main Content list). It shows all nodes that have
draft revisions newer than the currently published revision, along with the latest
draft note left by an editor.

A **Pending Review block** is also available. Add it to a dashboard or admin layout
to show a quick count of content awaiting review, with a link to the full list.

Configuration
-------------

### Permissions

The module defines four permissions that allow you to separate editorial roles:

| Permission | Description |
|---|---|
| **Create draft revisions** | Create new draft revisions and edit existing draft revisions. |
| **Activate draft revisions** | Make a draft revision current on an **unpublished** node. |
| **Publish and unpublish content** | Publish and unpublish nodes; also required to activate revisions on already-published nodes, since doing so changes what is publicly visible. |
| **View pending draft revisions** | Access the Pending Review view and dashboard block. |

A typical two-role setup might look like:

- **Editor**: Grant _Create draft revisions_ and _View pending draft revisions_.
- **Publisher**: Grant all four permissions.

> **Important:** Users without _Publish and unpublish content_ cannot directly edit a
> published node via its standard edit form — saving the form would immediately publish
> their changes. Instead, editors should use the **Revisions** tab to copy or edit a
> draft revision, then leave it for a publisher to review and make current.

### Make revision information more prominent

By default, revision information appears inside the sidebar tabs on the content
editing form. If your editorial workflow relies heavily on revisions, you can make
this section always visible by enabling a per-content-type setting:

1. Go to **Admin > Structure > Content Types**.
2. Click **Edit** for the content type you want to configure.
3. Open the **Revision** section.
4. Check **Display revision information prominently**.

When enabled, the revision information fieldset will appear above the sidebar tabs
on all editing forms for that content type, making it immediately visible without
requiring an extra click.

Related Modules
---------------

### Diff

Install the [Diff module](https://backdropcms.org/project/diff) to enable
side-by-side comparisons between revisions. When Diff is active, a **Diff**
link appears in the revisions table, letting editors and reviewers quickly see
exactly what changed between a draft and the current live version.

Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://docs.backdropcms.org/documentation/extend-with-modules.


Issues
------

Bugs and Feature Requests should be reported in the Issue Queue:
https://github.com/backdrop-contrib/draft_workflow/issues.


Current Maintainers
-------------------

- [Joseph Flatt](https://github.com/hosef)
- Tim Erickson ([@stpaultim](https://github.com/stpaultim)).


License
-------

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.
