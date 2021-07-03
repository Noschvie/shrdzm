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
    || !isset($data->reading) 
    || !isset($data->value) 
    || empty(trim($data->name))
    || empty(trim($data->reading))
    || empty(trim($data->value))
    )
{

    $fields = ['fields' => ['name','reading','value']];
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
				$reading = trim($data->reading);
				$value = trim($data->value);

				try
				{
					$insert_query = "INSERT INTO `data`(`name`,`reading`,`value`) VALUES(:name,:reading,:value)";

					$insert_stmt = $conn->prepare($insert_query);

					// DATA BINDING
					$insert_stmt->bindValue(':name', htmlspecialchars(strip_tags($name)),PDO::PARAM_STR);
					$insert_stmt->bindValue(':reading', htmlspecialchars(strip_tags($reading)),PDO::PARAM_STR);
					$insert_stmt->bindValue(':value', $value,PDO::PARAM_STR);

					$insert_stmt->execute();

					$returnData = msg(1,201,'Measurement added.');
				}
				catch(PDOException $e){
					$returnData = msg(0,500,$e->getMessage());
				}			
			}
			else
			{
                $returnData = msg(0,422,'Invalid Name!');				
			}
		}
	}
}

echo json_encode($returnData);
?>