<?php
//

//

//



//

//





//

if (!defined ('PDF_TYPE_NULL'))
    define ('PDF_TYPE_NULL', 0);
if (!defined ('PDF_TYPE_NUMERIC'))
    define ('PDF_TYPE_NUMERIC', 1);
if (!defined ('PDF_TYPE_TOKEN'))
    define ('PDF_TYPE_TOKEN', 2);
if (!defined ('PDF_TYPE_HEX'))
    define ('PDF_TYPE_HEX', 3);
if (!defined ('PDF_TYPE_STRING'))
    define ('PDF_TYPE_STRING', 4);
if (!defined ('PDF_TYPE_DICTIONARY'))
    define ('PDF_TYPE_DICTIONARY', 5);
if (!defined ('PDF_TYPE_ARRAY'))
    define ('PDF_TYPE_ARRAY', 6);
if (!defined ('PDF_TYPE_OBJDEC'))
    define ('PDF_TYPE_OBJDEC', 7);
if (!defined ('PDF_TYPE_OBJREF'))
    define ('PDF_TYPE_OBJREF', 8);
if (!defined ('PDF_TYPE_OBJECT'))
    define ('PDF_TYPE_OBJECT', 9);
if (!defined ('PDF_TYPE_STREAM'))
    define ('PDF_TYPE_STREAM', 10);
if (!defined ('PDF_TYPE_BOOLEAN'))
    define ('PDF_TYPE_BOOLEAN', 11);
if (!defined ('PDF_TYPE_REAL'))
    define ('PDF_TYPE_REAL', 12);
    
require_once("pdf_context.php");
require_once("wrapper_functions.php");

class pdf_parser {
	
	/**
     * Filename
     * @var string
     */
    var $filename;
    
    /**
     * File resource
     * @var resource
     */
    var $f;
    
    /**
     * PDF Context
     * @var object pdf_context-Instance
     */
    var $c;
    
    /**
     * xref-Data
     * @var array
     */
    var $xref;

    /**
     * root-Object
     * @var array
     */
    var $root;
	
    /**
     * PDF version of the loaded document
     * @var string
     */
    var $pdfVersion;
    
    /**
     * Constructor
     *
     * @param string $filename  Source-Filename
     */
	function __construct($filename) {
        $this->filename = $filename;
        
        $this->f = @fopen($this->filename, "rb");

        if (!$this->f)
            $this->error(sprintf("Cannot open %s !", $filename));

        $this->getPDFVersion();

        $this->c = new pdf_context($this->f);
        
        $this->pdf_read_xref($this->xref, $this->pdf_find_xref());

        
        $this->getEncryption();

        
        $this->pdf_read_root();
    }
    
    /**
     * Close the opened file
     */
    function closeFile() {
    	if (isset($this->f)) {
    	    fclose($this->f);	
    		unset($this->f);
    	}	
    }
    
    /**
     * Print Error and die
     *
     * @param string $msg  Error-Message
     */
    function error($msg) {
    	die("<b>PDF-Parser Error:</b> ".$msg);	
    }
    
    /**
     * Check Trailer for Encryption
     */
    function getEncryption() {
        if (isset($this->xref['trailer'][1]['/Encrypt'])) {
            $this->error("File is encrypted!");
        }
    }
    
	/**
     * Find/Return /Root
     *
     * @return array
     */
    function pdf_find_root() {
        if ($this->xref['trailer'][1]['/Root'][0] != PDF_TYPE_OBJREF) {
            $this->error("Wrong Type of Root-Element! Must be an indirect reference");
        }
        return $this->xref['trailer'][1]['/Root'];
    }

    /**
     * Read the /Root
     */
    function pdf_read_root() {
        
        $this->root = $this->pdf_resolve_object($this->c, $this->pdf_find_root());
    }
    
    /**
     * Get PDF-Version
     *
     * And reset the PDF Version used in FPDI if needed
     */
    function getPDFVersion() {
        fseek($this->f, 0);
        preg_match("/\d\.\d/",fread($this->f,16),$m);
        if (isset($m[0]))
            $this->pdfVersion = $m[0];
        return $this->pdfVersion;
    }
    
