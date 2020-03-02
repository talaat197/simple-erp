<?php
/**********************************************************************
    
***********************************************************************/





function print_document_link($doc_no, $link_text, $link=true, $type_no, 
	$icon=false, $class='printlink', $id='', $email=0, $extra=0)
{
	global $path_to_root;
	include_once($path_to_root . "/includes/types.inc");

	$url = $path_to_root.'/reporting/prn_redirect.php?';
	$def_orientation = (user_def_print_orientation() == 1 ? 1 : 0);

	switch ($type_no)
	{
		case ST_SALESQUOTE :
			$rep = 111;
			
			$ar = array(
				'PARAM_0' => $doc_no, 
				'PARAM_1' => $doc_no, 
				'PARAM_2' => '', 
				'PARAM_3' => $email, 
				'PARAM_4' => '',
				'PARAM_5' => $def_orientation);
			break;
		case ST_SALESORDER :
			$rep = 109;
			
			$ar = array(
				'PARAM_0' => $doc_no, 
				'PARAM_1' => $doc_no, 
				'PARAM_2' => '', 
				'PARAM_3' => $email, 
				'PARAM_4' => 0, 
				'PARAM_5' => '',
				'PARAM_6' => $def_orientation);
			break;
		case ST_CUSTDELIVERY :
			$rep = 110;
			
			$ar = array(
				'PARAM_0' => $doc_no, 
				'PARAM_1' => $doc_no, 
				'PARAM_2' => $email,
				'PARAM_3' => $extra,
				'PARAM_4' => '',
				'PARAM_5' => $def_orientation);
			break;
		case ST_SALESINVOICE : 
		case ST_CUSTCREDIT : 
			$rep = $type_no==ST_CUSTCREDIT ? 113 : 107;
			
			$ar = array(
				'PARAM_0' => $doc_no, 
				'PARAM_1' => $doc_no, 
				'PARAM_2' => '', 
				'PARAM_3' => $email, 
				'PARAM_4' => '',
				'PARAM_5' => '', 
				'PARAM_6' => $rep == 107 ? '' : $def_orientation);
			if ($rep == 107)
				$ar['PARAM_7'] = $def_orientation;
			break;
		case ST_PURCHORDER :
			$rep = 209;
			
			$ar = array(
				'PARAM_0' => $doc_no, 
				'PARAM_1' => $doc_no, 
				'PARAM_2' => '', 
				'PARAM_3' => $email, 
				'PARAM_4' => '',
				'PARAM_5' => $def_orientation);
			break;
		case ST_CUSTPAYMENT :
			$rep = 112;
			
			$ar = array(
				'PARAM_0' => $doc_no, 
				'PARAM_1' => $doc_no, 
				'PARAM_2' => '', 
				'PARAM_3' => $email,
				'PARAM_4' => '',
				'PARAM_5' => $def_orientation);
			break;
		case ST_SUPPAYMENT :
			$rep = 210;
			
			$ar = array(
				'PARAM_0' => $doc_no, 
				'PARAM_1' => $doc_no, 
				'PARAM_2' => '', 
				'PARAM_3' => $email, 
				'PARAM_4' => '',
				'PARAM_5' => $def_orientation);
			break;
		case ST_WORKORDER :
			$rep = 409;
			
			$ar = array(
				'PARAM_0' => $doc_no, 
				'PARAM_1' => $doc_no, 
				'PARAM_2' => $email, 
				'PARAM_3' => '', 
				'PARAM_4' => $def_orientation);
			break;
		case ST_DIMENSION :
			$rep = 501;
			
			$ar = array(
				'PARAM_0' => $doc_no, 
				'PARAM_1' => $doc_no, 
				'PARAM_2' => 1, 
				'PARAM_3' => '', 
				'PARAM_4' => $def_orientation);
			break;
		default:
			return null;
	}

	return print_link($link_text, $rep, $ar, "", $icon, $class, $id);
}
//
//	Universal link to any kind of report.
//
function print_link($link_text, $rep, $pars = array(), $dir = '', 
	$icon=false, $class='printlink', $id='')
{
	global $path_to_root, $SysPrefs;

	$url = $dir == '' ?  $path_to_root.'/reporting/prn_redirect.php?' : $dir;

	$id = default_focus($id);
	foreach($pars as $par => $val) {
		$pars[$par] = "$par=".urlencode($val);
	}
	$pars[] = 'REP_ID='.urlencode($rep);
	$url .= implode ('&', $pars);

	if ($class != '')
		$class = $SysPrefs->pdf_debug ? '' : " class='$class'";
	if ($id != '')
		$id = " id='$id'";
	$pars = access_string($link_text);
	if (user_graphic_links() && $icon)
		$pars[0] = set_icon($icon, $pars[0]);
	return "<a target='_blank' href='$url'$id$class $pars[1]>$pars[0]</a>";
}

/*




$str = "PPFrt#2000 CID#6378465 TaxEx#2345-038 abcde ertrgdert COD#4253 jdegtd PIN#6473654";
$p = parse_notes_params($str);
echo print_r($p, true);

An example of usage will be in the reporting/rep110.php file at near the end just before the last $rep-Font(); statement: 


    $notes_params = parse_notes_params($branch['notes']);
    if ($packing_slip == 0 && array_key_exists('CID', $notes_params)) {
        $rep->NewLine(1);
        $rep->TextCol(1, 7, "Old Customer# : " . $notes_params['CID'], - 2);
    }
*/

function parse_notes_params($str, $sep=" ", $delim="#") {
    $str_params = explode($sep, $str);
    $param_array=Array('notes' => '');
    foreach ($str_params AS $str_param) {
        $param_set=explode($delim, trim($str_param));
        $key = (array_key_exists(0, $param_set) ? trim($param_set[0]) : '');
        $val = (array_key_exists(1, $param_set) ? trim($param_set[1]) : '');
        if (strlen($key) > 0 && strlen($val) > 0) {
            $param_array[$key]=$val;
        } else {
            
            
            
            $param_array['notes'] .= (" " .  $str_param);
        }
    }
    $param_array['notes'] = trim($param_array['notes']);
    return $param_array;
}

