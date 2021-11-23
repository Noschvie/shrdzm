<?php
function logging2file($text)
{
	$serverName = $_SERVER['SERVER_NAME'];
	$documentRoot = $_SERVER['DOCUMENT_ROOT'];
	
	if($serverName == "api.shrdzm.com")
	{
		$logfileName = '/var/log/shrdzm/'.date('Y-m-d').'_shrdzm.log';
	}
	else
	{
		$logfileName = '../../priv/logs/'.date('Y-m-d').'_shrdzm.log';
	}
	
	$logdatei_handle = fopen ( $logfileName, "a+" );
	
	fputs ( $logdatei_handle, date('Y-m-d H:i:s')." [".$serverName.",".$documentRoot."] ".$text."\n" );

	fclose ( $logdatei_handle );	
}

function loging2file($text)
{
	$serverName = $_SERVER['SERVER_NAME'];
	$documentRoot = $_SERVER['DOCUMENT_ROOT'];
	
	if($serverName == "api.shrdzm.com")
	{
		$logfileName = '/var/log/shrdzm/'.date('Y-m-d').'_shrdzm.log';
	}
	else
	{
		$logfileName = '../../priv/logs/'.date('Y-m-d').'_shrdzm.log';
	}
	
	$logdatei_handle = fopen ( $logfileName, "a+" );
	
	fputs ( $logdatei_handle, date('Y-m-d H:i:s')." [".$serverName.",".$documentRoot."] ".$text."\n" );

	fclose ( $logdatei_handle );	
}
?>