<?php

function backdropapi_preprocess_views_view_table(&$vars) {
  $vars['classes_array'][] = 'table table-striped';
}
