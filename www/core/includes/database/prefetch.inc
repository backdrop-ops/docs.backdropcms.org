<?php
/**
 * @file
 * Database interface code for engines that need complete control over their
 * result sets.
 */

/**
 * @addtogroup database
 * @{
 */

/**
 * An implementation of DatabaseStatementInterface that prefetches all data.
 *
 * This class behaves very similar to a PDOStatement but as it always fetches
 * every row it is possible to manipulate those results.
 */
class DatabaseStatementPrefetch implements Iterator, DatabaseStatementInterface {

  /**
   * The query string.
   *
   * @var string
   */
  protected $queryString;

  /**
   * Driver-specific options. Can be used by child classes.
   *
   * @var array
   */
  protected $driverOptions;

  /**
   * Reference to the database connection object for this statement.
   *
   * The name $dbh is inherited from PDOStatement.
   *
   * @var DatabaseConnection
   */
  public $dbh;

  /**
   * Main data store.
   *
   * @var array
   */
  protected $data = array();

  /**
   * The current row, retrieved in PDO::FETCH_ASSOC format.
   *
   * @var array
   */
  protected $currentRow = NULL;

  /**
   * The key of the current row.
   *
   * @var int
   */
  protected $currentKey = NULL;

  /**
   * The list of column names in this result set.
   *
   * @var array
   */
  protected $columnNames = NULL;

  /**
   * The number of rows affected by the last query.
   *
   * @var int
   */
  protected $rowCount = NULL;

  /**
   * The number of rows in this result set.
   *
   * @var int
   */
  protected $resultRowCount = 0;

  /**
   * Holds the current fetch style (which will be used by the next fetch).
   * @see PDOStatement::fetch()
   *
   * @var int
   */
  protected $fetchStyle = PDO::FETCH_OBJ;

  /**
   * Holds supplementary current fetch options (to be used by the next fetch).
   *
   * @var array
   */
  protected $fetchOptions = array(
    'class' => 'stdClass',
    'constructor_args' => array(),
    'object' => NULL,
    'column' => 0,
  );

  /**
   * Holds the default fetch style.
   *
   * @var int
   */
  protected $defaultFetchStyle = PDO::FETCH_OBJ;

  /**
   * Holds supplementary default fetch options.
   *
   * @var array
   */
  protected $defaultFetchOptions = array(
    'class' => 'stdClass',
    'constructor_args' => array(),
    'object' => NULL,
    'column' => 0,
  );

  /**
   * Creates a prefetch database statement object.
   *
   * @param DatabaseConnection $connection
   *   The database connection for this query.
   * @param string $query
   *   The query to be executed.
   * @param array $driver_options
   *   Driver-specific options.
   */
  public function __construct(DatabaseConnection $connection, $query, array $driver_options = array()) {
    $this->dbh = $connection;
    $this->queryString = $query;
    $this->driverOptions = $driver_options;
  }

  /**
   * Executes a prepared statement.
   *
   * @param array $args
   *   An array of values with as many elements as there are bound parameters in
   *   the SQL statement being executed.
   * @param array $options
   *   An array of options for this query.
   * @return bool
   *   TRUE on success, or FALSE on failure.
   */
  public function execute($args = array(), $options = array()) { // phpcs:ignore -- No type hints, add in 2.x.
    if (isset($options['fetch'])) {
      if (is_string($options['fetch'])) {
        // Default to an object. Note: db fields will be added to the object
        // before the constructor is run. If you need to assign fields after
        // the constructor is run, see http://drupal.org/node/315092.
        $this->setFetchMode(PDO::FETCH_CLASS, $options['fetch']);
      }
      else {
        $this->setFetchMode($options['fetch']);
      }
    }

    $logger = $this->dbh->getLogger();
    if (!empty($logger)) {
      $query_start = microtime(TRUE);
    }

    // Prepare the query.
    $statement = $this->getStatement($this->queryString, $args);
    if (!$statement) {
      $this->throwPDOException();
    }

    $return = $statement->execute($args);
    if (!$return) {
      $this->throwPDOException();
    }

    // Fetch all the data from the reply, in order to release any lock
    // as soon as possible.
    $this->rowCount = $statement->rowCount();
    $this->data = $statement->fetchAll(PDO::FETCH_ASSOC);
    // Destroy the statement as soon as possible.
    unset($statement);

    $this->resultRowCount = count($this->data);

    if ($this->resultRowCount) {
      $this->columnNames = array_keys($this->data[0]);
    }
    else {
      $this->columnNames = array();
    }

    if (!empty($logger)) {
      $query_end = microtime(TRUE);
      $logger->log($this, $args, $query_end - $query_start);
    }

    // Initialize the first row in $this->currentRow.
    $this->next();

    return $return;
  }

  /**
   * Throw a PDO Exception based on the last PDO error.
   */
  protected function throwPDOException() {
    $error_info = $this->dbh->errorInfo();
    // We rebuild a message formatted in the same way as PDO.
    $exception = new PDOException("SQLSTATE[" . $error_info[0] . "]: General error " . $error_info[1] . ": " . $error_info[2]);
    $exception->errorInfo = $error_info;
    throw $exception;
  }

