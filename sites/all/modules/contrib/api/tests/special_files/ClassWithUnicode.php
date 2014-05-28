<?php

/**
 * @file
 * Tests that Unicode characters can be dealt with.
 */

/**
 * Class with a Unicode 2-byte character in its description, for testing.
 *
 * This was excerpted from the CacheArray class, when it had a typo in the
 * documentation, with this line, which caused the API module parse to crash:
 * the PHP limitation, see the note in the official PHP documentation atá
 *
 * What we want to happen is that the "do not display this" line gets displayed
 * for the whole file code and for the class description, but the methods and
 * class should still be visible.
 */
abstract class CacheArray implements ArrayAccess {

  /**
   * A cid to pass to cache()->set() and cache()->get().
   */
  protected $cid;

  /**
   * Flags an offset value to be written to the persistent cache.
   *
   * If a value is assigned to a cache object with offsetSet(), by default it
   * will not be written to the persistent cache unless it is flagged with this
   * method. This allows items to be cached for the duration of a request,
   * without necessarily writing back to the persistent cache at the end.
   *
   * @param $offset
   *   The array offset that was request.
   * @param $persist
   *   Optional boolean to specify whether the offset should be persisted or
   *   not, defaults to TRUE. When called with $persist = FALSE the offset will
   *   be unflagged so that it will not written at the end of the request.
   */
  protected function persist($offset, $persist = TRUE) {
    $this->keysToPersist[$offset] = $persist;
  }
}