    /**
     * Find the xref-Table
     */
    function pdf_find_xref() {
       	$toRead = 1500;
                
        $stat = fseek ($this->f, -$toRead, SEEK_END);
        if ($stat === -1) {
            fseek ($this->f, 0);
        }
       	$data = fread($this->f, $toRead);
        
        $pos = strlen($data) - strpos(strrev($data), strrev('startxref')); 
        $data = substr($data, $pos);
        
        if (!preg_match('/\s*(\d+).*$/s', $data, $matches)) {
            $this->error("Unable to find pointer to xref table");
    	}

    	return (int) $matches[1];
    }

    /**
     * Read xref-table
     *
     * @param array $result Array of xref-table
     * @param integer $offset of xref-table
     */
    function pdf_read_xref(&$result, $offset) {
        fseek($this->f, $o_pos = $offset-20); 
            
        $data = fread($this->f, 100);
        
        $xrefPos = strpos($data, 'xref');
        
        if ($xrefPos === false) {
            $this->error('Unable to find xref table.');
        }
        
        if (!isset($result['xref_location'])) {
            $result['xref_location'] = $o_pos+$xrefPos;
            $result['max_object'] = 0;
    	}

    	$cylces = -1;
        $bytesPerCycle = 100;
        
    	fseek($this->f, $o_pos = $o_pos+$xrefPos+4); 
        $data = fread($this->f, $bytesPerCycle);
        
        while (($trailerPos = strpos($data, 'trailer', max($bytesPerCycle*$cylces++, 0))) === false && !feof($this->f)) {
            $data .= fread($this->f, $bytesPerCycle);
        }
        
        if ($trailerPos === false) {
            $this->error('Trailer keyword not found after xref table');
        }
        
        $data = substr($data, 0, $trailerPos);
        
        
        preg_match_all("/(\r\n|\n|\r)/", substr($data, 0, 100), $m); 

        $differentLineEndings = count(array_unique($m[0]));
        if ($differentLineEndings > 1) {
            $lines = preg_split("/(\r\n|\n|\r)/", $data, -1, PREG_SPLIT_NO_EMPTY);
        } else {
            $lines = explode($m[0][1], $data);
        }
        
        $data = $differentLineEndings = $m = null;
        unset($data, $differentLineEndings, $m);
        
        $linesCount = count($lines);
        
        $start = 1;
        
        for ($i = 0; $i < $linesCount; $i++) {
            $line = trim($lines[$i]);
            if ($line) {
                $pieces = explode(" ", $line);
                $c = count($pieces);
                switch($c) {
                    case 2:
                        $start = (int)$pieces[0];
                        $end   = $start+(int)$pieces[1];
                        if ($end > $result['max_object'])
                            $result['max_object'] = $end;
                        break;
                    case 3:
                        if (!isset($result['xref'][$start]))
                            $result['xref'][$start] = array();
                        
                        if (!array_key_exists($gen = (int) $pieces[1], $result['xref'][$start])) {
                	        $result['xref'][$start][$gen] = $pieces[2] == 'n' ? (int) $pieces[0] : null;
                	    }
                        $start++;
                        break;
                    default:
                        $this->error('Unexpected data in xref table');
                }
            }
        }
        
        $lines = $pieces = $line = $start = $end = $gen = null;
        unset($lines, $pieces, $line, $start, $end, $gen);
        
        fseek($this->f, $o_pos+$trailerPos+7);
        
        $c = new pdf_context($this->f);
	    $trailer = $this->pdf_read_value($c);
	    
	    $c = null;
	    unset($c);
	    
	    if (!isset($result['trailer'])) {
            $result['trailer'] = $trailer;          
	    }
	    
	    if (isset($trailer[1]['/Prev'])) {
	    	$this->pdf_read_xref($result, $trailer[1]['/Prev'][1]);
	    } 
	    
	    $trailer = null;
	    unset($trailer);
        
        return true;
    }
    
