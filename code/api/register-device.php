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
require __DIR__.'/middlewares/Auth.php';

$allHeaders = getallheaders();
$db_connection = new Database();
$conn = $db_connection->dbConnection();
$auth = new Auth($conn,$allHeaders);
$returnData = [
    "success" => 0,
    "status" => 401,
    "message" => "Unauthorized"
];

// GET DATA FORM REQUEST
$data = json_decode(file_get_contents("php://input"));

// IF REQUEST METHOD IS NOT POST
if($_SERVER["REQUEST_METHOD"] != "POST"):
    $returnData = msg(0,404,'Page Not Found!');
	
// CHECKING EMPTY FIELDS
elseif(!isset($data->name) 
    || !isset($data->type) 
    || empty(trim($data->name))
    || empty(trim($data->type))
    ):

    $fields = ['fields' => ['name','type']];
    $returnData = msg(0,422,'Please Fill in all Required Fields!',$fields);	

else:
	if($auth->isAuth())
	{
		$auth1 = $auth->isAuth();

		$name = trim($data->name);
		$type = trim($data->type);
		$userid = $auth1["user"]["id"];

        try
		{
            $check_device = "SELECT `name` FROM `devices` WHERE `name`=:name";
            $check_device_stmt = $conn->prepare($check_device);
            $check_device_stmt->bindValue(':name', $name,PDO::PARAM_STR);
            $check_device_stmt->execute();
			
            if($check_device_stmt->rowCount()):
                $update_query = "UPDATE `devices` SET `userid` = :userid, `type` = :type WHERE `name` = :name";

                $update_stmt = $conn->prepare($update_query);

                // DATA BINDING
                $update_stmt->bindValue(':name', htmlspecialchars(strip_tags($name)),PDO::PARAM_STR);
                $update_stmt->bindValue(':type', htmlspecialchars(strip_tags($type)),PDO::PARAM_STR);
                $update_stmt->bindValue(':userid', $userid,PDO::PARAM_STR);

                $update_stmt->execute();

                $returnData = msg(1,201,'Device successfully updated.');
			else:
                $insert_query = "INSERT INTO `devices`(`name`,`userid`,`type`) VALUES(:name,:userid,:type)";

                $insert_stmt = $conn->prepare($insert_query);

                // DATA BINDING
                $insert_stmt->bindValue(':name', htmlspecialchars(strip_tags($name)),PDO::PARAM_STR);
                $insert_stmt->bindValue(':type', htmlspecialchars(strip_tags($type)),PDO::PARAM_STR);
                $insert_stmt->bindValue(':userid', $userid,PDO::PARAM_STR);

                $insert_stmt->execute();

                $returnData = msg(1,201,'Device successfully registered.');
            endif;
		}
        catch(PDOException $e){
            $returnData = msg(0,500,$e->getMessage());
        }			
	}
endif;

echo json_encode($returnData);