/**
 * JavaScript additions to the API site.
 */
(function ($) {

"use strict";

Backdrop.behaviors.apiTheme = {};
Backdrop.behaviors.apiTheme.attach = function(context, settings) {
  Backdrop.apiTheme.rotateHeaders(context, settings);
  Backdrop.apiTheme.autocomplete(context, settings);
  Backdrop.apiTheme.headingPermalinks(context, settings);
};

Backdrop.apiTheme = {};

/**
 * Rotate table headers that have a data-rotate attribute.
 */
Backdrop.apiTheme.rotateHeaders = function(context, settings) {
  // Rotate table headers 45 degrees. CSS added in style.css.
  $(context).find('tr[data-rotate]').once('rotate').each(function() {
    var $row = $(this);
    var degrees = parseFloat($row.data('rotate'));
    if (degrees < 0) {
      degrees += 360;
    }

    var widest_cell = 0;
    $row.children('th').not(':first').each(function() {
      var $cell = $(this);
      if ($cell.width() > widest_cell) {
        widest_cell = $cell.width();
      }
      $cell.wrapInner('<span class="rotate1"><span class="rotate2"></span></span>');

      $cell.css({
        verticalAlign: 'bottom',
        border: 0
      });

      // Remove the header and calculate cell width.
      $cell.children('.rotate1')
          .hide()
          .css({
            transform: 'translate(' + $cell.width() + 'px, ' + (Math.ceil($cell.width() / 2) - 1) + 'px) rotate(' + degrees + 'deg)',
            display: 'block',
            whiteSpace: 'nowrap',
            width: ($cell.width() + 1) + 'px'
          })
          .show();
    });

    // A^2 + B^2 = C^2. We have the C value and both A and B are equal.
    var newHeight = Math.round(Math.sqrt(Math.pow(widest_cell, 2) / 2));
    $row.height(newHeight + 15);

    $row.find('.rotate2').css({
      width: widest_cell + 'px',
      display: 'inline-block'
    });

    var $table = $(this).closest('table').addClass('sticky-enabled');
    var $firstCell = $table.find('thead th:first');
    $firstCell.width($firstCell.width());
    Backdrop.attachBehaviors($table.parent());

    $table.prev('table').addClass('sticky-rotated');
  });
};

/**
 * Replace the normal autocomplete with Twitter typeahead.js.
 */
Backdrop.apiTheme.autocomplete = function(context, settings) {
  $(context).find('input[data-typeahead]').once('typeahead').each(function() {
    var $element = $(this);

    // Filter function passed into Bloodhound below. Converts our api.module
    // autocomplete into a format suitable for typeahead.
    var apiFilter = function(response) {
      var filtered = [];
      for (var n in response) {
        if (response.hasOwnProperty(n)) {
          filtered.push({
            value: n,
            display: response[n]
          });
        }
      }
      return filtered;
    };

    // Create and initialize the data selector.
    var apiIndex = new Bloodhound({
      datumTokenizer: Bloodhound.tokenizers.obj.whitespace('value'),
      queryTokenizer: Bloodhound.tokenizers.whitespace,
      limit: 10,
      remote: {
        url: $element.data('typeahead') + '/%QUERY',
        filter: apiFilter
      }
    });
    apiIndex.initialize();

    $element.typeahead(null, {
      name: 'api',
      displayKey: 'value',
      source: apiIndex.ttAdapter()
    });

    // On selection or autocomplete, use the autocomplete URL.
    $element.on('typeahead:selected typeahead:autocompleted', function(e, suggestion, dataset) {
      var url = $element.data('typeahead-selected');
      window.location = url + '/' + suggestion.value;
    });

    // Upon submission of the form, if the typed in value matches a suggestion
    // identically, use the autocomplete URL. This is useful for short
    // suggesions like the t() or url() functions.
    $element.closest('form').submit(function(e) {
      // If triggered by jQuery, let normal processing run its course.
      if (e.isTrigger) {
        return;
      }

      var $form = $(this);
      apiIndex.get($element.val(), function(suggestions) {
        for (var n in suggestions) {
          if (suggestions.hasOwnProperty(n)) {
            var suggestion = suggestions[n];
            if (suggestion.value.indexOf($element.val()) === 0) {
              var url = $element.data('typeahead-selected');
              window.location = url + '/' + suggestion.value;
            }
            break;
          }
        };
      });

      // Try submitting the form in 1 second.
      setTimeout(function() {
        $form.trigger('submit', [true]);
      }, 1000);

      // Stop the normal form submission.
      return e.preventDefault();
    });
  });
};

/**
 * Add a permalink to all headings with an ID attribute.
 */
Backdrop.apiTheme.headingPermalinks = function(context, settings) {
  $(context).find('.block-backdropapi-form-api-table :header[id], .view-form-api.view-display-id-page :header[id]').once('permalink').each(function() {
    $(this).append('&nbsp;<a href="#' + $(this).attr('id') + '" class="permalink">&para;</a>');
  });
};

})(jQuery);
