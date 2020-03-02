<?php
//

//

//



//

//





//

class pdf_context {

	var $file;
	var $buffer;
	var $offset;
	var $length;

	var $stack;

	

	function __construct($f) {
		$this->file = $f;
		$this->reset();
	}

	
	
	

	function reset($pos = null, $l = 100) {
		if (!is_null ($pos)) {
			fseek ($this->file, $pos);
		}

		$this->buffer = $l > 0 ? fread($this->file, $l) : '';
		$this->length = strlen($this->buffer);
		if ($this->length < $l)
            $this->increase_length($l - $this->length);
		$this->offset = 0;
		$this->stack = array();
	}

	
	
	
	
	

	function ensure_content() {
		if ($this->offset >= $this->length - 1) {
			return $this->increase_length();
		} else {
			return true;
		}
	}

	

	function increase_length($l=100) {
		if (feof($this->file)) {
			return false;
		} else {
			$totalLength = $this->length + $l;
		    do {
                $this->buffer .= fread($this->file, $totalLength-$this->length);
            } while ((($this->length = strlen($this->buffer)) != $totalLength) && !feof($this->file));
			
			return true;
		}
	}

}