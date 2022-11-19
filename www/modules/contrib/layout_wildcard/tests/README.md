Layout Wildcard Test
====================

This module is a helper module used in the automated tests for Layout Wildcard. When enabled, it sets up a View and several Layouts that we use in our test suite.

## View

"Layout test view" (`layout_test_view`) has four page displays with paths

* `layout-test-view`
* `layout-test-view/1`
* `layout-test-view/2`
* `layout-test-view/3`

Each display page displays teasers for all content.


## Layouts

All layouts are initially disabled so that they can be tested individually. 

Each layout has a custom block added to the Top region so that it can be easily identified when it is being used, and the automated tests check for the text in the custom block being present on the page.


### Test alternative paths (`alternative_paths`) 

THis layout has a primary path of `layout-test-view/1`' and has alternative paths of `layout-test-view/2` and `layout-test-view/3`.

When layout `alternative_paths` is enabled, since its primary path `layout-test-view/1` already exists (provided by the View), this layout will override the page with path `layout-test-view/1`. In addition, since this layout has the alternative paths `layout-test-view/2` and `layout-test-view/3`, it will also override those pages. However, it will _not_ override the page at `layout-test-view`.


### Test ancestor matching (`ancestor_matching`)

This layout has a primary path of `layout-test-view` and has ancestor matching turned on.

When layout `ancestor_matching` is enabled, since its primary path `layout-test-view` already exists (provided by the View), this layout will be an override of the first page of the view with path `layout-test-view`.

But since that layout has ancestor matching turned on, it will also override the other three displays of the View because `layout-test-view` is an ancestor of `layout-test-view/1`, `layout-test-view/2`, and `layout-test-view/3`.

Note that `layout-test-view/%` is also an ancestor of `layout-test-view/1`, but that path won't be considered because it has a different number of placeholders than the request path.


### Test context positions (`context_positions`) 

This layout has a primary path of `override/node/%` and an alternative path of `node/%`. It has a Node context set on the placeholder in position 2.

When layout `context-positions` is enabled, it creates a layout-provided page at `override/node/%` (so no main content block), but has a block that displays the Body field of the node specified by the context.

Since it has an alternative path of `node/%`, it overrides the system path `node/%` and so a request path like `node/1` will use the layout to display just the Body field. Since the node ID is in position 1 in this request path, but the context of the layout is set on position 2, the Layout Wildcard module should alter the context to put it in the proper position.
