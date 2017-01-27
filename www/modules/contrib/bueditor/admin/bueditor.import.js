(function($) {

//quick import predefined editors.
$(document).ready(function() {
  var imp = Backdrop.settings.BUE.imp, dir = Backdrop.settings.basePath + imp.dir, html = [];
  $.each(imp.editors, function(i, name) {html[i] = '<a href="#" title="'+ name +'">'+ name +'</a>'});
  html = Backdrop.t('You can use these quick-import templates: !list', {'!list': html.join(', ')});
  $(document.createElement('div')).html(html).insertBefore('div.form-item-code div.warning:last').find('a').click(function(e) {
    var name = this.title, file = dir  + name + '.bueditor.txt', $a = $(this);
    $a.html(Backdrop.t('Loading...'));
    $.ajax({
      url: file,
      dataType: 'text',
      error: function(request) {alert(Backdrop.ajaxError(request, file))},
      success: function(code) {code.substr(0, 5) == 'array' && $('#edit-code').val(code).focus()},
      complete: function() {$a.html(name)}
    });
    return false;
  });
});

})(jQuery);