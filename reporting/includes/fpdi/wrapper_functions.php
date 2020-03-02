<?php
//

//

//



//

//





//


if (!defined("PHP_VER_LOWER43")) 
	define("PHP_VER_LOWER43", version_compare(PHP_VERSION, "4.3", "<"));


/**
 * ensure that strspn works correct if php-version < 4.3
 */
function _strspn($str1, $str2, $start=null, $length=null) {
    $numargs = func_num_args();

    if (PHP_VER_LOWER43 == 1) {
        if (isset($length)) {
            $str1 = substr($str1, $start, $length);
        } else {
            $str1 = substr($str1, $start);
        }
    }

    if ($numargs == 2 || PHP_VER_LOWER43 == 1) {
        return strspn($str1, $str2);
    } else if ($numargs == 3) {
        return strspn($str1, $str2, $start);
    } else {
        return strspn($str1, $str2, $start, $length);
    }
}


/**
 * ensure that strcspn works correct if php-version < 4.3
 */
function _strcspn($str1, $str2, $start=null, $length=null) {
    $numargs = func_num_args();

    if (PHP_VER_LOWER43 == 1) {
        if (isset($length)) {
            $str1 = substr($str1, $start, $length);
        } else {
            $str1 = substr($str1, $start);
        }
    }

    if ($numargs == 2 || PHP_VER_LOWER43 == 1) {
        return strcspn($str1, $str2);
    } else if ($numargs == 3) {
        return strcspn($str1, $str2, $start);
    } else {
        return strcspn($str1, $str2, $start, $length);
    }
}