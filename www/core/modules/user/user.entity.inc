<?php
/**
 * @file
 * Entity classes and controllers for User module.
 */

/**
 * Defines the user entity class.
 */
class User extends Entity implements UserInterface {

  /**
   * The user ID.
   *
   * @var integer
   */
  public $uid;

  /**
   * The unique user name.
   *
   * @var string
   */
  public $name = '';

  /**
   * The user's password (hashed).
   *
   * @var string
   */
  public $pass;

  /**
   * The user's email address.
   *
   * @var string
   */
  public $mail = '';

  /**
   * The user's signature.
   *
   * @var string
   */
  public $signature;

  /**
   * The user's signature format.
   *
   * @var string
   */
  public $signature_format;

  /**
   * The timestamp when the user was created.
   *
   * @var integer
   */
  public $created;

  /**
   * The timestamp when the user was changed.
   *
   * @var integer
   *
   * @since 1.25.0 Property added.
   */
  public $changed;

  /**
   * The timestamp when the user last accessed the site. A value of 0 means the
   * user has never accessed the site.
   *
   * @var integer
   */
  public $access = 0;

  /**
   * The timestamp when the user last logged in. A value of 0 means the user has
   * never logged in.
   *
   * @var integer
   */
  public $login = 0;

  /**
   * Whether the user is active (1) or blocked (0).
   *
   * @var integer
   */
  public $status = 1;

  /**
   * The user's timezone.
   *
   * @var string
   */
  public $timezone;

  /**
   * The user's langcode.
   *
   * @var string
   */
  public $langcode = LANGUAGE_NONE;

  /**
   * The user's preferred langcode for receiving emails and viewing the site.
   *
   * @var string
   */
  public $preferred_langcode = LANGUAGE_NONE;

  /**
   * The file entity for the user's picture.
   *
   * Early in loading a user object, the picture property will contain the file
   * ID only. It is expanded to be the full file entity in User::attachLoad().
   *
   * @var File|int
   */
  public $picture;

  /**
   * The email address used for initial account creation.
   *
   * @var string
   */
  public $init = '';

  /**
   * An unindexed list of role names.
   *
   * @var array
   */
  public $roles = array();

  /**
   * Implements EntityInterface::id().
   */
  public function id() {
    return isset($this->uid) ? $this->uid : NULL;
  }

  /**
   * Implements EntityInterface::entityType().
   */
  public function entityType() {
    return 'user';
  }

  /**
   * Implements EntityInterface::label().
   */
  public function label() {
    return user_format_name($this);
  }

  /**
   * Implements EntityInterface::uri().
   */
  public function uri() {
    // Anonymous users do not have a URI.
    $uri = FALSE;
    if ($this->uid) {
      $uri = array(
        'path' => 'user/' . $this->uid,
        'options' => array(),
      );
    }
    return $uri;
  }

  /**
   * Overrides Entity::createAccess().
   */
  public static function createAccess($bundle = NULL, $account = NULL) {
    // If no user object is supplied, the access check is for the current user.
    if (empty($account)) {
      $account = $GLOBALS['user'];
    }

    return user_access('administer users', $account);
  }

