<?php
//

//

//



//

//





//

class LZWDecode {

    var $sTable = array();
    var $data = null;
    var $tIdx;
    var $bitsToGet = 9;
    var $bytePointer;
    var $bitPointer;
    var $nextData = 0;
    var $nextBits = 0;
    var $andTable = array(511, 1023, 2047, 4095);

    function __construct(&$fpdi) {
        $this->fpdi =& $fpdi;
    }

    /**
     * Method to decode LZW compressed data.
     *
     * @param string data    The compressed data.
     */
    function decode($data) {

        if($data[0] == 0x00 && $data[1] == 0x01) {
            $this->fpdi->error("LZW flavour not supported.");
        }

        $this->initsTable();

        $this->data = $data;

        
        $this->bytePointer = 0;
        $this->bitPointer = 0;

        $this->nextData = 0;
        $this->nextBits = 0;

        $oldCode = 0;

        $string = "";
        $uncompData = "";

        while (($code = $this->getNextCode()) != 257) {
            if ($code == 256) {
                $this->initsTable();
                $code = $this->getNextCode();

                if ($code == 257) {
                    break;
                }

                $uncompData .= $this->sTable[$code];
                $oldCode = $code;

            } else {

                if ($code < $this->tIdx) {
                    $string = $this->sTable[$code];
                    $uncompData .= $string;

                    $this->addStringToTable($this->sTable[$oldCode], $string[0]);
                    $oldCode = $code;
                } else {
                    $string = $this->sTable[$oldCode];
                    $string = $string.$string[0];
                    $uncompData .= $string;

                    $this->addStringToTable($string);
                    $oldCode = $code;
                }
            }
        }
        
        return $uncompData;
    }


    /**
     * Initialize the string table.
     */
    function initsTable() {
        $this->sTable = array();

        for ($i = 0; $i < 256; $i++)
            $this->sTable[$i] = chr($i);

        $this->tIdx = 258;
        $this->bitsToGet = 9;
    }

    /**
     * Add a new string to the string table.
     */
    function addStringToTable ($oldString, $newString="") {
        $string = $oldString.$newString;

        
        $this->sTable[$this->tIdx++] = $string;

        if ($this->tIdx == 511) {
            $this->bitsToGet = 10;
        } else if ($this->tIdx == 1023) {
            $this->bitsToGet = 11;
        } else if ($this->tIdx == 2047) {
            $this->bitsToGet = 12;
        }
    }

    
    function getNextCode() {
        if ($this->bytePointer == strlen($this->data))
            return 257;

        $this->nextData = ($this->nextData << 8) | (ord($this->data[$this->bytePointer++]) & 0xff);
        $this->nextBits += 8;

        if ($this->nextBits < $this->bitsToGet) {
            $this->nextData = ($this->nextData << 8) | (ord($this->data[$this->bytePointer++]) & 0xff);
            $this->nextBits += 8;
        }

        $code = ($this->nextData >> ($this->nextBits - $this->bitsToGet)) & $this->andTable[$this->bitsToGet-9];
        $this->nextBits -= $this->bitsToGet;

        return $code;
    }
}