<?php

function loging2file($text)
{
	$serverName = $_SERVER['SERVER_NAME'];
	
	$logfileName = '/var/log/shrdzm/'.date('Y-m-d').'_echo_shrdzm.log';
	
	$logdatei_handle = fopen ( $logfileName, "a+" );
	
	fputs ( $logdatei_handle, date('Y-m-d H:i:s')." [".$serverName."] ".$text."\n" );

	fclose ( $logdatei_handle );	
}

function logging2file($text)
{
	$serverName = $_SERVER['SERVER_NAME'];
	
	$logfileName = '/var/log/shrdzm/'.date('Y-m-d').'_echo_shrdzm.log';
	
	$logdatei_handle = fopen ( $logfileName, "a+" );
	
	fputs ( $logdatei_handle, date('Y-m-d H:i:s')." [".$serverName."] ".$text."\n" );

	fclose ( $logdatei_handle );	
}
?>