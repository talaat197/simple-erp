<?php
/**********************************************************************
    
***********************************************************************/
include_once($path_to_root . "/includes/db/class.reflines_db.inc");
include_once($path_to_root . "/admin/db/fiscalyears_db.inc");
include_once($path_to_root . "/includes/types.inc");
//---------------------------------------------------------------------------------------------

//	. all transactions now have references stored in trans table.

//	. template based reflines implemented
//

//		- implement refline field in all transaction tables (obsoletes not always accurate find_refline_id)
//		- remove save() and restore_last() - for now preserved for reflines without placeholder
//		- see fixmes below
//		- remove refs table and create view instead (need e.g. CREATE VIEW support in db_import/db_export)

$refline_options = array(
	ST_JOURNAL => array('date', 'user'),
	ST_COSTUPDATE => array('date', 'user'),

	ST_BANKPAYMENT => array('date', 'user'),
	ST_BANKDEPOSIT => array('date', 'user'),
	ST_BANKTRANSFER => array('date', 'user'),
	ST_SUPPAYMENT => array('date', 'user'),
	ST_CUSTPAYMENT => array('date', 'user'),

	ST_SALESORDER => array('date', 'customer', 'branch', 'user', 'pos'),
	ST_SALESQUOTE => array('date', 'customer', 'branch', 'user', 'pos'),
	ST_SALESINVOICE => array('date', 'customer', 'branch', 'user', 'pos'),
	ST_CUSTCREDIT => array('date', 'customer', 'branch', 'user', 'pos'),
	ST_CUSTDELIVERY => array('date', 'customer', 'branch', 'user', 'pos'),

	ST_LOCTRANSFER => array('date', 'location', 'user'),
	ST_INVADJUST => array('date', 'location', 'user'),

	ST_PURCHORDER => array('date', 'location', 'supplier', 'user'),
	ST_SUPPINVOICE => array('date', 'location', 'supplier', 'user'),
	ST_SUPPCREDIT => array('date', 'location', 'supplier', 'user'),
	ST_SUPPRECEIVE => array('date', 'location', 'supplier', 'user'),

	ST_WORKORDER => array('date', 'location', 'user'),
	ST_MANUISSUE => array('date', 'location', 'user'),
	ST_MANURECEIVE => array('date', 'user'),
	ST_DIMENSION => array('date','user'),
);

$refline_placeholders = array(
	'MM' => 'date',
	'YY' => 'date',
	'YYYY' => 'date',
	'FF' => 'date', 
	'FFFF' => 'date',
	'UU' => 'user',
 	'P' => 'pos',
//	 FIXME:  for placeholders below all the code should work, but as the ref length is variable,
//	   length specification in placeholder format should be implemented.
//	'C' => 'customer',
//	'B' => 'branch',
//	'S' => 'supplier',
//	'L' => 'location'
);

class references 
{
	var $reflines;
	
	function __construct()
	{
		$this->reflines = new reflines_db();
	}

	function _legacy_line($refline)
	{
		return strpbrk($refline['pattern'], '{}') == false;
	}

