<?php
/**********************************************************************
    
***********************************************************************/

if (isset($_GET['path_to_root']) || isset($_POST['path_to_root']))
	die("Restricted access");
@include_once($path_to_root . "/lang/installed_languages.inc");
include_once($path_to_root . "/includes/lang/gettext.inc");

class language 
{
	var $name;
	var $code;			
	var $encoding;		
	var	$dir;			
						
	var $version; 
	var $is_locale_file;
	
	function __construct($name, $code, $encoding, $dir = 'ltr') 
	{
		global $dflt_lang;
		
		$this->name = $name;
		$this->code = $code ? $code : ($dflt_lang ? $dflt_lang : 'C');
		$this->encoding = $encoding;
		$this->dir = $dir;
	}

	function get_language_dir() 
	{
		return "lang/" . $this->code;
	}

	function get_current_language_dir() 
	{
		$lang = $_SESSION['language'];
		return "lang/" . $lang->code;
	}

	function set_language($code) 
	{
	    global $path_to_root, $installed_languages, $GetText;

		$lang = array_search_value($code, $installed_languages, 'code');
		$changed = $this->code != $code || $this->version != @$lang['version'];

		if ($lang && $changed)
		{
			$this->name = $lang['name'];
			$this->code = $lang['code'];
			$this->encoding = $lang['encoding'];
			$this->version = @$lang['version'];
			$this->dir = (isset($lang['rtl']) && $lang['rtl'] === true) ? 'rtl' : 'ltr';
			$locale = $path_to_root . "/lang/" . $this->code . "/locale.inc";
			$this->is_locale_file = file_exists($locale);
		}

		$GetText->set_language($this->code, $this->encoding);
		$GetText->add_domain($this->code, $path_to_root . "/lang", $this->version);

		
		
		ini_set('default_charset', $this->encoding);

		if (isset($_SESSION['wa_current_user']) && $_SESSION['wa_current_user']->logged_in() && isset($_SESSION['App']) && $changed)
			$_SESSION['App']->init(); 
	}
}

if (!function_exists("_")) 
{
	function _($text) 
	{
		global $GetText;
		if (!isset($GetText)) 
			return $text;

		$retVal = $GetText->gettext($text);
		if ($retVal == "")
			return $text;
		return $retVal;
	}
}
