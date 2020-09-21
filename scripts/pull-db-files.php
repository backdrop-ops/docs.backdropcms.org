<?php
/**
 * @file
 * Get the latest sanitized database and/or files for local development.
 *
 * Options:
 *   --database (-d): Download the latest database backup.
 *   --files (-f): Download the latest files directory backup.
 *
 * This will only work if you have the Sanitize website credentials setup in
 * your `.env` file. See the .example.env file for details. Make sure to run
 * `lando rebuild -y` after setting up this file.
 *
 * If you don't have access to the Sanitize website, you can request access by
 * opening an issue at
 * https://github.com/backdrop-ops/api.backdropcms.org/issues
 */

$user = getenv('LP_USER');
$pass = getenv('LP_PASS');
$db_url = 'https://sanitize.backdropcms.org/api.backdropcms.org/sanitized/api_backdrop-latest-sanitized.sql.gz';
$files_url = 'https://sanitize.backdropcms.org/api.backdropcms.org/files_backups/api_backdrop-files-latest.tar.gz';

if (empty($user) || empty($pass)) {
  print "\n\t\033[33mWarning\033[0m: You don't seem to have your credentials"
    . " set in your \033[1m.env\033[0m file.\n"
    . "\tSet them up then run: \033[1mlando rebuild -y\033[0m\n"
    . "\tOr file an issue at: https://github.com/backdrop-ops/api.backdropcms.org/issues to request access.\n\n";

  return 0;
}

if (in_array('--database', $argv) || in_array('-d', $argv)) {
  passthru(
    "wget --http-user=$user --http-password=$pass -c $db_url"
  );
}

if (in_array('--files', $argv) || in_array('-f', $argv)) {
  passthru(
    "wget --http-user=$user --http-password=$pass -c $files_url"
  );
}
