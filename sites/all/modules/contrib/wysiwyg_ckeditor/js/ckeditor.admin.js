(function ($, Drupal) {

"use strict";

Drupal.ckeditor = Drupal.ckeditor || {};

Drupal.behaviors.ckeditorAdmin = {
  attach: function (context, settings) {
    var $context = $(context);
    $(context).find('.ckeditor-toolbar-configuration').once(function() {
      var $wrapper = $(this);
      var $textareaWrapper = $(this).find('.form-item-editor-settings-toolbar').hide();
      var $textarea = $textareaWrapper.find('textarea');
      var $toolbarAdmin = $(settings.ckeditor.toolbarAdmin);
      var sortableSettings = {
        connectWith: '.ckeditor-buttons',
        placeholder: 'ckeditor-button-placeholder',
        forcePlaceholderSize: true,
        tolerance: 'pointer',
        cursor: 'move',
        stop: adminToolbarValue,
      };
      $toolbarAdmin.insertAfter($textareaWrapper).find('.ckeditor-buttons').sortable(sortableSettings);
      $toolbarAdmin.find('.ckeditor-multiple-buttons li').draggable({
        connectToSortable: '.ckeditor-toolbar-active .ckeditor-buttons',
        helper: 'clone',
      });
      $toolbarAdmin.bind('click.ckeditorAddRow', function(event) {
        if ($(event.target).is('a.ckeditor-row-add')) {
          adminToolbarAddRow.apply(event.target, [event]);
        }
      });
      $toolbarAdmin.bind('click.ckeditorAddRow', function(event) {
        if ($(event.target).is('a.ckeditor-row-remove')) {
          adminToolbarRemoveRow.apply(event.target, [event]);
        }
      });
      if ($toolbarAdmin.find('.ckeditor-toolbar-active ul').length > 1) {
        $toolbarAdmin.find('a.ckeditor-row-remove').hide();
      }

      /**
       * Add a new row of buttons.
       */
      function adminToolbarAddRow(event) {
        var $rows = $(this).closest('.ckeditor-toolbar-active').find('.ckeditor-buttons');
        $rows.last().clone().empty().insertAfter($rows.last()).sortable(sortableSettings);
        $(this).siblings('a').show();
        redrawToolbarGradient();
        event.preventDefault();
      }

      /**
       * Remove a row of buttons.
       */
      function adminToolbarRemoveRow(event) {
        var $rows = $(this).closest('.ckeditor-toolbar-active').find('.ckeditor-buttons');
        if ($rows.length === 2) {
          $(this).hide();
        }
        if ($rows.length > 1) {
          var $lastRow = $rows.last();
          var $disabledButtons = $wrapper.find('.ckeditor-toolbar-disabled .ckeditor-buttons');
          $lastRow.children(':not(.ckeditor-multiple-button)').prependTo($disabledButtons);
          $lastRow.sortable('destroy').remove();
          redrawToolbarGradient();
          adminToolbarValue();
        }
        event.preventDefault();
      }

      /**
       * Browser quirk work-around to redraw CSS3 gradients.
       */
      function redrawToolbarGradient() {
        $wrapper.find('.ckeditor-toolbar-active').css('position', 'relative');
        window.setTimeout(function() {
          $wrapper.find('.ckeditor-toolbar-active').css('position', '');
        }, 10);
      }

      /**
       * jQuery Sortable stop event. Save updated toolbar positions to the textarea.
       */
      function adminToolbarValue(event, ui) {
        // Update the toolbar config after updating a sortable.
        var toolbarConfig = [];
        $wrapper.find('.ckeditor-toolbar-active ul').each(function() {
          var $rowButtons = $(this).find('li');
          var rowConfig = [];
          if ($rowButtons.length) {
            $rowButtons.each(function() {
              rowConfig.push(this.getAttribute('data-button-name'));
            });
            toolbarConfig.push(rowConfig);
          }
        });
        $textarea.val(JSON.stringify(toolbarConfig, null, '  '));
      }

    });
  },
  detach: function (context, settings) {
    
  }
};

})(jQuery, Drupal);
