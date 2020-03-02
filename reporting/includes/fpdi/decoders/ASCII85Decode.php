<?php
//

//

//



//

//





//

if (!defined("ORD_z"))
	define("ORD_z",ord('z'));
if (!defined("ORD_exclmark"))
	define("ORD_exclmark", ord('!'));
if (!defined("ORD_u"))	
	define("ORD_u", ord("u"));
if (!defined("ORD_tilde"))
	define("ORD_tilde", ord('~'));

class ASCII85Decode {

    function __construct(&$fpdi) {
        $this->fpdi =& $fpdi;
    }


    function decode($in) {
        $out = "";
        $state = 0;
        $chn = null;
        
        $l = strlen($in);
        
        for ($k = 0; $k < $l; ++$k) {
            $ch = ord($in[$k]) & 0xff;
            
            if ($ch == ORD_tilde) {
                break;
            }
            if (preg_match("/^\s$/",chr($ch))) {
                continue;
            }
            if ($ch == ORD_z && $state == 0) {
                $out .= chr(0).chr(0).chr(0).chr(0);
                continue;
            }
            if ($ch < ORD_exclmark || $ch > ORD_u) {
                $this->fpdi->error("Illegal character in ASCII85Decode.");
            }
            
            $chn[$state++] = $ch - ORD_exclmark;
            
            if ($state == 5) {
                $state = 0;
                $r = 0;
                for ($j = 0; $j < 5; ++$j)
                    $r = $r * 85 + $chn[$j];
                $out .= chr($r >> 24);
                $out .= chr($r >> 16);
                $out .= chr($r >> 8);
                $out .= chr($r);
            }
        }
        $r = 0;
        
        if ($state == 1)
            $this->fpdi->error("Illegal length in ASCII85Decode.");
        if ($state == 2) {
            $r = $chn[0] * 85 * 85 * 85 * 85 + ($chn[1]+1) * 85 * 85 * 85;
            $out .= chr($r >> 24);
        }
        else if ($state == 3) {
            $r = $chn[0] * 85 * 85 * 85 * 85 + $chn[1] * 85 * 85 * 85  + ($chn[2]+1) * 85 * 85;
            $out .= chr($r >> 24);
            $out .= chr($r >> 16);
        }
        else if ($state == 4) {
            $r = $chn[0] * 85 * 85 * 85 * 85 + $chn[1] * 85 * 85 * 85  + $chn[2] * 85 * 85  + ($chn[3]+1) * 85 ;
            $out .= chr($r >> 24);
            $out .= chr($r >> 16);
            $out .= chr($r >> 8);
        }

        return $out;
    }
}