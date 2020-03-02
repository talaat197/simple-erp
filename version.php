<?php
//==========================================================================================
//

//


$db_version = "2.4.1";
$src_version = "";


$version = isset($SysPrefs->version) ? $SysPrefs->version : $src_version;

//======================================================================

//


$repo_auth = isset($SysPrefs->repo_auth) ? $SysPrefs->repo_auth :
array(
	 'login' => 'anonymous',
	 'pass' => 'password',
	 'host' => 'repo.AgroPhos.eu', 
	 'branch' => '2.4'	
);
