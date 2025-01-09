<?php

/**
 * Backdrop inline Diff formatter.
 */
class BackdropDiffInline {
  var $a;
  var $b;

  /**
   * Constructor.
   */
  function __construct($a, $b) {
    $this->a = $a;
    $this->b = $b;
  }

  /**
   * Render differences inline using HTML markup.
   */
  function render() {
    $a = preg_split('/(<[^>]+?>| )/', $this->a, -1, PREG_SPLIT_DELIM_CAPTURE);
    $b = preg_split('/(<[^>]+?>| )/', $this->b, -1, PREG_SPLIT_DELIM_CAPTURE);
    $diff = new Diff($a, $b);
    $diff->edits = $this->process_edits($diff->edits);

    // Assemble highlighted output
    $output = '';
    foreach ($diff->edits as $chunk) {
      switch ($chunk->type) {
        case 'copy':
          $output .= implode('', $chunk->closing);
          break;
        case 'delete':
          foreach ($chunk->orig as $piece) {
            if (strpos($piece, '<') === 0 && backdrop_substr($piece, backdrop_strlen($piece) - 1) === '>') {
              $output .= $piece;
            }
            else {
              $output .= theme('diff_inline_chunk', array('text' => $piece, 'type' => $chunk->type));
            }
          }
          break;
        default:
          $chunk->closing = $this->process_chunk($chunk->closing);
          foreach ($chunk->closing as $piece) {
            if ($piece === ' ' || (strpos($piece, '<') === 0 && backdrop_substr($piece, backdrop_strlen($piece) - 1) === '>' && backdrop_strtolower(backdrop_substr($piece, 1, 3)) != 'img')) {
              $output .= $piece;
            }
            else {
              $output .= theme('diff_inline_chunk', array('text' => $piece, 'type' => $chunk->type));
            }
          }
          break;
      }
    }
    return $output;
  }

  /**
   * Merge chunk segments between tag delimiters.
   */
  function process_chunk($chunk) {
    $processed = array();
    $j = 0;
    foreach ($chunk as $i => $piece) {
      $next = isset($chunk[$i+1]) ? $chunk[$i+1] : NULL;
      if (!isset($processed[$j])) {
        $processed[$j] = '';
      }
      if (strpos($piece, '<') === 0 && backdrop_substr($piece, backdrop_strlen($piece) - 1) === '>') {
        $processed[$j] = $piece;
        $j++;
      }
      elseif (isset($next) && strpos($next, '<') === 0 && backdrop_substr($next, backdrop_strlen($next) - 1) === '>') {
        $processed[$j] .= $piece;
        $j++;
      }
      else {
        $processed[$j] .= $piece;
      }
    }
    return $processed;
  }

  /**
   * Merge copy and equivalent edits into intelligible chunks.
   */
  function process_edits($edits) {
    $processed = array();
    $current = array_shift($edits);

    // Make two passes -- first merge space delimiter copies back into their originals.
    while ($chunk = array_shift($edits)) {
      if ($chunk->type == 'copy' && $chunk->orig === array(' ')) {
        $current->orig = array_merge((array) $current->orig, (array) $chunk->orig);
        $current->closing = array_merge((array) $current->closing, (array) $chunk->closing);
      }
      else {
        $processed[] = $current;
        $current = $chunk;
      }
    }
    $processed[] = $current;

    // Initial setup
    $edits = $processed;
    $processed = array();
    $current = array_shift($edits);

    // Second, merge equivalent chunks into each other.
    while ($chunk = array_shift($edits)) {
      if ($current->type == $chunk->type) {
        $current->orig = array_merge((array) $current->orig, (array) $chunk->orig);
        $current->closing = array_merge((array) $current->closing, (array) $chunk->closing);
      }
      else {
        $processed[] = $current;
        $current = $chunk;
      }
    }
    $processed[] = $current;

    return $processed;
  }
}
