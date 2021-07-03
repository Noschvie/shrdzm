<?php

function loging2file($text)
{
	$serverName = $_SERVER['SERVER_NAME'];
	
	$logfileName = '/var/log/apache2/'.date('Y-m-d').'_shrdzm.log';
	
	$logdatei_handle = fopen ( $logfileName, "a+" );
	
	fputs ( $logdatei_handle, date('Y-m-d H:i:s')." [".$serverName."] ".$text."\n" );

	fclose ( $logdatei_handle );	
}

?>