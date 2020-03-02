<?php
/**********************************************************************
    
***********************************************************************/
$messages = array(); 
$before_box = ''; 

function get_backtrace($html = false, $skip=0)
{
	$str = '';
	if ($html) $str .= '<table border=0>';
	$trace = debug_backtrace();

	foreach($trace as $trn => $tr) {
		if ($trn <= $skip) continue;
		if ($html) $str .= '<tr><td>';
		if (isset($tr['file']) && isset($tr['line']))
			$str .= $tr['file'].':'.$tr['line'].': ';
		if ($html) $str .= '</td><td>';
		if (isset($tr['type'])) {
	 		if($tr['type'] == '::') {
				$str .= $tr['class'].'::';
			} else if($tr['type'] == '->') {
				$str .= '('.$tr['class'].' Object)'.'->';
			}
		}
		$str .= $tr['function'].'(';
		
		if(isset($tr['args']) && is_array($tr['args'])) {
			$args = array();
			foreach($tr['args'] as $n=>$a) {
				if (is_object($tr['args'][$n]))
					$args[$n] = "(".get_class($tr['args'][$n])." Object)";
				elseif (is_array($tr['args'][$n]))
					$args[$n] = "(Array[".count($tr['args'][$n])."])";
				else
					$args[$n] = "'".$tr['args'][$n]."'";
			}
			$str .= implode(',',$args);
		}
		$str .= ')</td>';
	}

	if ($html) $str .= '</tr></table>';
	return $str;
}

//-----------------------------------------------------------------------------



function error_handler($errno, $errstr, $file, $line) {
    global $messages, $SysPrefs;

	
	
	$excluded_warnings = array(
		'html_entity_decode', 				
		'should be compatible with that',	
		'mysql extension is deprecated'		
	);
	foreach($excluded_warnings as $ref) {
		if (strpos($errstr, $ref) !== false) {
			return true;
		}
	}

	$bt = isset($SysPrefs) && $SysPrefs->go_debug>1 ? get_backtrace(true, 1) : array();

	
	if ($errno & error_reporting()) {
		
		if (!in_array(array($errno, $errstr, $file, $line, @$bt), $messages))
			$messages[] = array($errno, $errstr, $file, $line, @$bt);
	}
	else if ($errno&~E_NOTICE && $errstr != '') { 
		$user = @$_SESSION["wa_current_user"]->loginname;
		$context = isset($SysPrefs) && !$SysPrefs->db_ok ? '[before upgrade]' : '';
		error_log(user_company() . ":$user:". basename($file) .":$line:$context $errstr");
	}
	
    return true;
}

function exception_handler($exception)
{
	error_handler(E_ERROR, sprintf(_("Unhandled exception [%s]: %s."), $exception->getCode(), $exception->getMessage()),
		 $exception->getFile(), $exception->getLine());
	end_page();
}
//------------------------------------------------------------------------------
//	Formats system messages before insert them into message <div>

function fmt_errors($center=false) {
    global $messages, $path_to_root, $SysPrefs;

  	$msg_class = array(
  		E_USER_ERROR => 'err_msg',
  		E_USER_WARNING =>'warn_msg', 
		E_USER_NOTICE => 'note_msg');

  	$type = E_USER_NOTICE;
  	$content = '';

  	if (count($messages)) {
		foreach($messages as $cnt=>$msg) {
			if ($SysPrefs->go_debug && $msg[0]>E_USER_NOTICE)
		 		$msg[0] = E_ERROR;

			if ($msg[0]>$type) continue;

			if ($msg[0]<$type) { 
				if ($msg[0] == E_USER_WARNING) {
					$type = E_USER_WARNING; 
					$content = ''; 			
				} else  {
					$type = E_USER_ERROR; 	
					if($type == E_USER_WARNING)
						$content = ''; 			
				}
			}

	    	$str = $msg[1];
			if (!in_array($msg[0], array(E_USER_NOTICE, E_USER_ERROR, E_USER_WARNING)) && $msg[2] != null)
		  		$str .= ' '._('in file').': '.$msg[2].' '._('at line ').$msg[3];

			if ($SysPrefs->go_debug>1 && $type!=E_USER_NOTICE && $type!=E_USER_WARNING)
		  		$str .= '<br>'.$msg[4];
			$content .= ($cnt ? '<hr>' : '').$str;
		}
		$class = $msg_class[$type];
    	$content = "<div class='$class'>$content</div>";
  	} elseif ($path_to_root=='.')
		return '';
  	return $content;
}
//-----------------------------------------------------------------------------

//
function error_box() {
    global $before_box;
    
    echo "<div id='msgbox'>";

	
	$before_box = ob_get_clean(); 
    ob_start('output_html');
    echo "</div>";
}
/*
	Helper to avoid sparse log notices.
*/
function end_flush() {
	global $Ajax;

	if (isset($Ajax))
		$Ajax->run();

 	 
 	 
 	while(ob_get_level() > 1)
 		ob_end_flush();
	@ob_end_flush();

	
	cancel_transaction();
}

function display_db_error($msg, $sql_statement=null, $exit=true)
{
	global $db, $SysPrefs, $db_connections;

	$warning = $msg==null;
	$db_error = db_error_no();
	
	if($warning)
		$str = "<b>" . _("Debug mode database warning:") . "</b><br>";
	else
		$str = "<b>" . _("DATABASE ERROR :") . "</b> $msg<br>";
	
	if ($db_error != 0) 
	{
		$str .= "error code : " . $db_error . "<br>";
		$str .= "error message : " . db_error_msg($db) . "<br>";
	}
	
	if ($SysPrefs->debug == 1) 
	{
		$cur_prefix = $db_connections[$_SESSION["wa_current_user"]->cur_con]['tbpref'];

		$str .= "sql that failed was : ".str_replace(TB_PREF, $cur_prefix, $sql_statement)."<br>";
	}
	
	$str .= "<br><br>";
	if (!$SysPrefs->go_debug)
		error_log($str);
	else {
		if($msg)
			trigger_error($str, E_USER_ERROR);
		else	
			trigger_error($str, E_USER_WARNING);
	}
	if ($exit)
		exit;
}

function frindly_db_error($db_error)
{
	if ($db_error == DB_DUPLICATE_ERROR) 
	{
		display_error(_("The entered information is a duplicate. Please go back and enter different values."));
		return true;
	}
	
	return false;
}

function check_db_error($msg, $sql_statement, $exit_if_error=true, $rollback_if_error=true)
{
	global $db, $SysPrefs;
	$db_error = db_error_no();
	
	if ($db_error != 0) 
	{
		
		if ($SysPrefs->go_debug || !frindly_db_error($db_error)) {
			display_db_error($msg, $sql_statement, false);
		}
		
		if ($rollback_if_error) 
		{
		  	$rollback_result = db_query("rollback");
		}
		
		if ($exit_if_error) 
		{
			end_page(); exit;
		}
	}
	return $db_error;
}

