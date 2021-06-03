<?php
function logReply($text)
{
	$datei_handle = fopen ( "/var/log/apache2/echo.log", "a+" );
	
	fputs ( $datei_handle, "\nlogging start:" );

	if (is_string($text )) 
	{
		fputs ( $datei_handle, $text );
		fputs ( $datei_handle, "\nLogging end" );		  
	}	
	
	fclose ( $datei_handle );	
}

function LaunchRequest($obj)
{	 
	$response = [
			'version' => '1.0',
			'sessionAttributes' => null,
			'response' => [
				  'outputSpeech' => [
						'type' => 'SSML',
						'ssml' => '<speak>Was kann ich für dich tun?</speak>'
				  ],
				  'shouldEndSession' => false
			]
	  ];

	if(!isset($obj->{'session'}->{'user'}->{'accessToken'}))
	{
		$response = [
				'version' => '1.0',
				'sessionAttributes' => null,
				'response' => [
					  'outputSpeech' => [
							'type' => 'SSML',
							'ssml' => '<speak>Damit Ihre Sensoren abgefragt werden können, müssen sie den skill erst mit SHRDM verbinden. Gehen sie bitte in die Alexa app und führen sie eine kontoverknüpfung durch</speak>'
					  ],
					  'card' => [
							'type' => 'LinkAccount'
					  ],					  
					  'shouldEndSession' => true
				]
		  ];
	}

	return json_encode ( $response );
}

function DeviceList($obj)
{
	$replytext = 'werde ich dir irgendwann mal sagen';
	

	$response = [
			'version' => '1.0',
			'sessionAttributes' => null,
			'response' => [
				  'outputSpeech' => [
						'type' => 'SSML',
						'ssml' => '<speak>'.$replytext.'</speak>'
				  ],
				  'shouldEndSession' => true
			]
	  ];

	return json_encode ( $response );	
}

// Daten aus dem Request holen
$entityBody = file_get_contents ( 'php://input' );

// Datei namens echo.log öffen
$datei_handle = fopen ( "/var/log/apache2/echo.log", "a+" );

////////////////
$obj = json_decode($entityBody);
$userID = $obj->{'session'}->{'user'}->{'userId'};

// request type
$requestType = $obj->{'request'}->{'type'};

if($requestType == "LaunchRequest")
{
	$replyToSender = LaunchRequest($obj);
}
else if($requestType == "IntentRequest")
{
	$intentName = $obj->{'request'}->{'intent'}->{'name'};
	
	if($intentName == "DeviceList")
	{
		$replyToSender = DeviceList($obj);
	}
}

// etwas validierung
if (is_string($entityBody ) && json_decode ( $entityBody ) != null) {
// Request schoen formatieren
   fputs ( $datei_handle, "\n" );
   fputs ( $datei_handle, json_encode ( json_decode ( $entityBody ), JSON_PRETTY_PRINT ) );
   fputs ( $datei_handle, "\n" );
      
   fputs( $datei_handle, "reply = ");
   fputs ( $datei_handle, "\n" );
}
// alles speichern und beenden
fclose ( $datei_handle );


header ( 'Content-Type: application/json;charset=UTF-8' );
echo $replyToSender;
return;
?>
