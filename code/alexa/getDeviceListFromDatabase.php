<?php
require __DIR__.'/logging.php';
require __DIR__.'/classes/Database.php';


const appID = 'amzn1.ask.skill.3f009f9d-ad83-49fc-b4f8-d2d162417f45';

function msg($success,$status,$message,$extra = [])
{
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
    "antwort" => "Unauthorized",
];

function DeviceList($token)
{
	$db_connection = new Database();
	$conn = $db_connection->dbConnection();

	$messageString = "";

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
			
			$returnDataString = '{ "success" : 1, "status" : 201, "message" : "%message%", "antwort" : "%antwort%", "devices" : [';
			
			if($query_devices->rowCount())
			{
				
				$counter = 0;
				
				while ($row = $query_devices->fetch(PDO::FETCH_ASSOC)) 
				{
					if($counter > 0)
						$returnDataString .= ',';
						
					$counter++;	
						
					$returnDataString .= '{"name" : "'.$row['name'].'", "alias" : "'.$row['alias'].'", "type" : "'.$row['type'].'"}';
					
					if($messageString != "")
						$messageString .= ", ";
						
					if($row['alias'] != "")
						$messageString .= $row['alias']." vom typ ".$row['type'];
					else						
						$messageString .= $row['name']." vom typ ".$row['type'];
				}	
				
			}	

			$returnDataString .= "]}";
			
			$returnDataString = str_replace("%message%", $messageString, $returnDataString);
			$returnDataString = str_replace("%antwort%", $messageString, $returnDataString);

			$returnData = json_decode($returnDataString);
		}
		else
		{
			$returnDataString = '{ "success" : 0, "status" : 201, "message" : "%message%", "antwort" : "%antwort%", "devices" : [';
			
			//return msg(0, 401, "Unauthorized");
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


// check appID
if($entityBodyJSON->appID != appID)
{
	loging2file('WRONG APPID');
	$replyToSender = json_encode(msg(0, 600, "WRONG APPID"));
}
else
{
	$replyToSender = json_encode ( DeviceList($entityBodyJSON->accessToken) );
}

header ( 'Content-Type: application/json' );
echo $replyToSender;
return;
?>
