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

function SetDeviceAlias($token, $devicename, $oldalias, $devicealias)
{
	$db_connection = new Database();
	$conn = $db_connection->dbConnection();

	try{
		
		$fetch_user_by_token = 'SELECT id FROM `users` WHERE `alexa_userid`="'.$token.'"';
		$query_stmt = $conn->prepare($fetch_user_by_token);
		$query_stmt->execute();

		// IF THE USER IS FOUNDED BY TOKEN
		if($query_stmt->rowCount())
		{
            $rowUser = $query_stmt->fetch(PDO::FETCH_ASSOC);			
			$fetch_devices_from_id = "";
			
			// search for this device from the user-token
			if($devicename != "")
				$fetch_devices_from_id = 'SELECT name, type, alias FROM `devices` WHERE `userid`="'.$rowUser['id'].'" AND `name`="'.$devicename.'"';
			else if($oldalias != "")
				$fetch_devices_from_id = 'SELECT name, type, alias FROM `devices` WHERE `userid`="'.$rowUser['id'].'" AND `alias`="'.$oldalias.'"';

			$query_devices = $conn->prepare($fetch_devices_from_id);
			$query_devices->execute();
			
			if($query_devices->rowCount() > 0)
			{
				if($devicename != "")
					$update_query = 'UPDATE `devices` SET `alias` = "'.$devicealias.'" WHERE `name` = "'.$devicename.'"';
				else if($oldalias != "")
					$update_query = 'UPDATE `devices` SET `alias` = "'.$devicealias.'" WHERE `alias` = "'.$oldalias.'"';
				
				$update_stmt = $conn->prepare($update_query);
				$update_stmt->execute();

				return msg(1, 201, "Success");				
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

if( !isset($entityBodyJSON->alias) 
    || (!isset($entityBodyJSON->name) && !isset($entityBodyJSON->oldalias))
    )
{
	logging2file($entityBody);
    $fields = ['fields' => ['userID','name','alias']];
    $replyToSender = json_encode(msg(0,422,'Please Fill in all Required Fields!',$fields));		
}
else
{
	if(array_key_exists('Authorization',$allHeaders) && !empty(trim($allHeaders['Authorization'])))
	{
		$token = explode(" ", trim($allHeaders['Authorization']));
		
		$oldalias = "";
		$name = "";
		
		if(isset($entityBodyJSON->oldalias) && !empty($entityBodyJSON->oldalias))
			$oldalias = $entityBodyJSON->oldalias;

		if(isset($entityBodyJSON->name) && !empty($entityBodyJSON->name))
			$name = $entityBodyJSON->name;
		

		$replyToSender = json_encode ( SetDeviceAlias($token[1], $name, $oldalias, $entityBodyJSON->alias) );
	}
}

header ( 'Content-Type: application/json' );
echo $replyToSender;
return;
?>
