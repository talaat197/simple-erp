<?php

/* List of installed additional extensions. If extensions are added to the list manually
	make sure they have unique and so far never used extension_ids as a keys,
	and $next_extension_id is also updated. More about format of this file yo will find in 
	FA extension system documentation.
*/

$next_extension_id = 20; 

$installed_extensions = array (
  1 => 
  array (
    'name' => 'Theme Response',
    'package' => 'response',
    'version' => '2.4.0-4',
    'type' => 'theme',
    'active' => false,
    'path' => 'themes/response',
  ),
  4 => 
  array (
    'name' => 'FrontHrm',
    'package' => 'fronthrm',
    'version' => '2.4.0-1',
    'type' => 'extension',
    'active' => false,
    'path' => 'modules/FrontHrm',
  ),
  5 => 
  array (
    'package' => 'FrontHrm',
    'name' => 'FrontHrm',
    'version' => '-',
    'available' => '',
    'type' => 'extension',
    'path' => 'modules/FrontHrm',
    'active' => false,
  ),
);
