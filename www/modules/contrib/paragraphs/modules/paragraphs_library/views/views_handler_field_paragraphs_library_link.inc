<?php
/**
 * @file
 * Definition of views_handler_field_paragraphs_library_link.
 */

/**
 * Field handler to present a link to the user.
 *
 * @ingroup views_field_handlers
 */
class views_handler_field_paragraphs_library_link extends views_handler_field {

  /**
   * {@inheritdoc}
   */
  public function construct() {
    parent::construct();
    $this->additional_fields['item_id'] = 'item_id';
  }

  /**
   * {@inheritdoc}
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['text'] = [
      'default' => '',
      'translatable' => TRUE,
    ];
    $options['path_only'] = [
      'default' => FALSE,
      'translatable' => FALSE,
    ];
    return $options;
  }

  /**
   * {@inheritdoc}
   */
  public function options_form(&$form, &$form_state) {
    $form['path_only'] = [
      '#type' => 'checkbox',
      '#title' => t('Path only'),
      '#default_value' => !empty($this->options['path_only']) ? $this->options['path_only'] : FALSE,
      '#description' => t('If checked, the path will be returned without being made into a link.'),
    ];
    $form['text'] = [
      '#type' => 'textfield',
      '#title' => t('Text to display'),
      '#default_value' => $this->options['text'],
      '#states' => [
        'visible' => [
          ':input[name="options[path_only]"]' => ['checked' => FALSE],
        ],
      ],
    ];
    parent::options_form($form, $form_state);
  }

  /**
   * An example of field level access control.
   */
  public function access() {
    return user_access('view paragraphs_library_item entities');
  }

  /**
   * {@inheritdoc}
   */
  public function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  /**
   * {@inheritdoc}
   */
  public function render($values) {
    $value = $this->get_value($values, 'item_id');
    return $this->render_link($this->sanitize_value($value), $values);
  }

  /**
   * Helper to render the link
   */
  public function render_link($data, $values) {
    $path_only = !empty($this->options['path_only']) ? $this->options['path_only'] : FALSE;
    $this->options['alter']['make_link'] = !$path_only;

    if ($path_only) {
      return "admin/content/paragraphs/library/" . $data;
    }
    else {
      $text = !empty($this->options['text']) ? $this->options['text'] : t('View');
      $this->options['alter']['path'] = "admin/content/paragraphs/library/" . $data;

      return $text;
    }
  }

}
