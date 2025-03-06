(function ($) {

/**
 * Provide the summary information for the lolspeak plugin's vertical tab.
 */
Backdrop.behaviors.menuPositionPLUGINSettingsSummary = {
  attach: function (context) {
    $('fieldset#edit-PLUGIN', context).backdropSetSummary(function (context) {
      var vals = [];
      $('input[type="checkbox"]:checked', context).each(function () {
        vals.push($.trim($('label[for="' + $(this).attr('id') + '"]').text()));
      });
      if (!vals.length) {
        vals.push(Backdrop.t('Any language'));
      }
      return vals.join(', ');
    });
  }
};

})(jQuery);
