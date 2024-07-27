/**
 * @file
 * Paragraphs actions JS code for paragraphs actions button.
 */

(function ($, Backdrop) {

  'use strict';

  /**
   * Process paragraph_actions elements.
   *
   * @type {Backdrop~behavior}
   *
   * @prop {Backdrop~behaviorAttach} attach
   *   Attaches paragraphsActions behaviors.
   */
  Backdrop.behaviors.paragraphsActions = {
    attach: function (context, settings) {
      var $actionsElement = $('.paragraphs-actions-more').once('paragraphs-actions-more', context);
      // Attach event handlers to toggle button.
      $actionsElement.each(function () {
        var $this = $(this);

        $this.on('focusout', function (e) {
          setTimeout(function () {
            if ($this.has(document.activeElement).length == 0) {
              // The focus left the action button group, hide actions.
              $this.removeAttr('open');
            }
          }, 1);
        });
      });
      $(document).on('keydown.paragraphsActions', function (event) {
        if (event.key === 'Escape') {
          $('.paragraphs-actions-more').removeAttr('open');
        }
      });

    },
    detach: function (context) {
      $(document).off('keydown.paragraphsActions');
    }
  };

})(jQuery, Backdrop);