	function _parse_next($type, $template, $context=null)
	{
		global $refline_options, $refline_placeholders;

		
		if (!isset($context))
			$context = new_doc_date();

		if (is_string($context))
			$context = array('date' => $context);

		$context['user'] = $_SESSION['wa_current_user']->user;
		$context['pos'] = $_SESSION['wa_current_user']->pos;
		$out = '';

		while(($start = strpos($template, '{')) !==false) {

			$out .= substr($template, 0, $start);
			$stop = strpos($template, '}');
			if ($stop === false) {
				display_warning(_("Invalid refline template."));
				$out .= $template; 
				break;
			}
			$ph = substr($template, $start+1, $stop-$start-1);
			$template = substr($template, $stop+1);

			if (isset($refline_placeholders[$ph])) {
				if (!isset($context[$refline_placeholders[$ph]]))
				{
					display_warning(sprintf(_("Missing refline context data: '%s'"), $refline_placeholders[$ph]));
					$out .= $ph; 
				} else {
					switch ($ph)
					{
						case 'MM':
						case 'YY':
						case 'YYYY':
							list($day, $month, $year) = explode_date_to_dmy($context['date']);
							$out .= $ph == 'MM' ? sprintf('%02d', $month) : ($ph == 'YY' ? sprintf('%02d', $year%100): sprintf('%04d', $year));
							break;
						case 'FF':
						case 'FFFF':
							list($day, $month, $year) = explode_date_to_dmy(get_fiscalyear_begin_for_date($context['date']));
							$out .= $ph == 'FF' ? sprintf('%02d', $year%100): sprintf('%04d', $year);
							break;
						case 'C':
							$out .= sprintf('%d', $context['customer']);
							break;

						case 'B':
							$out .= sprintf('%d', $context['branch']);
							break;

						case 'S':
							$out .= sprintf('%d', $context['supplier']);
							break;

						case 'L':
							$out .= sprintf('%s', $context['location']);
							break;

						case 'P':
							$out .= sprintf('%s', $context['pos']);
							break;

						case 'UU':
							$out .= sprintf('%02d', $context['user']);
						break;
					}
				}
			} elseif (is_numeric($ph)) {
				$out .= '{'.$ph.'}'; 
			}
		}

		$out .= $template;	
		if (!preg_match('/^([^\{]*)?\{([^\}]*)\}(.*)/', $out, $match)) {	
			display_error(_("Missing numeric placeholder in refline definition."));
			return $out;
		}

		$prefix = $match[1];
		$postfix = $match[3];

		$db_info = get_systype_db_info($type);
		$trans_table = $db_info[0];
		$type_fld = $db_info[1];
		$tno_fld = $db_info[2];
		$ref_fld = $db_info[3];
		$type = db_escape($type);

		
		$sql = "SELECT MAX(CAST(SUBSTR($ref_fld, ".(strlen($prefix)+1).",LENGTH($ref_fld)-".(strlen($postfix)+strlen($prefix)).") AS UNSIGNED))"
				." FROM `$trans_table` tbl
					LEFT JOIN ".TB_PREF."voided v ON tbl.`$tno_fld`=v.id AND v.type=$type"
				." WHERE ISNULL(v.id)"
				.($type_fld ? " AND tbl.`$type_fld`=$type" : '')
				." AND `$ref_fld` REGEXP ".db_escape('^'.preg_quote($prefix).'[0-9]*'.preg_quote($postfix).'$');
		$result = db_query($sql, 'cannot retrieve last reference');
		$result = db_fetch_row($result);

			
		return $prefix.sprintf('%0'.strlen($match[2]).'d', $result[0]+1).$postfix;
	}

	//
	//	Get/check transaction reference.
	
	
	//
	function _get($type, $trans_no=0, $ref=null)
	{
		$db_info = get_systype_db_info($type);
		$trans_table = $db_info[0];
		$type_fld = $db_info[1];
		$tno_fld = $db_info[2];
		$ref_fld = $db_info[3];

		$type = db_escape($type);

		$sql = "SELECT `$ref_fld` 
				FROM `$trans_table` tbl
					LEFT JOIN ".TB_PREF."voided v ON 
				tbl.`$tno_fld`=v.id AND v.type=$type"
			." WHERE ISNULL(v.id)"
			.($type_fld ? " AND tbl.`$type_fld`=$type" : '');
		if ($ref)
		{
			$sql .= " AND tbl.`$ref_fld`=".db_escape(trim($ref));
			if ($trans_no)
				$sql .= " AND tbl.`$tno_fld` != ".db_escape($trans_no);
		} else {
			$sql .= " AND tbl.`$tno_fld`=".db_escape($trans_no);
		}

		$result = db_query($sql, "could not test for unique reference");
		if (!$result)
			return false;

		$result = db_fetch_row($result);
		return $result[0];
	}

	function is_new_reference($ref, $type, $trans_no=0)
	{
		return !$this->_get($type, $trans_no, $ref);
	}

	//
	
	//
	function get_next($type, $line=null, $context=null) 
	{

		if (isset($line))
			$refline = $this->reflines->get($line);
		else {
			$refs = $this->reflines->get_all("trans_type=".db_escape($type)." AND `default`");
			$refline = db_fetch($refs);
		}

		if ($this->_legacy_line($refline))
			return $refline['pattern'];

		return $this->_parse_next($type, $refline['prefix'].$refline['pattern'], $context);
	}

	/**
 	*	Normalize reference to format allowed by refline (optionally selected by prefix).
 	*	FIXME: currently this is fake function which return either input reference or 
 	*	next reference when no line has been recognized.
	**/
	function normalize($ref, $type, $context, $line=null)
	{
		if (!isset($type)) 
			return $ref;

		if (!$line)
			$line = $this->reflines->find_refline_id($ref, $type);

		return $this->is_valid($ref, $type, $context, $line) ? $ref : $this->get_next($type, $line, $context);
	}

