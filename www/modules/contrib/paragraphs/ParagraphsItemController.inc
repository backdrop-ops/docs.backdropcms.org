<?php
/**
 * @file
 * Entity controller for the paragraphs entity.
 */

/**
 * Class ParagraphsItemController
 */
class ParagraphsItemController extends EntityDatabaseStorageController  {

  /**
   * Implements EntityStorageControllerInterface::save().
   */
  public function save(EntityInterface $entity) {
    $transaction = db_transaction();
    try {
      // Load the stored entity, if any.
      if (!$entity->isNew() && !isset($entity->original)) {
        $entity->original = entity_load_unchanged($this->entityType, $entity->id());
      }

      $this->preSave($entity);
      $this->invokeHook('presave', $entity);

      if (!$entity->isNew()) {
        $return = backdrop_write_record($this->entityInfo['base table'], $entity, $this->idKey);
        $this->resetCache(array($entity->{$this->idKey}));
        $this->postSave($entity, TRUE);
        $this->invokeHook('update', $entity);
      }
      else {
        $return = backdrop_write_record($this->entityInfo['base table'], $entity);
        // Reset general caches, but keep caches specific to certain entities.
        $this->resetCache(array());
        $this->postSave($entity, FALSE);
        $this->invokeHook('insert', $entity);
      }

      if ($this->revisionKey) {
        $this->saveRevision($entity);
      }


      // Ignore slave server temporarily.
      db_ignore_slave();
      unset($entity->is_new);
      unset($entity->original);

      return $return;
    }
    catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception($this->entityType, $e);
      throw new EntityStorageException($e->getMessage(), (int) $e->getCode(), $e);
    }
  }

  /**
   * Saves a new revision.
   *
   * @param ParagraphsItem $entity
   *   The paragraphs item entity whose revision should be saved.
   */
  protected function saveRevision(EntityInterface $entity) {
    $record = clone $entity;

    if (empty($entity->{$this->revisionKey}) || !empty($entity->revision)) {
      backdrop_write_record($this->revisionTable, $record);
      db_update($this->entityInfo['base table'])
        ->fields(array($this->revisionKey => $record->{$this->revisionKey}))
        ->condition($this->idKey, $entity->{$this->idKey})
        ->execute();
    }
    else {
      backdrop_write_record($this->revisionTable, $record, $this->revisionKey);
    }
    // Make sure to update the new revision key for the entity.
    $entity->{$this->revisionKey} = $record->{$this->revisionKey};
  }

  /**
   * Deletes a ParagraphItem revision.
   */
  public function deleteRevision($revision_id) {
    $info = entity_get_info($this->entityType);
    $entity_revisions = entity_load_multiple($this->entityType, FALSE, array($info['entity keys']['revision'] => $revision_id));

    if (!empty($entity_revisions)) {
      $entity_revision = reset($entity_revisions);
      // Prevent deleting the default revision.
      if (paragraphs_entity_revision_is_default($this->entityType, $entity_revision)) {
        return FALSE;
      }

      db_delete($this->revisionTable)
        ->condition($this->revisionKey, $revision_id)
        ->execute();

      $this->invoke('revision_delete', $entity_revision);
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Builds a ParagraphItem display.
   */
  public function view($entities, $view_mode = 'full', $langcode = NULL, $page = NULL) {
    $entities_keyed = array();
    foreach ($entities as $entity) {
      $key = isset($entity->{$this->idKey}) ? $entity->{$this->idKey} : NULL;
      $entities_keyed[$key] = $entity;
    }
    $entities = $entities_keyed;
    field_attach_prepare_view($this->entityType, $entities, $view_mode);
    entity_prepare_view($this->entityType, $entities);
    $langcode = isset($langcode) ? $langcode : $GLOBALS['language_content']->langcode;

    $view = array();
    foreach ($entities as $entity) {
      $build = $this->buildContent($entity, $view_mode, $langcode);
      $build += array(
        //'#theme' => 'entity',
        '#entity_type' => $this->entityType,
        '#entity' => $entity,
        '#view_mode' => $view_mode,
        '#language' => $langcode,
        '#page' => $page,
      );
      // Allow modules to modify the structured entity.
      backdrop_alter(array($this->entityType . '_view', 'entity_view'), $build, $this->entityType);
      $key = isset($entity->{$this->idKey}) ? $entity->{$this->idKey} : NULL;
      $view[$this->entityType][$key] = $build;
    }
    return $view;
  }

  /**
   * Implements EMWControllerInterface.
   *
   * @param $content
   *   Optionally. Allows pre-populating the built content to ease overridding
   *   this method.
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    // Remove previously built content, if exists.
    $entity->content = $content;
    $langcode = isset($langcode) ? $langcode : $GLOBALS['language_content']->langcode;

    // Allow modules to change the view mode.
    $context = array(
      'entity_type' => $this->entityType,
      'entity' => $entity,
      'langcode' => $langcode,
    );
    backdrop_alter('entity_view_mode', $view_mode, $context);
    // Make sure the used view-mode gets stored.
    $entity->content += array('#view_mode' => $view_mode);

    // By default add in properties for all defined extra fields.
    // if ($extra_field_controller = entity_get_extra_fields_controller($this->entityType)) {
      // $wrapper = entity_metadata_wrapper($this->entityType, $entity);
      // $extra = $extra_field_controller->fieldExtraFields();
      // $type_extra = &$extra[$this->entityType][$this->entityType]['display'];
      // $bundle_extra = &$extra[$this->entityType][$wrapper->getBundle()]['display'];

      // foreach ($wrapper as $name => $property) {
        // if (isset($type_extra[$name]) || isset($bundle_extra[$name])) {
          // $this->renderEntityProperty($wrapper, $name, $property, $view_mode, $langcode, $entity->content);
        // }
      // }
    // }

    // Add in fields.
    if (!empty($this->entityInfo['fieldable'])) {
      // Perform the preparation tasks if they have not been performed yet.
      // An internal flag prevents the operation from running twice.
      $key = isset($entity->{$this->idKey}) ? $entity->{$this->idKey} : NULL;
      field_attach_prepare_view($this->entityType, array($key => $entity), $view_mode);
      $entity->content += field_attach_view($this->entityType, $entity, $view_mode, $langcode);
    }
    // Invoke hook_ENTITY_view() to allow modules to add their additions.
    if (module_exists('rules')) {
      rules_invoke_all($this->entityType . '_view', $entity, $view_mode, $langcode);
    }
    else {
      module_invoke_all($this->entityType . '_view', $entity, $view_mode, $langcode);
    }
    module_invoke_all('entity_view', $entity, $this->entityType, $view_mode, $langcode);
    $build = $entity->content;
    unset($entity->content);
    return $build;
  }

  
}

