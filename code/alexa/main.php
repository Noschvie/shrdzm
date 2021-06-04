<?php
require __DIR__.'/logging.php';
require __DIR__.'/classes/Database.php';


function extractNameFromToken($token)
{
	$db_connection = new Database();
	$conn = $db_connection->dbConnection();

	$returnData = "unbekannter";

	try{
		
		$fetch_user_by_token = 'SELECT name FROM `users` WHERE `alexa_userid`="'.$token.'"';
		$query_stmt = $conn->prepare($fetch_user_by_token);
		$query_stmt->execute();

		// IF THE USER IS FOUNDED BY NAME
		if($query_stmt->rowCount())
		{
            $row = $query_stmt->fetch(PDO::FETCH_ASSOC);			
			$returnData = $row['name'];
		}
	}
	catch(PDOException $e)
	{
		return "error";
	}	
	
	return $returnData;
}

function listDevices($token)
{
	$db_connection = new Database();
	$conn = $db_connection->dbConnection();

	$returnData = "keine";

	try{
		
		$fetch_user_by_token = 'SELECT id FROM `users` WHERE `alexa_userid`="'.$token.'"';
		$query_stmt = $conn->prepare($fetch_user_by_token);
		$query_stmt->execute();

		// IF THE USER IS FOUNDED BY NAME
		if($query_stmt->rowCount())
		{
            $rowUser = $query_stmt->fetch(PDO::FETCH_ASSOC);			
			
			$fetch_devices_from_id = 'SELECT name, type FROM `devices` WHERE `userid`="'.$rowUser['id'].'"';
			$query_devices = $conn->prepare($fetch_devices_from_id);
			$query_devices->execute();
			
			if($query_devices->rowCount())
			{
				$returnData = "";
				
				while ($row = $query_devices->fetch(PDO::FETCH_ASSOC)) 
				{
					$returnData .= ", ".$row['name']." vom typ ".$row['type'];
				}				
			}			
		}
	}
	catch(PDOException $e)
	{
		return "error";
	}	
	
	return $returnData;
}
function LaunchRequest($obj)
{	 	
	$response = [
			'version' => '1.0',
			'sessionAttributes' => null,
			'response' => [
				  'outputSpeech' => [
						'type' => 'SSML',
						'ssml' => '<speak>Fehler beim Interpretieren der Anfrage. Bitte Skill Entwickler kontaktieren</speak>'
				  ],
				  'shouldEndSession' => true
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
	else
	{
		$name = extractNameFromToken($obj->{'session'}->{'user'}->{'accessToken'});

		$response = [
				'version' => '1.0',
				'sessionAttributes' => null,
				'response' => [
					  'outputSpeech' => [
							'type' => 'SSML',
							'ssml' => '<speak>Hallo '.$name.', Was kann ich für dich tun?</speak>'
					  ],
					  'shouldEndSession' => false
				]
		  ];
	}

	return json_encode ( $response );
}

function deviceList($obj)
{
	$replytext = 'werde ich dir irgendwann mal sagen';
	
	$replytext = listDevices($obj->{'session'}->{'user'}->{'accessToken'});

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
		$replyToSender = deviceList($obj);
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