	//
	
	
	//
 	function is_valid($reference, $type, $context=null, $line=null)
	{
 		if (!isset($line))
 			$line = $this->reflines->find_refline_id($reference, $type, true);

 		if (!isset($line))
 			return false;

		$refline = $this->reflines->get($line);

		if ($this->_legacy_line($refline))	//legacy non-templated line
			return strlen(trim($reference)) > 0;

		$regex = preg_quote($refline['prefix'].$refline['pattern']);
		if (!is_array($context))
			$context = array('date'=>$context);

 		$context['pos'] = $_SESSION["wa_current_user"]->pos;

		if (is_date(@$context['date']))
		{
			list($year4, $month, $day) = explode("-", date2sql($context['date']));
			$year2 = substr($year4, 2);
			$f_year = explode("-", date2sql(get_fiscalyear_begin_for_date($context['date'])));
			$f_year2 = substr($f_year[0], 2);
			$f_year4 = $f_year[0];
		} else
		{
			$month = '\d{2,}';
			$year2 = '\d{2,}';
			$year4 = '\d{4,}';
			$f_year2 = '\d{2,}';
			$f_year4 = '\d{4,}';
		}
		$cust = @$context['customer'] ? $context['customer'] : '\d+';
		$supp = @$context['supplier'] ? $context['supplier'] : '\d+';
		$branch = @$context['branch'] ? $context['branch'] : '\d+';
		$location = @$context['location'] ? $context['location'] : '[a-z0-9]+';
 		$pos = @$context['pos'] ? $context['pos'] : '\d+';
		$user = sprintf("%02d", $_SESSION['wa_current_user']->user);

		$regex = preg_replace(
			array(
				'/\\\{/',	
				'/\\\}/',
				'/\{MM\}/',
				'/\{YY\}/',
				'/\{YYYY\}/',
				'/\{FF\}/',
				'/\{FFFF\}/',
				'/\{C\}/',
				'/\{B\}/',
				'/\{S\}/',
				'/\{L\}/',
				'/\{UU\}/',
 				'/\{P\}/',
				'/\{\d+}/',
			),
			array(
				'{',
				'}',
				$month,
				$year2,
				$year4,
				$f_year2,
				$f_year4,
				$cust,
				$branch,
				$supp,
				$location,
				$user,
 				$pos,
				'\d+',
			), $regex);

		$regex = '"^'.$regex.'"i';

		return preg_match($regex, $reference, $match) ? 1 : 0;
	}

	//
	//	Save reference (and prepare next) on write transaction.
	//
	function save($type, $id, $reference, $line = null) 
	{
		if ($reference == 'auto')
			return;

	    $sql = "REPLACE ".TB_PREF."refs SET reference=".db_escape($reference)
			.", type=".db_escape($type).", id=".db_escape($id);
    	db_query($sql, "could not update reference entry");

		if (!isset($line))
		{
			$line = $this->reflines->find_refline_id($reference, $type);
		}

		$refline = $this->reflines->get($line);
		 
		if ($this->_legacy_line($refline) && ($reference == $this->get_next($type, $line))) { 
			$next = $this->_increment($reference);	
			$this->reflines->save_next($type, $next, $line);
		}
	}
	//
	
	//
	function restore_last($type, $id)
	{
		
		$reference = get_reference($type, $id);
		$line = $this->reflines->find_refline_id($reference, $type);
		$refline = $this->reflines->get($line);

		if ($this->_legacy_line($refline)) 
		{
			$last = $this->_increment($this->get_next($type, $line), true); 
			if ($reference == $last)
			{
				
			    $sql = "UPDATE ".TB_PREF."reflines SET pattern=SUBSTRING(" . db_escape(trim($last)) .", LENGTH(`prefix`)+1)"
					. " WHERE trans_type = ".db_escape($type) . " AND `id`=".db_escape($line);

				db_query($sql, "The next transaction ref for $type could not be updated");
			}
		}
	}

	//-----------------------------------------------------------------------
	//
	//	Increments (or decrements if $back==true) reference template
	//
	function _increment($reference, $back=false) 
	{
		
		
       	
        
        
        
        //
        if (preg_match('/^(\D*?)(\d+)(.*)/', $reference, $result) == 1) 
        {
			list($all, $prefix, $number, $postfix) = $result;
			$dig_count = strlen($number); 
			$fmt = '%0' . $dig_count . 'd'; 
			$val = intval($number + ($back ? ($number<1 ? 0 : -1) : 1));
			$nextval =  sprintf($fmt, $val); 

			return $prefix.$nextval.$postfix;
        }
        else 
            return $reference;
	}

}

//----------------------------------------------------------------------------
//
//	Check if reference was not used so far (for other transaction than $trans_no)
//
function is_new_reference($ref, $type, $trans_no=0)
{
	global $Refs;

	return $Refs->is_new_reference($ref, $type, $trans_no);
}

function get_reference($type, $trans_no)
{
	global $Refs;

	return $Refs->_get($type, $trans_no);
}
