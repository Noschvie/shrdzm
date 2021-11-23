<?php
require __DIR__.'/logging.php';
require __DIR__.'/classes/Database.php';

function msg($success,$status,$message,$extra = []){
    return array_merge([
        'success' => $success,
        'status' => $status,
        'message' => $message
    ],$extra);
}

$returnData = [
    "success" => 0,
    "status" => 401,
    "message" => "Unauthorized",
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

function GetLastDeviceData($token, $alias, $name, $count)
{
	$db_connection = new Database();
	$conn = $db_connection->dbConnection();
	$dataFound = false;

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
			
			
			$query_devices = $conn->prepare($fetch_devices_from_id);
			$query_devices->execute();

			if($query_devices->rowCount())
			{
//				$returnDataString = '{ "success" : 1, "status" : 201, "message" : "OK", "data" : [';
				$returnDataString = '{ "data" : [';

				$counter = 0;
								
				while ($row = $query_devices->fetch(PDO::FETCH_ASSOC))				
				{
					// get parameters from found devices`
					$deviceDataParameterList = getDeviceDataParameterList($conn, $row['type']);
					
					foreach ($deviceDataParameterList as $para) 
					{
						$fetch_last_value_from_device = 'SELECT * FROM `data` WHERE `name`="'.$row['name'].'" AND reading="'.$para.'" order by timestamp desc limit '.strval($count);
						$query_last_value_from_device = $conn->prepare($fetch_last_value_from_device);
						$query_last_value_from_device->execute();

						if($query_last_value_from_device->rowCount())
						{
							$dataFound = true;
							
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
								
									$returnDataString .= '{"timestamp" : "'.$rowValue['timestamp'].'", "name" : "'.$row['name'].'", "alias" : "'.$row['alias'].'", "reading" : "'.$rowValue['reading'].'", "value" : "'.$rowValue['value'].'", "unit" : "'.$rowParameter['unit'].'", "unit_native" : "'.$rowParameter['unit_native'].'", "desc" : "'.$rowParameter['alias_de'].'"}';								
								}
							}						
						}					
					}
				}		

				if($dataFound == true)
					$returnDataString .= '], "success" : 1, "status" : 201, "message" : "OK" }';
				else
					$returnDataString .= '], "success" : 0, "status" : 408, "message" : "no data" }';

//				$returnDataString .= "]}";
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
$allHeaders = getallheaders();

if(array_key_exists('Authorization',$allHeaders) && !empty(trim($allHeaders['Authorization'])))
{
	$token = explode(" ", trim($allHeaders['Authorization']));
		
	if(!isset($entityBodyJSON->userID) 
		|| !isset($entityBodyJSON->count) 
		|| (!isset($entityBodyJSON->alias) && !isset($entityBodyJSON->name)) 
		|| empty(trim($entityBodyJSON->userID))
		)
	{
		$fields = ['fields' => ['userID','name','alias','count']];
		$replyToSender = json_encode(msg(0,422,'Please Fill in all Required Fields!',$fields));		
	}
	else
	{
		$alias = isset($entityBodyJSON->alias) ? $entityBodyJSON->alias : "";	
		$name = isset($entityBodyJSON->name) ? $entityBodyJSON->name : "";	
	
		logging2file("alias = ".$alias);
		logging2file("name = ".$name);
	
		$replyToSender = json_encode ( GetLastDeviceData($token[1], $alias, $name, $entityBodyJSON->count) );
	}
}
else
{
	$replyToSender = json_encode ( msg(0, 401, "Unauthorized") );	
}

header ( 'Content-Type: application/json' );
echo $replyToSender;
return;
?>