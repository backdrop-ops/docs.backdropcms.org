(function($, Backdrop) {
    Backdrop.ajax.prototype.commands.paragraphs_modal_admin_message_hide = function(ajax, response, status) {
        $('.paragraphs-modal-admin-message').delay(1000).fadeOut(800);
        //alert(response.value1);
    }
})(jQuery, Backdrop);
