(function ($, Backdrop) {
    Backdrop.ajax.prototype.commands.paragraphs_modal_admin_message_hide = function (ajax, response, status) {
        $('.paragraphs-modal-admin-message').delay(1000).fadeOut(800, function () {
            $(this).remove();
        });
    }

    Backdrop.ajax.prototype.commands.paragraphs_modal_admin_refresh = function (ajax, response, status) {
        $(response.selector).addClass('paragraphs-item-stale');
        $('.paragraphs-item-stale').removeClass(response.selector);
        $('.paragraphs-item-stale').after(response.new_element);
        $(response.selector).addClass('paragraphs-item-modal-admin');
        $(response.selector).prepend($('.paragraphs-item-stale > .dropbutton-wrapper'));
        $('.paragraphs-item-stale').remove();
    }
})(jQuery, Backdrop);
