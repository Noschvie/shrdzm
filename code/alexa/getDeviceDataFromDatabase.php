<?php
require __DIR__.'/logging.php';
require __DIR__.'/classes/Database.php';


const appID = 'amzn1.ask.skill.3f009f9d-ad83-49fc-b4f8-d2d162417f45';

function msg($success,$status,$message,$extra = []){
    return array_merge([
        'success' => $success,
        'status' => $status,
        'message' => $message,
		'antwort' => $message
    ],$extra);
}

$returnData = [
    "success" => 0,
    "status" => 401,
    "message" => "Unauthorized",
    "antwort" => "Unauthorized"
];

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

function getDeviceDataValues($token, $alias, $name)
{
	$closing = true;
	
	$db_connection = new Database();
	$conn = $db_connection->dbConnection();

	if($alias != "")
		$returnData = "leider keine messwerte von ".$alias.". welche werte soll ich dir anstelle dessen sagen?";
	else
		$returnData = "leider keine messwerte. ";

	$messageString = "";


	try{
		
		$fetch_user_by_token = 'SELECT id FROM `users` WHERE `alexa_userid`="'.$token.'"';
		$query_stmt = $conn->prepare($fetch_user_by_token);
		$query_stmt->execute();

		// IF THE USER IS FOUNDED BY NAME
		if($query_stmt->rowCount())
		{
            $rowUser = $query_stmt->fetch(PDO::FETCH_ASSOC);			
			
			if($alias != "")
				$fetch_devices_from_id = 'SELECT name, type, alias FROM `devices` WHERE `userid`="'.$rowUser['id'].'" AND alias="'.$alias.'"';
			else if($name != "")
				$fetch_devices_from_id = 'SELECT name, type, alias FROM `devices` WHERE `userid`="'.$rowUser['id'].'" AND name="'.$name.'"';
			else			
				$fetch_devices_from_id = 'SELECT name, type, alias FROM `devices` WHERE `userid`="'.$rowUser['id'].'"';
			
			loging2file($fetch_devices_from_id);
			
			$query_devices = $conn->prepare($fetch_devices_from_id);
			$query_devices->execute();

			if($query_devices->rowCount())
			{
				$returnDataString = '{ "success" : 1, "status" : 201, "message" : "%message%", "message" : "%message%", "antwort" : "%antwort%", "data" : [';
				$counter = 0;
								
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
									
									if($counter > 0)
										$returnDataString .= ',';
										
									$counter++;		
								
									$returnDataString .= '{"timestamp" : "'.$rowValue['timestamp'].'", "name" : "'.$row['name'].'", "alias" : "'.$row['alias'].'", "value" : "'.$rowValue['value'].'", "unit" : "'.$rowParameter['unit'].'", "desc" : "'.$rowParameter['alias_de'].'"}';									

									if($alias != "")
										$messageString .= $rowParameter['alias_de']." : ".str_replace('.', ' komma ', $rowValue['value'])." ".$rowParameter['unit'].". ";
									else
										$messageString .= $row['alias'].": ".$rowParameter['alias_de']." : ".str_replace('.', ' komma ', $rowValue['value'])." ".$rowParameter['unit'].". ";																		
								}
							}						
						}					
					}
				}		

				$returnDataString .= "]}";
				
				$returnDataString = str_replace("%message%", $messageString, $returnDataString);
				$returnDataString = str_replace("%antwort%", $messageString, $returnDataString);
				
				$returnData = json_decode($returnDataString);				
			}
			else
			{
				return msg(0, 501, "Device not found");
			}
		}
		else
		{
			return msg(0, 401, "Unauthorized");
		}		
	}
	catch(PDOException $e)
	{
		return msg(0, $e->$code, $e->$message);
	}	
	
	return $returnData;
}


$entityBody = file_get_contents ( 'php://input' );
$entityBodyJSON = json_decode($entityBody);

if(!isset($entityBodyJSON->userID) 
    || !isset($entityBodyJSON->accessToken) 
    || (!isset($entityBodyJSON->alias) && !isset($entityBodyJSON->name) && !isset($entityBodyJSON->aliasname)) 
    || empty(trim($entityBodyJSON->userID))
    || empty(trim($entityBodyJSON->accessToken))
    )
{
    $fields = ['fields' => ['userID','accessToken','name','alias']];
    $replyToSender = json_encode(msg(0,422,'Please Fill in all Required Fields!',$fields));		
}
else
{
	if(isset($entityBodyJSON->aliasname))
	{
		$alias = $entityBodyJSON->aliasname;	
	}
	else
	{
		$alias = isset($entityBodyJSON->alias) ? $entityBodyJSON->alias : "";	
	}
	
	$name = isset($entityBodyJSON->name) ? $entityBodyJSON->name : "";	
		
	$replyToSender = json_encode ( getDeviceDataValues($entityBodyJSON->accessToken, $alias, $name) );
}

header ( 'Content-Type: application/json' );
echo $replyToSender;
return;
?>
