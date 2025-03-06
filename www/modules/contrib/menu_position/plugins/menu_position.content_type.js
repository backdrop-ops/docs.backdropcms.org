(function ($) {

/**
 * Provide the summary information for the content type plugin's vertical tab.
 */
Backdrop.behaviors.menuPositionContentTypeSettingsSummary = {
  attach: function (context) {
    $('fieldset#edit-content-type', context).backdropSetSummary(function (context) {
      var vals = [];
      $('input[type="checkbox"]:checked', context).each(function () {
        vals.push($.trim($('label[for="' + $(this).attr('id') + '"]').text()));
      });
      if (!vals.length) {
        vals.push(Backdrop.t('Any content type'));
      }
      return vals.join(', ');
    });
  }
};

})(jQuery);
