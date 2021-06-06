<?php
require __DIR__.'/logging.php';
require __DIR__.'/classes/Database.php';


const appID = 'amzn1.ask.skill.3f009f9d-ad83-49fc-b4f8-d2d162417f45';

class Reply {
    public $appID = "";
    public $antwort  = "";
	public $state = "NOK";
	public $closing = true;
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

	if($aliasname != "")
		$returnData = "leider keine messwerte von ".$aliasname.". welche werte soll ich dir anstelle dessen sagen?";
	else
		$returnData = "leider keine messwerte. ";

	try{
		
		$fetch_user_by_token = 'SELECT id FROM `users` WHERE `alexa_userid`="'.$token.'"';
		$query_stmt = $conn->prepare($fetch_user_by_token);
		$query_stmt->execute();

		// IF THE USER IS FOUNDED BY NAME
		if($query_stmt->rowCount())
		{
            $rowUser = $query_stmt->fetch(PDO::FETCH_ASSOC);			
			
			if($aliasname != "")
				$fetch_devices_from_id = 'SELECT name, type, alias FROM `devices` WHERE `userid`="'.$rowUser['id'].'" AND alias="'.$aliasname.'"';
			else
				$fetch_devices_from_id = 'SELECT name, type, alias FROM `devices` WHERE `userid`="'.$rowUser['id'].'"';
			

			$query_devices = $conn->prepare($fetch_devices_from_id);
			$query_devices->execute();
			
			if($query_devices->rowCount())
			{
				if($aliasname != "")
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
									if($aliasname != "")
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
	}	
	
	return array($returnData, $closing);
}


$entityBody = file_get_contents ( 'php://input' );
$entityBodyJSON = json_decode($entityBody);

// prepare reply
$reply = new Reply();
$reply->appID = $entityBodyJSON->appID;

// check appID
if($entityBodyJSON->appID != appID)
{
	loging2file('WRONG APPID');
	$reply->state = 'WRONG APPID';
	$reply->closing = true;
}
else
{
	$reply->state = 'OK';	

	$replyArray = getDeviceDataValues($entityBodyJSON->accessToken, $entityBodyJSON->aliasname);
	$reply->antwort = $replyArray[0];
	$reply->closing = $replyArray[1];
}

$replyToSender = json_encode ( $reply );

header ( 'Content-Type: application/json' );
echo $replyToSender;
return;
?>
