<?php
/**********************************************************************
    
***********************************************************************/
/*
	Print request redirector. This file is fired via print link or 
	print button in reporting module. 
*/
$path_to_root = "..";
global $page_security;
$page_security = 'SA_OPEN';	
include_once($path_to_root . "/includes/session.inc");

if (user_save_report_selections() > 0 && isset($_POST['REP_ID'])) {	
	for($i=0; $i<12; $i++) { 
		if (isset($_POST['PARAM_'.$i]) && !is_array($_POST['PARAM_'.$i])) {
			$rep = $_POST['REP_ID'];
			setcookie("select[$rep][$i]", $_POST['PARAM_'.$i], time()+60*60*24*user_save_report_selections()); 
		}	
	}
}	

if (isset($_GET['xls']) || isset($_GET['xml']))
{
	$filename = $_GET['filename'];
	$unique_name = preg_replace('/[^0-9_a-z.\-]/i', '', $_GET['unique']);
	$path =  company_path(). '/pdf_files/';
	header("Content-type: ". (isset($_GET['xls']) ? "application/vnd.ms-excel" : "text/xml"));
	header("Content-Disposition: attachment; filename=$filename" );
	header("Expires: 0");
	header("Cache-Control: must-revalidate, post-check=0,pre-check=0");
	header("Pragma: public");
	echo file_get_contents($path.$unique_name);
	exit();
}

if (!isset($_POST['REP_ID'])) {	
	$def_pars = array(0, 0, '', '', 0, '', '', 0); //default values
	$rep = $_POST['REP_ID'] = $_GET['REP_ID'];
	for($i=0; $i<8; $i++) {
		$_POST['PARAM_'.$i] = isset($_GET['PARAM_'.$i]) 
			? $_GET['PARAM_'.$i] : $def_pars[$i];
	}
}

$rep = preg_replace('/[^a-z_0-9]/i', '', $_POST['REP_ID']);

$rep_file = find_custom_file("/reporting/rep$rep.php");

if ($rep_file) {
	require($rep_file);
} else
	display_error("Cannot find report file '$rep'");
exit();

