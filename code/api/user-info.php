<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Headers: access");
header("Access-Control-Allow-Methods: POST");
header("Content-Type: application/json; charset=UTF-8");
header("Access-Control-Allow-Headers: Content-Type, Access-Control-Allow-Headers, Authorization, X-Requested-With");

require __DIR__.'/classes/logging.php';
require __DIR__.'/classes/Database.php';
require __DIR__.'/fetchUserByToken.php';

function msg($success,$status,$message,$extra = []){
    return array_merge([
        'success' => $success,
        'status' => $status,
        'message' => $message
    ],$extra);
}

$allHeaders = getallheaders();
$db_connection = new Database();
$conn = $db_connection->dbConnection();

$returnData = [
    "success" => 0,
    "status" => 401,
    "message" => "Unauthorized"
];

if(array_key_exists('Authorization',$allHeaders) && !empty(trim($allHeaders['Authorization'])))
{
	$token = explode(" ", trim($allHeaders['Authorization']));
	if(isset($token[1]) && !empty(trim($token[1])))
	{
		$userInfo = fetchUserByToken($conn, trim($token[1]));
		if(isset($userInfo))
		{
			$returnData = msg(1,201,$userInfo["user"]);
		}
	}
}

echo json_encode($returnData);