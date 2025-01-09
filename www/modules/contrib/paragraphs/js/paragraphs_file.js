/**
 * @file
 * Provides JavaScript for Paragraphs.
 */

(function ($) {

  /**
   * Allows submit buttons in entity forms to trigger uploads by undoing
   * work done by Backdrop.behaviors.fileButtons.
   */
  Backdrop.behaviors.paragraphsFile = {
    attach: function (context) {
      if (Backdrop.file) {
        $('input.paragraphs-add-more-submit', context).off('mousedown', Backdrop.file.disableFields);
      }
    },
    detach: function (context) {
      if (Backdrop.file) {
        $('input.form-submit', context).on('mousedown', Backdrop.file.disableFields);
      }
    }
  };

})(jQuery);