  /**
   * Grab a PDOStatement object from a given query and its arguments.
   *
   * Some drivers will need to perform some preparation themselves to get the
   * statement right.
   *
   * @param string $query
   *   The query.
   * @param array $args
   *   An array of arguments.
   * @return PDOStatement
   *   A PDOStatement object.
   */
  protected function getStatement($query, &$args = array()) { // phpcs:ignore -- No type hint on $args, add in 2.x.
    return $this->dbh->prepare($query);
  }

  /**
   * Return the object's SQL query string.
   */
  public function getQueryString() {
    return $this->queryString;
  }

  /**
   * @see PDOStatement::setFetchMode()
   */
  public function setFetchMode($fetchStyle, $a2 = NULL, $a3 = NULL) {
    $this->defaultFetchStyle = $fetchStyle;
    switch ($fetchStyle) {
      case PDO::FETCH_CLASS:
        $this->defaultFetchOptions['class'] = $a2;
        if ($a3) {
          $this->defaultFetchOptions['constructor_args'] = $a3;
        }
        break;
      case PDO::FETCH_COLUMN:
        $this->defaultFetchOptions['column'] = $a2;
        break;
      case PDO::FETCH_INTO:
        $this->defaultFetchOptions['object'] = $a2;
        break;
    }

    // Set the values for the next fetch.
    $this->fetchStyle = $this->defaultFetchStyle;
    $this->fetchOptions = $this->defaultFetchOptions;
  }

  /**
   * Return the current row formatted according to the current fetch style.
   *
   * This is the core method of this class. It grabs the value at the current
   * array position in $this->data and format it according to $this->fetchStyle
   * and $this->fetchMode.
   *
   * @return mixed
   *   The current row formatted as requested.
   */
  #[\ReturnTypeWillChange]
  public function current() {
    if (isset($this->currentRow)) {
      switch ($this->fetchStyle) {
        case PDO::FETCH_ASSOC:
          return $this->currentRow;
        case PDO::FETCH_BOTH:
          // PDO::FETCH_BOTH returns an array indexed by both the column name
          // and the column number.
          return $this->currentRow + array_values($this->currentRow);
        case PDO::FETCH_NUM:
          return array_values($this->currentRow);
        case PDO::FETCH_LAZY:
          // We do not do lazy as everything is fetched already. Fallback to
          // PDO::FETCH_OBJ.
        case PDO::FETCH_OBJ:
          return (object) $this->currentRow;
        case PDO::FETCH_CLASS:
          if (!isset($class_name)) {
            $class_name = $this->fetchOptions['class'];
          }
          if (count($this->fetchOptions['constructor_args'])) {
            // Verify the current db connection to avoid this code being called
            // in an inappropriate context.
            $db_connection_options = Database::getConnection()->getConnectionOptions();
            $valid_db_drivers = array('sqlite', 'oracle');
            if (!in_array($db_connection_options['driver'], $valid_db_drivers)) {
              throw new BadMethodCallException();
            }
            $reflector = new ReflectionClass($class_name);
            $result = $reflector->newInstanceArgs($this->fetchOptions['constructor_args']);
          }
          else {
            $result = new $class_name();
          }
          foreach ($this->currentRow as $k => $v) {
            $result->$k = $v;
          }
          return $result;
        case PDO::FETCH_INTO:
          foreach ($this->currentRow as $k => $v) {
            $this->fetchOptions['object']->$k = $v;
          }
          return $this->fetchOptions['object'];
        case PDO::FETCH_COLUMN:
          if (isset($this->columnNames[$this->fetchOptions['column']])) {
            return $this->currentRow[$this->columnNames[$this->fetchOptions['column']]];
          }
          else {
            return NULL;
          }
      }
    }
    return NULL;
  }

  /* Implementations of Iterator. */

  /**
   * {@inheritdoc}
   */
  #[\ReturnTypeWillChange]
  public function key() {
    return $this->currentKey;
  }

  /**
   * {@inheritdoc}
   */
  #[\ReturnTypeWillChange]
  public function rewind() {
    // Nothing to do: our DatabaseStatement can't be rewound.
  }

  /**
   * {@inheritdoc}
   */
  #[\ReturnTypeWillChange]
  public function next() {
    if (!empty($this->data)) {
      $this->currentRow = reset($this->data);
      $this->currentKey = key($this->data);
      unset($this->data[$this->currentKey]);
    }
    else {
      $this->currentRow = NULL;
    }
  }

  /**
   * {@inheritdoc}
   */
  #[\ReturnTypeWillChange]
  public function valid() {
    return isset($this->currentRow);
  }

  /* Implementations of DatabaseStatementInterface. */

  /**
   * {@inheritdoc}
   */
  public function rowCount() {
    return $this->rowCount;
  }