  /**
   * Overrides Entity::access().
   *
   * @param string $op
   *   The operation to be performed on the user. Possible values are:
   *   - create
   *   - view
   *   - update
   *   - delete
   * @param User $account
   *   (optional) The user to check for. Leave it to NULL to check for the
   *   global user.
   *
   * @return bool
   *   TRUE if access is granted, FALSE otherwise.
   */
  public function access($op, $account = NULL) {
    // Casting class with private property causes errors due to added prefix.
    // e.g. "\0" . 'User' . "\0". So use static array instead.
    $rights = &backdrop_static(__METHOD__, array());

    // We can't always count on Backdrop to provide an AnonymousUser for
    // anonymous users, so test explicitly.
    if (!$this->uid) {
      return FALSE;
    }

    if ($op == 'create') {
      return self::createAccess(NULL, $account);
    }
    elseif (!in_array($op, array('view', 'update', 'delete'), TRUE)) {
      // If the $op was not one of the supported ones, we return access denied.
      return FALSE;
    }
    // If no user object is supplied, the access check is for the current user.
    if (empty($account)) {
      $account = $GLOBALS['user'];
    }

    $cid = $this->id();

    // If we've already checked access for this node, user and op, return from
    // cache.
    if (isset($rights[$account->uid][$cid][$op])) {
      return $rights[$account->uid][$cid][$op];
    }

    if ($op == 'view') {
      // Admins can view all, users can view own profiles at all times.
      if ($account->uid == $this->uid || user_access('administer users', $account)) {
        $rights[$account->uid][$cid][$op] = TRUE;
        return $rights[$account->uid][$cid][$op];
      }
      elseif (user_access('access user profiles', $account)) {
        $rights[$account->uid][$cid][$op] = $account->status;
        return $rights[$account->uid][$cid][$op];
      }
      $rights[$account->uid][$cid][$op] = FALSE;
      return $rights[$account->uid][$cid][$op];
    }
    elseif ($op == 'update') {
      $rights[$account->uid][$cid][$op] = (($account->uid == $this->uid) || user_access('administer users', $account)) && $this->uid > 0;
      return $rights[$account->uid][$cid][$op];
    }
    elseif ($op == 'delete') {
      $rights[$account->uid][$cid][$op] = ((($account->uid == $this->uid) && user_access('cancel account', $account)) || user_access('administer users', $account)) && $this->uid > 0;
      return $rights[$account->uid][$cid][$op];
    }

    $rights[$account->uid][$cid][$op] = FALSE;
    return $rights[$account->uid][$cid][$op];
  }

  /**
   * Implements EntityInterface::save().
   */
  public function save() {
    // Since pictures are not yet fields Save the picture object, if it is set.
    $picture = empty($this->picture) ? NULL : $this->picture;
    // Save the user.
    $return = parent::save();
    // Restore the picture object.
    $this->picture = $picture;

    return $return;
  }

}


/**
 * Controller class for users.
 *
 * This extends the EntityDatabaseStorageController class, adding required
 * special handling for user objects.
 */
class UserStorageController extends EntityDatabaseStorageController {

  /**
   * Overrides EntityDatabaseStorageController::attachLoad().
   *
   * @param User[] $queried_users
   *   The user entities for which fields should be attached.
   */
  function attachLoad(&$queried_users, $revision_id = FALSE) {
    // Build an array of user picture IDs so that these can be fetched later.
    $picture_fids = array();
    foreach ($queried_users as $key => $record) {
      if ($record->picture) {
        $picture_fids[] = $record->picture;
      }
      $queried_users[$key]->data = unserialize((string) $record->data);
      $queried_users[$key]->roles = array();
      if ($record->uid) {
        $queried_users[$record->uid]->roles[] = BACKDROP_AUTHENTICATED_ROLE;
      }
      else {
        $queried_users[$record->uid]->roles[] = BACKDROP_ANONYMOUS_ROLE;
      }
    }

    // Add any additional roles from the database.
    $result = db_query('SELECT role, uid FROM {users_roles} WHERE uid IN (:uids)', array(':uids' => array_keys($queried_users)));
    foreach ($result as $record) {
      $queried_users[$record->uid]->roles[] = $record->role;
    }

    // Add the full file objects for user pictures if enabled.
    if (!empty($picture_fids) && config_get('system.core', 'user_pictures')) {
      $pictures = file_load_multiple($picture_fids);
      foreach ($queried_users as $entity) {
        if (!empty($entity->picture) && isset($pictures[$entity->picture])) {
          $entity->picture = $pictures[$entity->picture];
        }
      }
    }
    // Call the default attachLoad() method. This will add fields and call
    // hook_user_load().
    parent::attachLoad($queried_users, $revision_id);
  }

  /**
   * Overrides EntityDatabaseStorageController::create().
   */
  public function create(array $values) {
    if (!isset($values['created'])) {
      $values['created'] = REQUEST_TIME;
    }
    $values['changed'] = REQUEST_TIME;

    // Users always have the authenticated user role.
    $values['roles'][] = BACKDROP_AUTHENTICATED_ROLE;

    return parent::create($values);
  }

