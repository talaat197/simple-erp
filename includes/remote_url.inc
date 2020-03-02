<?php
/**********************************************************************
    
***********************************************************************/

/*
	Read content of remote url via http.
	Does not require curl php extension nor allow_url_fopen=1.
*/
function url_get_contents($url, $timeout=10)
{
	
	$parsedUrl = parse_url($url);

	if (@$parsedUrl['scheme'] == 'file')
		return file_get_contents($parsedUrl['path']);

	$host = $parsedUrl['host'];

	if (isset($parsedUrl['path'])) {
		$path = $parsedUrl['path'];
	} else {
		
		$path = '/';
	}

	if (isset($parsedUrl['query'])) {
		$path .= '?' . $parsedUrl['query'];
	}

	if (isset($parsedUrl['port'])) {
		$port = $parsedUrl['port'];
	} else {
		
		$port = '80';
	}

	$response = '';
	
	$fp = @fsockopen($host, $port, $errno, $errstr, $timeout );
	if( !$fp ) {
		return null;
	} else {
		
		fputs($fp, "GET $path HTTP/1.0\r\n" .
			"Host: $host\r\n".
			(isset($parsedUrl['pass']) ? "Authorization: Basic ".
				base64_encode($parsedUrl['user'].':'.$parsedUrl['pass'])."\r\n" : '').
			"User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.0.3) Gecko/20060426 Firefox/1.5.0.3\r\n" .
			"Accept: */*\r\n" .
			"Accept-Language: en-us,en;q=0.5\r\n" .
			"Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n" .
			"Connection: close\r\n" .
			"Referer: http://$host\r\n\r\n");

		

		$response = stream_get_contents($fp);

		if (!strpos($response, "200 OK\r\n"))
			return null;
		
		$pos = strpos($response, "\r\n\r\n");

		$response = substr($response, $pos + 4);
	}

	
	return $response;
}

function url_copy($from, $to, $timeout=10)
{
	$f = fopen($to, 'wb');
	if (!$f || !fwrite($f, url_get_contents($from, $timeout)))
		return false;
	fclose($f);
	return true;
}
