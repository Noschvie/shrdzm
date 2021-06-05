<?php
require __DIR__.'/logging.php';
require __DIR__.'/classes/Database.php';

function responseGenerator($text, $shouldEndSession)
{
	$response = [
			'version' => '1.0',
			'sessionAttributes' => null,
			'response' => [
				  'outputSpeech' => [
						'type' => 'SSML',
						'ssml' => '<speak>'.$text.'</speak>'
				  ],
				  'shouldEndSession' => $shouldEndSession
			]
	  ];	
	  
	return json_encode($response);
}

function navigateHome()
{		
	$response = [
			'version' => '1.0',
			'sessionAttributes' => null,
			'response' => [
				  'outputSpeech' => [
						'type' => 'SSML',
						'ssml' => '<speak>Ich gehe zum Startbildschirm</speak>'
				  ],
				  'directives' => [[
						'type' => 'Alexa.Presentation.APL.RenderDocument',
						'token' => 'homeToken',
						'document' => [
							'src' => 'doc://alexa/apl/documents/SHRDZM',
							'type' => 'Link'
						],					  						
						'datasources' => [
							'text' => [
								'start' => 'SHRDZM'
							],					  						
							'assets' => [
								'shrdzm' => 'https://pintarweb.selfhost.at/alexa/statistic/logo_200.jpg?raw=true'
							]							
						]
				  ]],					  
				  'shouldEndSession' => true
			]
	  ];	
	  
	  loging2file("navigateHome = ".json_encode($response));

	return json_encode($response);		
}

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
			
			$fetch_devices_from_id = 'SELECT name, type, alias FROM `devices` WHERE `userid`="'.$rowUser['id'].'"';
			$query_devices = $conn->prepare($fetch_devices_from_id);
			$query_devices->execute();
			
			if($query_devices->rowCount())
			{
				$returnData = "";
				
				while ($row = $query_devices->fetch(PDO::FETCH_ASSOC)) 
				{
					if($row['alias'] != "")
						$returnData .= ", ".$row['alias']." vom typ ".$row['type'];
					else						
						$returnData .= ", ".$row['name']." vom typ ".$row['type'];
				}				
			}			
		}
	}
	catch(PDOException $e)
	{
		return "error";
	}	
	
	return $returnData.". Von welchem sensor willst du die werte wissen? du kannst mich auch nach allen fragen.";
}

function getDeviceDataParameterList($conn, $type)
{
	$fetch_device_data_parameter_list = 'SELECT parameter FROM `devicetypeparameter` WHERE `type`="'.$type.'"';
	$query_device_data_parameter_list = $conn->prepare($fetch_device_data_parameter_list);
	$query_device_data_parameter_list->execute();
	
	$deviceDataParameterList = array();
	
	if($query_device_data_parameter_list->rowCount())
	{
		while ($rowValue = $query_device_data_parameter_list->fetch(PDO::FETCH_ASSOC))
		{
			$deviceDataParameterList[] = $rowValue['parameter'];
		}
	}
	
	return $deviceDataParameterList;
}

function getDeviceDataValues($token, $aliasname)
{
	$closing = true;
	
	$db_connection = new Database();
	$conn = $db_connection->dbConnection();

	if(isset($aliasname))
		$returnData = "leider keine messwerte von ".$aliasname." welche werte soll ich dir anstelle dessen sagen?";
	else
		$returnData = "leider keine messwerte";

	try{
		
		$fetch_user_by_token = 'SELECT id FROM `users` WHERE `alexa_userid`="'.$token.'"';
		$query_stmt = $conn->prepare($fetch_user_by_token);
		$query_stmt->execute();

		// IF THE USER IS FOUNDED BY NAME
		if($query_stmt->rowCount())
		{
            $rowUser = $query_stmt->fetch(PDO::FETCH_ASSOC);			
			
			if(isset($aliasname))
				$fetch_devices_from_id = 'SELECT name, type, alias FROM `devices` WHERE `userid`="'.$rowUser['id'].'" AND alias="'.$aliasname.'"';
			else
				$fetch_devices_from_id = 'SELECT name, type, alias FROM `devices` WHERE `userid`="'.$rowUser['id'].'"';
			

			$query_devices = $conn->prepare($fetch_devices_from_id);
			$query_devices->execute();
			
			if($query_devices->rowCount())
			{
				if(isset($aliasname))				
					$returnData = "Letzte messungen für ".$aliasname.": ";
				else
					$returnData = "Letzte messungen für alle sensoren: ";
								
				while ($row = $query_devices->fetch(PDO::FETCH_ASSOC))				
				{
					// get parameters from found devices`
					$deviceDataParameterList = getDeviceDataParameterList($conn, $row['type']);
					
					foreach ($deviceDataParameterList as $para) 
					{
						$fetch_last_value_from_device = 'SELECT * FROM `data` WHERE `name`="'.$row['name'].'" AND reading="'.$para.'" order by timestamp desc limit 1';
						$query_last_value_from_device = $conn->prepare($fetch_last_value_from_device);
						$query_last_value_from_device->execute();
						
						if($query_last_value_from_device->rowCount())
						{
							while ($rowValue = $query_last_value_from_device->fetch(PDO::FETCH_ASSOC))
							{
								$fetch_devicetype_parameter = 'SELECT * FROM `devicetypeparameter` WHERE `type`="'.$row['type'].'" and parameter="'.$rowValue['reading'].'"';
								
								$query_devicetype_parameter = $conn->prepare($fetch_devicetype_parameter);
								$query_devicetype_parameter->execute();
								
								if($query_devicetype_parameter->rowCount() == 1)
								{
									$rowParameter = $query_devicetype_parameter->fetch(PDO::FETCH_ASSOC);
									if(isset($aliasname))				
										$returnData .= $rowParameter['alias_de']." : ".str_replace('.', ' komma ', $rowValue['value'])." ".$rowParameter['unit'].". ";
									else
										$returnData .= $row['alias'].": ".$rowParameter['alias_de']." : ".str_replace('.', ' komma ', $rowValue['value'])." ".$rowParameter['unit'].". ";
								}
								else
								{
									$returnData .= $rowValue['reading']." : ".$rowValue['value']." einheiten. ";
								}
							}						
						}					
					}
				}				
			}
			$closing = false;
		}
	}
	catch(PDOException $e)
	{
		return array("hatte leider einen fehler bei der datenbankabfrage. versuche es bitte später nochmal", true);
//		return "error";
	}	
	
	return array($returnData, $closing);
//	return $returnData;
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
							'ssml' => '<speak>Damit deine Sensoren abgefragt werden können, musst du den skill erst mit deinem <phoneme alphabet="ipa" ph="ˈʃɹdzm">SHRDZM</phoneme> verbinden. Gehe bitte in die Alexa app und führe eine kontoverknüpfung durch. Zum testen kannst du als benutzername "test" und als passwort "test0001" verwenden. alles kleingeschrieben. Viel spass</speak>'
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
							'ssml' => '<speak>Hallo '.$name.', Willkommen bei <phoneme alphabet="ipa" ph="ˈʃɹdzm">SHRDZM</phoneme>. Was kann ich für dich tun?</speak>'
					  ],
					  'shouldEndSession' => false
				]
		  ];
	}

	return json_encode ( $response );
}