  /**
   * {@inheritdoc}
   */
  public function fetch($fetch_style = NULL, $cursor_orientation = PDO::FETCH_ORI_NEXT, $cursor_offset = NULL) {
    if (isset($this->currentRow)) {
      // Set the fetch parameter.
      $this->fetchStyle = isset($fetch_style) ? $fetch_style : $this->defaultFetchStyle;
      $this->fetchOptions = $this->defaultFetchOptions;

      // Grab the row in the format specified above.
      $return = $this->current();
      // Advance the cursor.
      $this->next();

      // Reset the fetch parameters to the value stored using setFetchMode().
      $this->fetchStyle = $this->defaultFetchStyle;
      $this->fetchOptions = $this->defaultFetchOptions;
      return $return;
    }
    else {
      return FALSE;
    }
  }

  /**
   * {@inheritdoc}
   */
  public function fetchColumn($index = 0) {
    if (isset($this->currentRow) && isset($this->columnNames[$index])) {
      // We grab the value directly from $this->data, and format it.
      $return = $this->currentRow[$this->columnNames[$index]];
      $this->next();
      return $return;
    }
    else {
      return FALSE;
    }
  }

  /**
   * {@inheritdoc}
   */
  public function fetchField($index = 0) {
    return $this->fetchColumn($index);
  }

  /**
   * {@inheritdoc}
   */
  public function fetchObject($class_name = NULL, $constructor_args = array()) {
    if (isset($this->currentRow)) {
      if (!isset($class_name)) {
        // Directly cast to an object to avoid a function call.
        $result = (object) $this->currentRow;
      }
      else {
        $this->fetchStyle = PDO::FETCH_CLASS;
        $this->fetchOptions = array('constructor_args' => $constructor_args);
        // Grab the row in the format specified above.
        $result = $this->current();
        // Reset the fetch parameters to the value stored using setFetchMode().
        $this->fetchStyle = $this->defaultFetchStyle;
        $this->fetchOptions = $this->defaultFetchOptions;
      }

      $this->next();

      return $result;
    }
    else {
      return FALSE;
    }
  }

  /**
   * {@inheritdoc}
   */
  public function fetchAssoc() {
    if (isset($this->currentRow)) {
      $result = $this->currentRow;
      $this->next();
      return $result;
    }
    else {
      return FALSE;
    }
  }

  /**
   * {@inheritdoc}
   */
  public function fetchAll($fetch_style = NULL, $fetch_column = NULL, $constructor_args = array()) {
    $this->fetchStyle = isset($fetch_style) ? $fetch_style : $this->defaultFetchStyle;
    $this->fetchOptions = $this->defaultFetchOptions;
    if (isset($fetch_column)) {
      $this->fetchOptions['column'] = $fetch_column;
    }
    if (isset($constructor_args)) {
      $this->fetchOptions['constructor_args'] = $constructor_args;
    }

    $result = array();
    // Traverse the array as PHP would have done.
    while (isset($this->currentRow)) {
      // Grab the row in the format specified above.
      $result[] = $this->current();
      $this->next();
    }

    // Reset the fetch parameters to the value stored using setFetchMode().
    $this->fetchStyle = $this->defaultFetchStyle;
    $this->fetchOptions = $this->defaultFetchOptions;
    return $result;
  }

  /**
   * {@inheritdoc}
   */
  public function fetchCol($index = 0) {
    if (isset($this->columnNames[$index])) {
      $column = $this->columnNames[$index];
      $result = array();
      // Traverse the array as PHP would have done.
      while (isset($this->currentRow)) {
        $result[] = $this->currentRow[$column];
        $this->next();
      }
      return $result;
    }
    else {
      return array();
    }
  }

  /**
   * {@inheritdoc}
   */
  public function fetchAllKeyed($key_index = 0, $value_index = 1) {
    if (!isset($this->columnNames[$key_index]) || !isset($this->columnNames[$value_index]))
      return array();

    $key = $this->columnNames[$key_index];
    $value = $this->columnNames[$value_index];

    $result = array();
    // Traverse the array as PHP would have done.
    while (isset($this->currentRow)) {
      $result[$this->currentRow[$key]] = $this->currentRow[$value];
      $this->next();
    }
    return $result;
  }

  /**
   * {@inheritdoc}
   */
  public function fetchAllAssoc($key, $fetch_style = NULL) {
    $this->fetchStyle = isset($fetch_style) ? $fetch_style : $this->defaultFetchStyle;
    $this->fetchOptions = $this->defaultFetchOptions;

    $result = array();
    // Traverse the array as PHP would have done.
    while (isset($this->currentRow)) {
      // Grab the row in its raw PDO::FETCH_ASSOC format.
      $row = $this->currentRow;
      // Grab the row in the format specified above.
      $result_row = $this->current();
      $result[$row[$key]] = $result_row;
      $this->next();
    }

    // Reset the fetch parameters to the value stored using setFetchMode().
    $this->fetchStyle = $this->defaultFetchStyle;
    $this->fetchOptions = $this->defaultFetchOptions;
    return $result;
  }

}

/**
 * @} End of "addtogroup database".
 */