    /**
     * Reads an Value
     *
     * @param object $c pdf_context
     * @param string $token a Token
     * @return mixed
     */
    function pdf_read_value(&$c, $token = null) {
    	if (is_null($token)) {
    	    $token = $this->pdf_read_token($c);
    	}
    	
        if ($token === false) {
    	    return false;
    	}

       	switch ($token) {
            case	'<':
    			
    			

                $pos = $c->offset;

    			while(1) {

                    $match = strpos ($c->buffer, '>', $pos);
				
    				
    				

    				if ($match === false) {
    					if (!$c->increase_length()) {
    						return false;
    					} else {
                        	continue;
                    	}
    				}

    				$result = substr ($c->buffer, $c->offset, $match - $c->offset);
    				$c->offset = $match + 1;
    				
    				return array (PDF_TYPE_HEX, $result);
                }
                
                break;
    		case	'<<':
    			

    			$result = array();

    			
    			
    			while (($key = $this->pdf_read_token($c)) !== '>>') {
    				if ($key === false) {
    					return false;
    				}
					
    				if (($value =   $this->pdf_read_value($c)) === false) {
    					return false;
    				}
                    $result[$key] = $value;
    			}
				
    			return array (PDF_TYPE_DICTIONARY, $result);

    		case	'[':
    			

    			$result = array();

    			
    			
    			while (($token = $this->pdf_read_token($c)) !== ']') {
                    if ($token === false) {
    					return false;
    				}
					
    				if (($value = $this->pdf_read_value($c, $token)) === false) {
                        return false;
    				}
					
    				$result[] = $value;
    			}
    			
                return array (PDF_TYPE_ARRAY, $result);

    		case	'('		:
                
                $pos = $c->offset;
                
                $openBrackets = 1;
    			do {
                    for (; $openBrackets != 0 && $pos < $c->length; $pos++) {
                        switch (ord($c->buffer[$pos])) {
                            case 0x28: 
                                $openBrackets++;
                                break;
                            case 0x29: 
                                $openBrackets--;
                                break;
                            case 0x5C: 
                                $pos++;
                        }
                    }
    			} while($openBrackets != 0 && $c->increase_length());
    			
    			$result = substr($c->buffer, $c->offset, $pos - $c->offset - 1);
    			$c->offset = $pos;
    			
    			return array (PDF_TYPE_STRING, $result);

    			
            case "stream":
            	$o_pos = ftell($c->file)-strlen($c->buffer);
		        $o_offset = $c->offset;
		        
		        $c->reset($startpos = $o_pos + $o_offset);
		        
		        $e = 0; 
		        if ($c->buffer[0] == chr(10) || $c->buffer[0] == chr(13))
		        	$e++;
		        if ($c->buffer[1] == chr(10) && $c->buffer[0] != chr(10))
		        	$e++;
		        
		        if ($this->actual_obj[1][1]['/Length'][0] == PDF_TYPE_OBJREF) {
		        	$tmp_c = new pdf_context($this->f);
		        	$tmp_length = $this->pdf_resolve_object($tmp_c,$this->actual_obj[1][1]['/Length']);
		        	$length = $tmp_length[1][1];
		        } else {
		        	$length = $this->actual_obj[1][1]['/Length'][1];	
		        }
		        
		        if ($length > 0) {
    		        $c->reset($startpos+$e,$length);
    		        $v = $c->buffer;
		        } else {
		            $v = '';   
		        }
		        $c->reset($startpos+$e+$length+9); 
		        
		        return array(PDF_TYPE_STREAM, $v);
		        
	        case '%':
	            
                $pos = $c->offset;
    			while(1) {
    			    
                    #$match = preg_match("/(\r\n|\r|\n)/", $c->buffer, $m, PREG_OFFSET_CAPTURE, $pos);
                    
                    $match = preg_match("/(\r\n|\r|\n)/", substr($c->buffer, $pos), $m);
					if ($match === false) {
    					if (!$c->increase_length()) {
    						return false;
    					} else {
                        	continue;
                    	}
    				}

    				
                    #$c->offset = $m[0][1]+strlen($m[0][0]);
    				
                    $c->offset = strpos($c->buffer, $m[0], $pos)+strlen($m[0]);
    				
    				return $this->pdf_read_value($c);
                }
                
    		default	:
            	if (is_numeric ($token)) {
                    
    				
    				if (($tok2 = $this->pdf_read_token ($c)) !== false) {
                        if (is_numeric ($tok2)) {

    						
    						
    						
    						
    						
    						if (($tok3 = $this->pdf_read_token ($c)) !== false) {
                                switch ($tok3) {
    								case	'obj'	:
                                        return array (PDF_TYPE_OBJDEC, (int) $token, (int) $tok2);
    								case	'R'		:
    									return array (PDF_TYPE_OBJREF, (int) $token, (int) $tok2);
    							}
    							
    							
    							
    							array_push ($c->stack, $tok3);
    						}
    					}

    					array_push ($c->stack, $tok2);
    				}

    				if ($token === (string)((int)$token))
        				return array (PDF_TYPE_NUMERIC, (int)$token);
    				else 
    					return array (PDF_TYPE_REAL, (float)$token);
    			} else if ($token == 'true' || $token == 'false') {
                    return array (PDF_TYPE_BOOLEAN, $token == 'true');
    			} else {

                    
    				return array (PDF_TYPE_TOKEN, $token);
    			}

         }
    }
    
