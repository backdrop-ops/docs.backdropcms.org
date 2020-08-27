/**
 * @file
 * Provides JavaScript for Paragraphs.
 */

(function ($) {

  /**
   * Allows submit buttons in entity forms to trigger uploads by undoing
   * work done by Backdrop.behaviors.fileButtons.
   */
  Backdrop.behaviors.paragraphs = {
    attach: function (context) {
      if (Backdrop.file) {
        $('input.paragraphs-add-more-submit', context).unbind('mousedown', Backdrop.file.disableFields);
      }
    },
    detach: function (context) {
      if (Backdrop.file) {
        $('input.form-submit', context).bind('mousedown', Backdrop.file.disableFields);
      }
    }
  };

})(jQuery);
