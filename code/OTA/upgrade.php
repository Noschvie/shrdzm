<?PHP
include("logging.php");

loging2file('SHRDZM upgrade called');

header('Content-type: text/plain; charset=utf8', true);

function check_header($name, $value = false) {
	if(!isset($_SERVER[$name])) {
		return false;
	}
	if($value && $_SERVER[$name] != $value) {
		return false;
	}
	return true;
}

function sendFile($path) {
    header($_SERVER["SERVER_PROTOCOL"].' 200 OK', true, 200);
    header('Content-Type: application/octet-stream', true);
    header('Content-Disposition: attachment; filename='.basename($path));
    header('Content-Length: '.filesize($path), true);
    header('x-MD5: '.md5_file($path), true);
    readfile($path);
}

if(!check_header('HTTP_USER_AGENT', 'ESP8266-http-Update')) {
	header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
	echo "only for SHRDZM updater!\n";
	exit();
}

if(
	!check_header('HTTP_X_ESP8266_STA_MAC') ||
	!check_header('HTTP_X_ESP8266_AP_MAC') ||
	!check_header('HTTP_X_ESP8266_FREE_SPACE') ||
	!check_header('HTTP_X_ESP8266_SKETCH_SIZE') ||
	!check_header('HTTP_X_ESP8266_CHIP_SIZE') ||
	!check_header('HTTP_X_ESP8266_SDK_VERSION') ||
	!check_header('HTTP_X_ESP8266_VERSION')
) {
	header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
	echo "only for SHRDZM updater! (header)\n";
	exit();
}

$localSHRDZMSensorBinary = "./bin/SHRDZMSensor.ino.generic.bin";
$localSHRDZMGatewayBinary = "./bin/SHRDZMGateway.ino.generic.bin";
$localSHRDZMGatewayMQTTBinary = "./bin/SHRDZMGatewayMQTT.ino.generic.bin";


loging2file($_SERVER['HTTP_USER_AGENT']);
loging2file($_SERVER['HTTP_X_ESP8266_STA_MAC']);
loging2file($_SERVER['HTTP_X_ESP8266_AP_MAC']);
loging2file($_SERVER['HTTP_X_ESP8266_FREE_SPACE']);
loging2file($_SERVER['HTTP_X_ESP8266_SKETCH_SIZE']);
loging2file($_SERVER['HTTP_X_ESP8266_CHIP_SIZE']);
loging2file($_SERVER['HTTP_X_ESP8266_SDK_VERSION']);
loging2file($_SERVER['HTTP_X_ESP8266_VERSION']);

$versionparts = explode(" ", $_SERVER['HTTP_X_ESP8266_VERSION']);
$parts = count($versionparts);

if($parts == 3)
{
	if($versionparts[0] == "SHRDZMSensor")
	{
		loging2file(md5_file($localSHRDZMSensorBinary));
		if($versionparts[2] != md5_file($localSHRDZMSensorBinary))
		{
			sendFile($localSHRDZMSensorBinary);
			exit();
		}
		else
		{
			loging2file("no new version for ".$_SERVER['HTTP_X_ESP8266_STA_MAC']);
			header($_SERVER["SERVER_PROTOCOL"].' 304 Not Modified', true, 304);
		}
	}
	elseif($versionparts[0] == "SHRDZMGateway")
	{
		loging2file(md5_file($localSHRDZMGatewayBinary));
		if($versionparts[2] != md5_file($localSHRDZMGatewayBinary))
		{
			sendFile($localSHRDZMGatewayBinary);
			exit();
		}
		else
		{
			loging2file("no new version for ".$_SERVER['HTTP_X_ESP8266_STA_MAC']);
			header($_SERVER["SERVER_PROTOCOL"].' 304 Not Modified', true, 304);
		}
	}
	elseif($versionparts[0] == "SHRDZMGatewayMQTT")
	{
		loging2file(md5_file($localSHRDZMGatewayMQTTBinary));
		if($versionparts[2] != md5_file($localSHRDZMGatewayMQTTBinary))
		{
			sendFile($localSHRDZMGatewayMQTTBinary);
			exit();
		}
		else
		{
			loging2file("no new version for ".$_SERVER['HTTP_X_ESP8266_STA_MAC']);
			header($_SERVER["SERVER_PROTOCOL"].' 304 Not Modified', true, 304);
		}
	}
	else
	{
		loging2file("update for unknown device ".$_SERVER['HTTP_X_ESP8266_STA_MAC']." not delivered");
		header($_SERVER["SERVER_PROTOCOL"].' 500 no version for this device', true, 500);
	}
}
else
{
//	loging2file("version does not follow specification for device ".$_SERVER['HTTP_X_ESP8266_STA_MAC']." and will not be not be delivered");
//	header($_SERVER["SERVER_PROTOCOL"].' 500 no version for this device', true, 500);
	loging2file("version does not follow specification for device ".$_SERVER['HTTP_X_ESP8266_STA_MAC']." but I will try to update");
	
	sendFile($localSHRDZMSensorBinary);
	exit();
}


?>