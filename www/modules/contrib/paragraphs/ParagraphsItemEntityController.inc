<?php

/**
 * @file
 * Entity Plus Controller implementation for the paragraphs entity.
 */

class ParagraphsItemEntityController extends EntityPlusController {

  /**
   * {@inheritdoc}
   */
  public function buildContent(EntityInterface $entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    $build = parent::buildContent($entity, $view_mode, $langcode);
    $build['#theme'] = 'paragraphs_item';
    return $build;
  }
}
