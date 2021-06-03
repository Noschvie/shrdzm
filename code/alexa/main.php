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
	$r = '{'
		. '"version": "1.0",'
		. '"response": {'
		. '	"card": {'
		. '		"type": "LinkAccount"'
		. '	},'		
		. '	"outputSpeech": {'
		. '		"type": "PlainText",'
		. '		"text": "Was kann ich für dich tun?"'
		. '	},'		
/*		. '"directives": ['	
		
		
. '{'
. '  "type": "Alexa.Presentation.APL.RenderDocument",'
. '  "token": "helloworldToken",'
. '  "document":'		
. '  {'
. '        "src": "doc://alexa/apl/documents/SHRDZM",'
. '        "type": "Link"'
. '  },'
. '    "datasources": {'
. '        "text": {'
. '            "start": "SHRDZM"'
. '        },'
. '        "assets": {'
. '            "shrdzm": "https://pintarweb.selfhost.at/alexa/statistic/logo_200.jpg?raw=true"'
. '        }'
. '    },'
. '    "sources": {}'

. '}'
		. '],'*/
		. '	"shouldEndSession": false'
		. '}'
		. '}';

	if(!isset($obj->{'session'}->{'user'}->{'accessToken'}))
	{
		$r = '{'
			. '"version": "1.0",'
			. '"response": {'
			. '	"card": {'
			. '		"type": "LinkAccount"'
			. '	},'		
			. '	"outputSpeech": {'
			. '		"type": "PlainText",'
			. '		"text": "Bitte mit Account verlinken"'
			. '	},'		
			. '	"shouldEndSession": true'
			. '}'
			. '}';
	}

	logReply(json_encode($response));

	return $r;
}

