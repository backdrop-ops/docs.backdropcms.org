(function ($) {

/**
 * Provide the summary information for the taxonomy plugin's vertical tab.
 */
Backdrop.behaviors.menuPositionTaxonomy = {
  attach: function (context) {
    $('fieldset#edit-taxonomy', context).backdropSetSummary(function (context) {
      if ($('input[name="term"]', context).val()) {
        return Backdrop.t('Taxonomy: %term', {'%term' : $('input[name="term"]', context).val()});
      }
      else if ($('select[name="vocabulary"]', context).val() != 0) {
        return Backdrop.t('Vocabulary: %vocab', { '%vocab': $('select[name="vocabulary"] option:selected', context).text()});
      }
      else {
        return Backdrop.t('Any vocabulary or taxonomy');
      }
    });
    // Reset the taxonomy term auto-complete object when the vocabulary changes.
    $('fieldset#edit-taxonomy #edit-vocabulary', context).change(function () {
      $input = $('#edit-term');
      // Remove old terms.
      $input.val('');
      // Unbind the original auto-complete handlers.
      $input.unbind('keydown');
      $input.unbind('keyup');
      $input.unbind('blur');
      // Set new autocomplete handlers.
      uri = Backdrop.settings.menu_position_taxonomy_url + '/' + $(this).val();
      $('#edit-term-autocomplete').val(uri);
      new Backdrop.jsAC($input, new Backdrop.ACDB(uri));
    });
  }
};

})(jQuery);
