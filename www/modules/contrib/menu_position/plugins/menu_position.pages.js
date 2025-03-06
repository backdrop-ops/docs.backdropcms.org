(function ($) {

/**
 * Provide the summary information for the pages plugin's vertical tab.
 */
Backdrop.behaviors.menuPositionPagesSettingsSummary = {
  attach: function (context) {
    $('fieldset#edit-pages', context).backdropSetSummary(function (context) {
      if (!$('textarea[name="pages"]', context).val()) {
        return Backdrop.t('Any page');
      }
      else {
        return Backdrop.t('Restricted to certain pages');
      }
    });
  }
};

})(jQuery);
