<?php
/**********************************************************************
    
***********************************************************************/
require_once($path_to_root. "/includes/JsHttpRequest.php");

class Ajax extends JsHttpRequest {

	var $aCommands = array();
	var $triggers = array();
	
    function __construct() 
    {
	  parent::__construct(@$_SESSION['language']->encoding);
    }
	//
	//	This function is used in ctrl routines to activate
	//	update of ajaxified html element selected by given name/id.
	//
	function activate($trigname) {
	  if (in_ajax()) {
		$this->triggers[$trigname] = true;
	  }
	}
	//
	//	Javascript clientside redirection.
	//	This is the last command added to reponse (if any).
	//
	function redirect($url) {
	    if(in_ajax()) {
			$this->_addCommand(true, array('n'=>'rd'), absolute_url($url));
		  	$this->run();
	    }
	}
	//
	
	//
	function popup($url) {
		  $this->_addCommand(true, array('n'=>'pu'), absolute_url($url));
	}
	//
	
	//
	function addScript($trigger, $sJS)
	{
		$this->_addCommand($trigger, array('n'=>'js'),$sJS);
		return $this;
	}
	//
	
	//
	function addAssign($trigger, $sTarget,$sAttribute,$sData)
	{
		$this->_addCommand($trigger, array('n'=>'as','t'=>$sTarget,'p'=>$sAttribute),$sData);
		return $this;
	}
	//
	
	//
	function addUpdate($trigger, $sTarget, $sData)
	{
		$this->_addCommand($trigger, array('n'=>'up','t'=>$sTarget),$sData);
		return $this;
	}
	//
	
	//
	function addDisable($trigger, $sTarget, $sData=true)
	{
		$this->_addCommand($trigger, array('n'=>'di','t'=>$sTarget),$sData);
		return $this;
	}
	//
	
	//
	function addEnable($trigger, $sTarget, $sData=true)
	{
		$this->_addCommand($trigger, array('n'=>'di','t'=>$sTarget), !$sData);
		return $this;
	}
	//
	
	//
	function addFocus($trigger, $sTarget)
	{
		$this->_addCommand($trigger, array('n'=>'fc'),$sTarget);
		return $this;
	}
	//
	
	//
	function _addCommand($trigger, $aAttributes, $mData)
	{
	  	if ($this->isActive() && ($trigger !== false)) {

			$aAttributes['why'] = $trigger;
			$aAttributes['data'] = $mData;
			$this->aCommands[] = $aAttributes;
	  	}
	}

	function run() 
	{
	    if (!$this->isActive()) return;

		
		foreach ($this->aCommands as $idx => $com) {


		  	if ($com['why'] !== true && !isset($this->triggers[$com['why']])) {
				unset($this->aCommands[$idx]);
			}
			elseif($com['n'] == 'up' && $com['t'] == '_page_body') {
			  	$cmds = array($com);
			  	foreach( $this->aCommands as $k=> $cmd) {
				  	if ($cmd['n'] == 'fc' || $cmd['n'] == 'js') {	
						$cmds[] = $cmd; //break;
			  		}
				}
				$this->aCommands = $cmds;
				break;
			}
		}
	    $GLOBALS['_RESULT'] = $this->aCommands;
	}
}

function in_ajax() {
    global $Ajax;
    return $Ajax->isActive();
}



//
function absolute_url($url) 
{
   return strpos($url, '..')===0 ? dirname($_SERVER['PHP_SELF']).'/'.$url : $url;
}