  /**
   * Overrides EntityDatabaseStorageController::save().
   *
   * @param User $entity
   *   The user entity to be saved.
   */
  public function save(EntityInterface $entity) {
    if (empty($entity->uid)) {
      $entity->uid = db_next_id(db_query('SELECT MAX(uid) FROM {users}')->fetchField());
      $entity->is_new = TRUE;
    }
    return parent::save($entity);
  }

  /**
   * Overrides EntityDatabaseStorageController::preSave().
   *
   * @param User $entity
   *   The user entity about to be saved.
   */
  protected function preSave(EntityInterface $entity) {
    // Save current time as last changed time.
    $entity->changed = REQUEST_TIME;

    // Update the user password if it has changed.
    $new_pass = isset($entity->pass) && strlen(trim($entity->pass)) > 0 ? trim($entity->pass) : NULL;
    if ($entity->isNew() || !is_null($new_pass) && $new_pass !== $entity->original->pass) {
      // Allow alternate password hashing schemes.
      require_once BACKDROP_ROOT . '/' . settings_get('password_inc', 'core/includes/password.inc');
      $entity->pass = user_hash_password($new_pass);
      // Abort if the hashing failed and returned FALSE.
      if (!$entity->pass) {
        throw new EntityMalformedException('The entity does not have a password.');
      }
    }

    if (valid_email_address($entity->name)) {
      // Check to see if matching is required.
      $user_email_match = config_get('system.core', 'user_email_match');
      if ($user_email_match && ($entity->name != $entity->mail)) {
        throw new EntityMalformedException('The username and email are both addresses that do not match.');
      }
    }

    if (!empty($entity->picture_upload)) {
      $entity->picture = $entity->picture_upload;
    }
    // Delete the picture if the submission indicates that it should be deleted
    // and no replacement was submitted.
    elseif (!empty($entity->picture_delete)) {
      $entity->picture = NULL;
      file_usage_delete($entity->original->picture, 'user', 'user', $entity->uid);
      file_delete($entity->original->picture->fid);
    }

    if (!$entity->isNew()) {
      // Process picture uploads.
      if (!empty($entity->picture->fid) && (!isset($entity->original->picture->fid) || $entity->picture->fid != $entity->original->picture->fid)) {
        $picture = $entity->picture;
        // If the picture is a temporary file, move it to its final location
        // and make it permanent.
        if (!$picture->status) {
          $info = image_get_info($picture->uri);
          $picture_directory =  file_default_scheme() . '://' . config_get('system.core', 'user_picture_path');

          // Prepare the pictures directory.
          file_prepare_directory($picture_directory, FILE_CREATE_DIRECTORY);
          $destination = file_stream_wrapper_uri_normalize($picture_directory . '/picture-' . $entity->uid . '-' . REQUEST_TIME . '.' . $info['extension']);

          // Move the temporary file into the final location.
          if ($picture = file_move($picture, $destination, FILE_EXISTS_RENAME)) {
            $entity->picture = $picture;
            file_usage_add($picture, 'user', 'user', $entity->uid);
          }
        }
        // Delete the previous picture if it was deleted or replaced.
        if (!empty($entity->original->picture->fid)) {
          file_usage_delete($entity->original->picture, 'user', 'user', $entity->uid);
          file_delete($entity->original->picture->fid);
        }
      }
      $entity->picture = empty($entity->picture->fid) ? NULL : $entity->picture->fid;

      // If the password is empty, that means it was not changed, so use the
      // original password.
      if (empty($entity->pass)) {
        $entity->pass = $entity->original->pass;
      }
    }

    // Prepare user roles.
    if (isset($entity->roles)) {
      $entity->roles = array_values(array_filter($entity->roles));
    }

    // Move account cancellation information into $entity->data.
    foreach (array('user_cancel_method', 'user_cancel_notify') as $key) {
      if (isset($entity->{$key})) {
        if (!$entity->data) {
          $entity->data = array();
        }
        $entity->data[$key] = $entity->{$key};
      }
    }
  }

