<?php
/**
 * @file
 * Definition of views_handler_field_paragraphs_library_link_edit.
 */

/**
 * Field handler to present an edit link to user
 *
 * @ingroup views_field_handlers
 */
class views_handler_field_paragraphs_library_link_edit extends views_handler_field_paragraphs_library_link {

  /**
   * {@inheritdoc}
   */
  public function render_link($data, $values) {
    if ($data && user_access('edit any paragraphs_library_item entities')) {
      $this->options['alter']['make_link'] = TRUE;

      $text = !empty($this->options['text']) ? $this->options['text'] : t('Edit');

      $this->options['alter']['path'] = "admin/content/paragraphs/library/$data/edit";
      $this->options['alter']['query'] = backdrop_get_destination();

      return $text;
    }
  }

}
