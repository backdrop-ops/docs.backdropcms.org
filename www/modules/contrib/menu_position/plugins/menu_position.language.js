(function ($) {

/**
 * Provide the summary information for the language plugin's vertical tab.
 */
Backdrop.behaviors.menuPositionLanguageSettingsSummary = {
  attach: function (context) {
    $('fieldset#edit-language', context).backdropSetSummary(function (context) {
      return $('select[name="language"] option:selected', context).text();
    });
  }
};

})(jQuery);