    /**
     * Resolve an object
     *
     * @param object $c pdf_context
     * @param array $obj_spec The object-data
     * @param boolean $encapsulate Must set to true, cause the parsing and fpdi use this method only without this para
     */
    function pdf_resolve_object(&$c, $obj_spec, $encapsulate = true) {
        
    	if (!is_array($obj_spec)) {
            return false;
    	}

    	if ($obj_spec[0] == PDF_TYPE_OBJREF) {

    		
    		if (isset($this->xref['xref'][$obj_spec[1]][$obj_spec[2]])) {

    			
    			
    			
    			
    			

    			$old_pos = ftell($c->file);

    			
    			
				
    			$c->reset($this->xref['xref'][$obj_spec[1]][$obj_spec[2]]);

    			$header = $this->pdf_read_value($c,null,true);

    			if ($header[0] != PDF_TYPE_OBJDEC || $header[1] != $obj_spec[1] || $header[2] != $obj_spec[2]) {
    				$this->error("Unable to find object ({$obj_spec[1]}, {$obj_spec[2]}) at expected location");
    			}

    			
    			
    			
				$this->actual_obj =& $result;
    			if ($encapsulate) {
    				$result = array (
    					PDF_TYPE_OBJECT,
    					'obj' => $obj_spec[1],
    					'gen' => $obj_spec[2]
    				);
    			} else {
    				$result = array();
    			}

    			
    			
    			while(1) {
                    $value = $this->pdf_read_value($c);
					if ($value === false || count($result) > 4) {
						
						break;
    				}

    				if ($value[0] == PDF_TYPE_TOKEN && $value[1] === 'endobj') {
    					break;
    				}

                    $result[] = $value;
    			}

    			$c->reset($old_pos);

                if (isset($result[2][0]) && $result[2][0] == PDF_TYPE_STREAM) {
                    $result[0] = PDF_TYPE_STREAM;
                }

    			return $result;
    		}
    	} else {
    		return $obj_spec;
    	}
    }

    
    
    /**
     * Reads a token from the file
     *
     * @param object $c pdf_context
     * @return mixed
     */
    function pdf_read_token(&$c)
    {
    	
    	
    	

    	if (count($c->stack)) {
    		return array_pop($c->stack);
    	}

    	

    	do {
    		if (!$c->ensure_content()) {
    			return false;
    		}
    		$c->offset += _strspn($c->buffer, " \n\r\t", $c->offset);
    	} while ($c->offset >= $c->length - 1);

    	

    	$char = $c->buffer[$c->offset++];

    	switch ($char) {

    		case '['	:
    		case ']'	:
    		case '('	:
    		case ')'	:

    			
    			

    			return $char;

    		case '<'	:
    		case '>'	:

    			
    			
    			

    			if ($c->buffer[$c->offset] == $char) {
    				if (!$c->ensure_content()) {
    				    return false;
    				}
    				$c->offset++;
    				return $char . $char;
    			} else {
    				return $char;
    			}

    		default		:

    			
    			
    			

    			if (!$c->ensure_content()) {
    				return false;
    			}

    			while(1) {

    				

    				$pos = _strcspn($c->buffer, " []<>()\r\n\t/", $c->offset);
    				if ($c->offset + $pos <= $c->length - 1) {
    					break;
    				} else {
    					
    					
    					
    					
    					

    					$c->increase_length();
    				}
    			}

    			$result = substr($c->buffer, $c->offset - 1, $pos + 1);

    			$c->offset += $pos;
    			return $result;
    	}
    }

	
}