function StatusRequest($obj)
{
	$replytext = 'Kann Status nicht ermitteln';
	
	$anwesenheit = explode(' ', preg_replace("/\s+/", " ", shell_exec('/usr/bin/perl /opt/fhem/fhem.pl 192.168.1.75:7072 "list Anwesenheit state"')))[3]; 

	$solveroutput = shell_exec('/usr/bin/perl /opt/fhem/fhem.pl 192.168.1.75:7072 "list LaCrosse_03 temperature"');
	$temperature = preg_split('/\s+/', $solveroutput)[3];
	
	$solveroutput = shell_exec('/usr/bin/perl /opt/fhem/fhem.pl 192.168.1.75:7072 "list MAX_1663d3 valveposition"');
	$heizungein = intval(preg_split('/\s+/', $solveroutput)[3]) > 0 ? 'ein' : 'aus';

	
	if($anwesenheit == 'present')
	{
		$replytext = 'Es ist jemand zu hause, die Temperatur im Wohnzimmer beträgt '.str_replace(".",",",$temperature).'° und die Heizung ist '.$heizungein;
	}
	else
	{		
		$replytext = 'Es ist niemand zu hause, die Temperatur im Wohnzimmer beträgt '.str_replace(".",",",$temperature).'° und die Heizung ist '.$heizungein;
	}
	
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

function StromRequest($obj)
{
	$replytext = 'Kann Status nicht ermitteln';
	
	$solveroutput = shell_exec('/usr/bin/perl /opt/fhem/fhem.pl 192.168.1.75:7072 "list SHRDZM_DC4F225823F5 counter_power_usage_in"');
	$watt = preg_split('/\s+/', $solveroutput)[3];
	
	$solveroutput = shell_exec('/usr/bin/perl /opt/fhem/fhem.pl 192.168.1.75:7072 "list SHRDZM_DC4F225823F5 statCounter_reading_p_inDay"');
	$kilowattstunden = preg_split('/\s+/', $solveroutput)[3] / 1000;
	$kilowattstunden = str_replace('.',',',$kilowattstunden);

	$response = [
			'version' => '1.0',
			'sessionAttributes' => null,
			'response' => [
				  'outputSpeech' => [
						'type' => 'SSML',
						'ssml' => '<speak>Der aktuelle Stromverbrauch liegt bei '.$watt.' watt. Insgesamt wurden heute schon '.$kilowattstunden.' kilowattstunden verbraucht.</speak>'
				  ],
				  'shouldEndSession' => true
			]
	  ];

	return json_encode ( $response );
}

function KlimaRequest($obj)
{
	$room = $obj->{'request'}->{'intent'}->{'slots'}->{'room'}->{'value'};
//	$room = $obj->{'request'}->{'intent'}->{'slots'}->{'room'}->{'resolutions'}->{'resolutionsPerAuthority'->{'resolutionsPerAuthority'};

	$entityBodyJSON = json_decode($obj);
	
	if($room == "" || $room == null)
	{
		$response = [
				'version' => '1.0',
				'response' => [
					  'outputSpeech' => [
							'type' => 'PlainText',
							'text' => 'Das habe ich jetzt leider nicht verstanden.',
							'ssml' => null
					  ],
					  'shouldEndSession' => true
				]
		  ];
	}
	else
	{
		// Wohnzimmer | Büro | Schlafzimmer | Küche | Balkon | Sabrinazimmer
		$attributType = 0;
		$artikel = "im";
		
		switch(strtolower($room))
		{
			case "wohnzimmer":
				$climaRoom = "LaCrosse_03";
				$artikel = "im";
				break;
			case "büro":
				$climaRoom = "ONBOARD";
				$artikel = "im";
				$attributType = 1;
				break;
			case "schlafzimmer":
				$climaRoom = "Schlafzimmer_LaCrosse";
				$artikel = "im";
				break;
			case "balkon":
				$climaRoom = "Balkon_LaCrosse";
				$artikel = "am";
				break;
			case "küche":
				$climaRoom = "LaCrosse_10";
				$artikel = "in der";
				// $attributType = 1;
			case "kühlschrank":
				$climaRoom = "5CCF7F0F63A6";
				$artikel = "im";
				$attributType = 1;
				break;
			case "bad":
				$climaRoom = "18FE34CC55C6";
				$artikel = "im";
				$attributType = 1;
		}
		
		if($attributType == 0)
			$solveroutput = shell_exec('/usr/bin/perl /opt/fhem/fhem.pl 192.168.1.75:7072 "list '.$climaRoom.' temperature"');
		else
			$solveroutput = shell_exec('/usr/bin/perl /opt/fhem/fhem.pl 192.168.1.75:7072 "list '.$climaRoom.' Temperature"');
			
		$temperature = preg_split('/\s+/', $solveroutput)[3];
		if($attributType == 0)
			$solveroutput = shell_exec('/usr/bin/perl /opt/fhem/fhem.pl 192.168.1.75:7072 "list '.$climaRoom.' humidity"');
		else
			$solveroutput = shell_exec('/usr/bin/perl /opt/fhem/fhem.pl 192.168.1.75:7072 "list '.$climaRoom.' Humidity"');
			
		$humidity = preg_split('/\s+/', $solveroutput)[3];
		
		$response = [
				'version' => '1.0',
				'response' => [
					  'outputSpeech' => [
							'type' => 'PlainText',
							'text' => 'Die Temperatur '.$artikel.' '.$room.' beträgt '.str_replace(".",",",$temperature).'° und die Luftfeuchtigkeit liegt bei '.str_replace(".",",",$humidity).'%',
							'ssml' => null
					  ],
					  'shouldEndSession' => true
				]
		  ];
	}
	
	return json_encode ( $response );
}

// Daten aus dem Request holen
$entityBody = file_get_contents ( 'php://input' );

// Datei namens echo.log öffen
$datei_handle = fopen ( "/var/log/apache2/echo.log", "a+" );

////////////////
$obj = json_decode($entityBody);
$userID = $obj->{'session'}->{'user'}->{'userId'};

/* $allowedUserID = 'amzn1.ask.account.AF4UDNSH3H3SY65WLM64A2E35F4Y4XOOOBGUB7IPMS5KQ4BAXNEMJNGLDCWC2RIZZHJDSI756DNL7LHFOCKRHQVN27CT2NNTD74W76TMBF4UYUONJXAUDEYXNMVMHFM45LJKPSO67THTQURNFMYA4JC6SIEUNPLBHD5NZLY7TC5NG5BP3TPSM27GLKNWADYQRXTN7YEVFLWXB4A';

if($userID != $allowedUserID)
{
	$responseArray = [
            'version' => '1.0',
            'response' => [
                  'outputSpeech' => [
                        'type' => 'PlainText',
                        'text' => 'Dich kenne ich leider nicht, bitte verschwinde aus diesem Skill!',
                        'ssml' => null
                  ],
                  'shouldEndSession' => true
            ]
      ];



	header ( 'Content-Type: application/json' );
	echo json_encode ( $responseArray );
	
	fclose ( $datei_handle );
	return;
}
*/

// request type
$requestType = $obj->{'request'}->{'type'};

if($requestType == "LaunchRequest")
{
	$replyToSender = LaunchRequest($obj);
}
else if($requestType == "IntentRequest")
{
	$intentName = $obj->{'request'}->{'intent'}->{'name'};
	
	if($intentName == "Klima")
	{
		$replyToSender = KlimaRequest($obj);
	}
	else if($intentName == "status")
	{
		$replyToSender = StatusRequest($obj);
	}
	else if($intentName == "strom")
	{
		$replyToSender = StromRequest($obj);
	}
}

// etwas validierung
if (is_string($entityBody ) && json_decode ( $entityBody ) != null) {
// Request schoen formatieren
   fputs ( $datei_handle, "\n" );
   fputs ( $datei_handle, json_encode ( json_decode ( $entityBody ), JSON_PRETTY_PRINT ) );
   fputs ( $datei_handle, "\n" );
      
   fputs( $datei_handle, "reply = ");
   //fputs( $datei_handle, $solveroutput1);
   fputs ( $datei_handle, "\n" );
}
// alles speichern und beenden
fclose ( $datei_handle );


header ( 'Content-Type: application/json;charset=UTF-8' );
echo $replyToSender;
return;
?>
