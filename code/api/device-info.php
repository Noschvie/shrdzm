<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Headers: access");
header("Access-Control-Allow-Methods: POST");
header("Content-Type: application/json; charset=UTF-8");
header("Access-Control-Allow-Headers: Content-Type, Access-Control-Allow-Headers, Authorization, X-Requested-With");

function msg($success,$status,$message,$extra = []){
    return array_merge([
        'success' => $success,
        'status' => $status,
        'message' => $message
    ],$extra);
}

require __DIR__.'/classes/Database.php';
require __DIR__.'/classes/logging.php';
require __DIR__.'/fetchUserByToken.php';

$allHeaders = getallheaders();
$db_connection = new Database();
$conn = $db_connection->dbConnection();
$returnData = [
    "success" => 0,
    "status" => 401,
    "message" => "Unauthorized"
];

// GET DATA FORM REQUEST
$data = json_decode(file_get_contents("php://input"));

// IF REQUEST METHOD IS NOT POST
if($_SERVER["REQUEST_METHOD"] != "POST")
{
    $returnData = msg(0,404,'Page Not Found!');
}
elseif(!isset($data->name) || empty($data->name))
{
    $fields = ['fields' => ['name']];
    $returnData = msg(0,422,'Please Fill in all Required Fields!',$fields);
}
else
{
	if(array_key_exists('Authorization',$allHeaders) && !empty(trim($allHeaders['Authorization'])))
	{
		$token = explode(" ", trim($allHeaders['Authorization']));
		
		if(isset($token[1]) && !empty(trim($token[1])))
		{
			$userInfo = fetchUserByToken($conn, trim($token[1]));
			if(isset($userInfo))
			{
				$devicename = $data->name;				
		
				try
				{
					$check_device = "SELECT * FROM `devices` WHERE `name`=:devicename";
					$check_device_stmt = $conn->prepare($check_device);
					$check_device_stmt->bindValue(':devicename', $devicename,PDO::PARAM_STR);
					$check_device_stmt->execute();
					
					if($check_device_stmt->rowCount())
					{						
						$row = $check_device_stmt->fetch(PDO::FETCH_ASSOC);
						
						if($row["userid"] == $userInfo["user"]["id"])						
							$returnData = msg(1,200,'Device exist',$userInfo);
						else
							$returnData = msg(1,42,'Unknown device'); 
					}
					else
					{
						$returnData = msg(1,42,'Unknown device');
					}
				}
				catch(PDOException $e){
					$returnData = msg(0,500,$e->getMessage());
				}			
			}
		}
	}
}

echo json_encode($returnData);