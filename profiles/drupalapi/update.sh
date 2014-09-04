#!/bin/bash

basedir=`pwd`

# Determine the update mode.
git=true
for dir in .sources/*
do
  if ! [ -d "$basedir/$dir/.git" ]
  then
    git=false
    break
  fi
done

# Update libraries.
if [ $git == true ]
then
  echo -e "# Rebuilding DrupalAPI reference sources via GIT"
  for dir in .sources/*
  do
    cd $basedir/$dir
    echo -en "$dir       \r"
    git pull > /dev/null
  done
  cd $basedir
else
  echo -e "# Rebuilding DrupalAPI reference sources via Drush make"
  echo -en "drush make -q --yes --working-copy --no-gitinfofile --no-core --contrib-destination=. ./.sources/sources.make\r"
  drush make -q --yes --working-copy --no-gitinfofile --no-core --contrib-destination=. ./.sources/sources.make 2> /dev/null
fi

# Re-index DrupalAPI reference sources.
for dir in ../../sites/*
do
  if [ -f "$basedir/$dir/settings.php" ]
  then
    cd $basedir/$dir
    install_profile=`drush vget install_profile`
    if echo "$install_profile" | grep -q drupalapi
    then
      if drush | grep -q queue-run
      then
        # Re-index DrupalAPI reference sources with Drush queue runner.
        echo -e "# Updating DrupalAPI reference"
        count=`drush queue-list | grep 'api_parse' | sed -e 's/[^0-9]//g'`
        if [ $count -eq 0 ]
        then
          echo -en "Initializing update\r"
          drush cron -q 2> /dev/null
          count=`drush queue-list | grep 'api_parse' | sed -e 's/[^0-9]//g'`
        fi
        while [ $count -gt 0 ]
        do
          echo -en "$count items remaining   \r"
          drush queue-run api_parse -q 2> /dev/null
          count=`drush queue-list | grep 'api_parse' | sed -e 's/[^0-9]//g'`
        done
      else
        # Flag site to be re-indexed if Drush queue runner is unavailable.
        echo -e "# Setting site to re-index on next access"
        drush vset drupalapi_indexed 0 --always-set -q 2> /dev/null
      fi
    fi
  fi
done
cd $basedir
