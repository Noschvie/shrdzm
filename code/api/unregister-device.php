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
elseif(!isset($data->name) 
    || empty(trim($data->name))
    )
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
				$name = trim($data->name);
				
				try
				{
					$check_device = "SELECT `name` FROM `devices` WHERE `name`=:name";
					$check_device_stmt = $conn->prepare($check_device);
					$check_device_stmt->bindValue(':name', $name,PDO::PARAM_STR);
					$check_device_stmt->execute();
					
					if($check_device_stmt->rowCount()):
						$delete_query = "DELETE from `devices` WHERE `name`=:name";
						$delete_stmt = $conn->prepare($delete_query);
						$delete_stmt->bindValue(':name', htmlspecialchars(strip_tags($name)),PDO::PARAM_STR);
						$delete_stmt->execute();

						$returnData = msg(1,201,'You have successfully unregistered the device.');
					else:
						$returnData = msg(1,202,'Device was already deleted.');
					endif;
				}
				catch(PDOException $e){
					$returnData = msg(0,500,$e->getMessage());
				}			
			}
		}
	}
}

echo json_encode($returnData);