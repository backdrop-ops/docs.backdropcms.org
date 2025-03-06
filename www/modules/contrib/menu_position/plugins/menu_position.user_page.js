(function ($) {

/**
 * Provide the summary information for the content type plugin's vertical tab.
 */
Backdrop.behaviors.menuPositionUserPageSettingsSummary = {
  attach: function (context) {
    $('fieldset#edit-user-page', context).backdropSetSummary(function (context) {
      if (!$('input#edit-user-page-enable:checked').length) {
        return Backdrop.t('Ignore user account page');
      }
      else {
        return Backdrop.t('User account page');
      }
    });
  }
};

})(jQuery);
