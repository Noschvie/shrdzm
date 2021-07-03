<?php
require __DIR__.'/logging.php';
require __DIR__.'/classes/Database.php';


const appID = 'amzn1.ask.skill.3f009f9d-ad83-49fc-b4f8-d2d162417f45';

class Reply {
    public $appID = "";
    public $antwort  = "";
	public $state = "NOK";
}


function DeviceList($token)
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
					if($returnData != "")
						$returnData .= ", ";
						
					if($row['alias'] != "")
						$returnData .= $row['alias']." vom typ ".$row['type'];
					else						
						$returnData .= $row['name']." vom typ ".$row['type'];
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
}
else
{
	$reply->state = 'OK';	

	$reply->antwort = DeviceList($entityBodyJSON->accessToken);			
}

$replyToSender = json_encode ( $reply );

header ( 'Content-Type: application/json' );
echo $replyToSender;
return;
?>