function setDeviceAlias($obj)
{
	$replytext = 'hat leider nicht geklappt';
	
	$listnummer = $obj->{'request'}->{'intent'}->{'slots'}->{'listnummer'}->{'value'};
	$aliasname = $obj->{'request'}->{'intent'}->{'slots'}->{'aliasname'}->{'value'};
	
	$replytext = "Ich setze ".$aliasname." für device nummer ".$listnummer;
	
//	$replytext = listDevices($obj->{'session'}->{'user'}->{'accessToken'});

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

function deviceDataRequest($obj)
{
	$aliasname = null;
	
	if(isset($obj->{'request'}->{'intent'}->{'slots'}->{'aliasname'}->{'value'}))	
		$aliasname = $obj->{'request'}->{'intent'}->{'slots'}->{'aliasname'}->{'value'};
		
	if(!isset($aliasname) && isset($obj->{'request'}->{'intent'}->{'slots'}->{'ort'}->{'value'}))
		$aliasname = $obj->{'request'}->{'intent'}->{'slots'}->{'ort'}->{'value'};
			
	$replytext = 'leider keine information zu '.$aliasname." vorhanden";
	
	$replytext = getDeviceDataValues($obj->{'session'}->{'user'}->{'accessToken'}, $aliasname);

	$response = [
			'version' => '1.0',
			'sessionAttributes' => null,
			'response' => [
				  'outputSpeech' => [
						'type' => 'SSML',
						'ssml' => '<speak>'.$replytext[0].'</speak>'
				  ],
				  'shouldEndSession' => $replytext[1]
			]
	  ];
		
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
				  'shouldEndSession' => false
			]
	  ];

	return json_encode ( $response );	
}

function getHelp($obj)
{
	$replytext = 'Frag mich zum beispiel welche sensoren ich unterstütze, frag nach den messwerten aller sensoren oder frag direkt nach den messwerten von einem sensor';
	
	$response = [
			'version' => '1.0',
			'sessionAttributes' => null,
			'response' => [
				  'outputSpeech' => [
						'type' => 'SSML',
						'ssml' => '<speak>'.$replytext.'</speak>'
				  ],
				  'shouldEndSession' => false
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
loging2file("requestType = ".$requestType);

$replyToSender = responseGenerator("Das habe ich jetzt nicht verstanden. Sag Hilfe wenn du hilfe benötigst", false);

if($requestType == "LaunchRequest")
{
	$replyToSender = LaunchRequest($obj);
}
else if($requestType == "IntentRequest")
{
	$intentName = $obj->{'request'}->{'intent'}->{'name'};
	loging2file("$intentName = ".$intentName);
	
	if($intentName == "DeviceList")
	{
		$replyToSender = deviceList($obj);
	}
	if($intentName == "DeviceAlias")
	{
		$replyToSender = setDeviceAlias($obj);
	}
	if($intentName == "DeviceDataRequest")
	{
		$replyToSender = deviceDataRequest($obj);
	}
	if($intentName == "AMAZON.StopIntent")
	{
		$replyToSender = responseGenerator("Adios", true);
	}
	if($intentName == "AMAZON.CancelIntent")
	{
		$replyToSender = responseGenerator("Adios", true);
	}
	if($intentName == "AMAZON.HelpIntent")
	{
		$replyToSender = getHelp($obj);
	}
	if($intentName == "AMAZON.FallbackIntent")
	{
		$replyToSender = responseGenerator("Das habe ich jetzt nicht verstanden. Sag Hilfe wenn du hilfe benötigst", false);
	}
	if($intentName == "AMAZON.NavigateHomeIntent")
	{
		$replyToSender = navigateHome();
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
