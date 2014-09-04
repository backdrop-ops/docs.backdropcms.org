core = 7.x
api = 2



; Modules

projects[api][type] = module
projects[api][subdir] = contrib
projects[api][version] = 1.5

projects[context][type] = module
projects[context][subdir] = contrib
projects[context][version] = 3.1

projects[ctools][type] = module
projects[ctools][subdir] = contrib
projects[ctools][version] = 1.3

projects[features][type] = module
projects[features][subdir] = contrib
projects[features][version] = 2.0
; Cleaner .info file format - https://drupal.org/comment/8270327#comment-8270327
projects[features][patch][] = https://drupal.org/files/issues/cleaner_info-2155793-1.patch

projects[grammar_parser_lib][type] = module
projects[grammar_parser_lib][subdir] = contrib
projects[grammar_parser_lib][version] = 2.1
; Remove makefile - https://drupal.org/comment/7989875#comment-7989875
projects[grammar_parser_lib][patch][] = http://drupal.org/files/gplib-no_makes-1463770-19.patch

projects[libraries][type] = module
projects[libraries][subdir] = contrib
projects[libraries][version] = 2.1

projects[strongarm][type] = module
projects[strongarm][subdir] = contrib
projects[strongarm][version] = 2.0

projects[views][type] = module
projects[views][subdir] = contrib
projects[views][version] = 3.7
; Fix boolean filter - https://drupal.org/node/2006560
projects[views][patch][] = https://drupal.org/files/handlerfilterbooleanstring.patch



; Libraries

libraries[documentation-6.x][download][type] = git
libraries[documentation-6.x][download][url] = http://git.drupal.org/project/documentation.git
libraries[documentation-6.x][download][branch] = 6.x-1.x
libraries[documentation-6.x][destination] = .sources
libraries[documentation-6.x][api_project_name] = drupal
libraries[documentation-6.x][api_branch] = 6.x

libraries[documentation-7.x][download][type] = git
libraries[documentation-7.x][download][url] = http://git.drupal.org/project/documentation.git
libraries[documentation-7.x][download][branch] = 7.x-1.x
libraries[documentation-7.x][destination] = .sources
libraries[documentation-7.x][api_project_name] = drupal
libraries[documentation-7.x][api_branch] = 7.x

libraries[drupal-6.x][download][type] = git
libraries[drupal-6.x][download][url] = http://git.drupal.org/project/drupal.git
libraries[drupal-6.x][download][branch] = 6.x
libraries[drupal-6.x][destination] = .sources
libraries[drupal-6.x][api_project_name] = drupal

libraries[drupal-7.x][download][type] = git
libraries[drupal-7.x][download][url] = http://git.drupal.org/project/drupal.git
libraries[drupal-7.x][download][branch] = 7.x
libraries[drupal-7.x][destination] = .sources
libraries[drupal-7.x][api_project_name] = drupal
libraries[drupal-7.x][api_project_title] = Drupal
libraries[drupal-7.x][api_project_type] = core
libraries[drupal-7.x][api_preferred] = true

libraries[grammar_parser][download][type] = git
libraries[grammar_parser][download][url] = http://git.drupal.org/project/grammar_parser.git
libraries[grammar_parser][download][revision] = 3292a99e25143430c44a31afbf4ce4582d693947
