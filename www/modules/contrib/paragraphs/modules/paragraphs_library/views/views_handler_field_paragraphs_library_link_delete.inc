<?php
/**
 * @file
 * Definition of views_handler_field_paragraphs_library_link_delete.
 */

/**
 * Field handler to present a delete link
 *
 * @ingroup views_field_handlers
 */
class views_handler_field_paragraphs_library_link_delete extends views_handler_field_paragraphs_library_link {

  /**
   * Renders the link.
   */
  public function render_link($data, $values) {
    // Ensure user has access to delete this node.
    if (!user_access('edit any paragraphs_library_item entities')) {
      return;
    }

    $this->options['alter']['make_link'] = TRUE;
    $this->options['alter']['path'] = "admin/content/paragraphs/library/$data/delete";
    $this->options['alter']['query'] = backdrop_get_destination();

    $text = !empty($this->options['text']) ? $this->options['text'] : t('Delete');
    return $text;
  }
}