  /**
   * Overrides EntityDatabaseStorageController::postSave().
   *
   * @param User $entity
   *   The user entity that has just been saved.
   */
  protected function postSave(EntityInterface $entity, $update) {
    if ($update) {
      // If the password has been changed, delete all open sessions for the
      // user and recreate the current one.
      if ($entity->pass != $entity->original->pass) {
        backdrop_session_destroy_uid($entity->uid);
        if ($entity->uid == $GLOBALS['user']->uid) {
          backdrop_session_regenerate();
        }
      }

      // Remove roles that are no longer enabled for the user.
      $entity->roles = array_values(array_filter($entity->roles));

      // Reload user roles if provided.
      if ($entity->roles != $entity->original->roles) {
        db_delete('users_roles')
          ->condition('uid', $entity->uid)
          ->execute();

        $query = db_insert('users_roles')->fields(array('uid', 'role'));
        foreach ($entity->roles as $role_name) {
          if (!in_array($role_name, array(BACKDROP_ANONYMOUS_ROLE, BACKDROP_AUTHENTICATED_ROLE))) {
            $query->values(array(
              'uid' => $entity->uid,
              'role' => $role_name,
            ));
          }
        }
        $query->execute();
      }

      // If the user was blocked, delete the user's sessions to force a logout.
      if ($entity->original->status != $entity->status && $entity->status == 0) {
        backdrop_session_destroy_uid($entity->uid);
      }

      // Send emails after we have the new user object.
      if ($entity->status != $entity->original->status) {
        // The user's status is changing; conditionally send notification email.
        $op = $entity->status == 1 ? 'status_activated' : 'status_blocked';
        _user_mail_notify($op, $entity);
      }
    }
    else {
      // Save user roles. Skip built-in roles, and ones that were already saved
      // to the database during hook calls.
      $role_names_to_skip = array_merge(array(BACKDROP_ANONYMOUS_ROLE, BACKDROP_AUTHENTICATED_ROLE), db_query('SELECT role FROM {users_roles} WHERE uid = :uid', array(':uid' => $entity->uid))->fetchCol());
      if ($roles_to_save = array_diff($entity->roles, $role_names_to_skip)) {
        $query = db_insert('users_roles')->fields(array('uid', 'role'));
        foreach ($roles_to_save as $role_name) {
          $query->values(array(
            'uid' => $entity->uid,
            'role' => $role_name,
          ));
        }
        $query->execute();
      }
    }
  }

  /**
   * Implements EntityControllerInterface::buildContent().
   */
  public function buildContent(EntityInterface $account, $view_mode = 'full', $langcode = NULL) {
    global $language_content;
    $langcode = $langcode ? $langcode : $language_content->langcode;

    // Remove previously built content, if exists.
    $account->content = array();

    // Allow modules to change the display mode.
    $view_mode = key(entity_view_mode_prepare('user', array($account->uid => $account), $view_mode, $langcode));

    // Build fields content.
    field_attach_prepare_view('user', array($account->uid => $account), $view_mode, $langcode);
    entity_prepare_view('user', array($account->uid => $account), $langcode);
    $account->content += field_attach_view('user', $account, $view_mode, $langcode);

    // Populate $account->content with a render() array.
    module_invoke_all('user_view', $account, $view_mode, $langcode);
    module_invoke_all('entity_view', $account, 'user', $view_mode, $langcode);

    // Make sure the current display mode is stored if no module has already
    // populated the related key.
    $account->content += array('#view_mode' => $view_mode);
  }

  /**
   * Overrides DefaultEntityController::view().
   */
  public function view($accounts, $view_mode = 'full', $langcode = NULL, $page = NULL) {
    global $language_content;
    $langcode = $langcode ? $langcode : $language_content->langcode;

    $view = array();
    foreach ($accounts as $account) {
      /** @var User $account */
      // Retrieve all profile fields and attach to $account->content.
      $this->buildContent($account, $view_mode, $langcode);

      $build = $account->content;
      // We don't need duplicate rendering info in account->content.
      unset($account->content);

      $build += array(
        '#theme' => 'user_profile',
        '#account' => $account,
        '#view_mode' => $view_mode,
        '#language' => $langcode,
        '#page' => $page,
      );

      // Allow modules to modify the structured user.
      $type = 'user';
      backdrop_alter(array('user_view', 'entity_view'), $build, $type);
      $view[$type][$account->id()] = $build;
    }

    return $view;
  }
}
