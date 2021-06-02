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

// INCLUDING DATABASE AND MAKING OBJECT
require __DIR__.'/classes/Database.php';
$db_connection = new Database();
$conn = $db_connection->dbConnection();

// GET DATA FORM REQUEST
$data = json_decode(file_get_contents("php://input"));
$returnData = [];

// IF REQUEST METHOD IS NOT POST
if($_SERVER["REQUEST_METHOD"] != "POST"):
    $returnData = msg(0,404,'Page Not Found!');

// CHECKING EMPTY FIELDS
elseif(!isset($data->name) 
    || !isset($data->password)
    || empty(trim($data->name))
    || empty(trim($data->password))
    ):

    $fields = ['fields' => ['name','password']];
    $returnData = msg(0,422,'Please Fill in all Required Fields!',$fields);

// IF THERE ARE NO EMPTY FIELDS THEN-
else:
    
    $name = trim($data->name);
    $password = trim($data->password);

    if(strlen($password) < 8):
        $returnData = msg(0,422,'Your password must be at least 8 characters long!');

    elseif(strlen($name) < 3):
        $returnData = msg(0,422,'Your name must be at least 3 characters long!');

    else:
        try{
            $fetch_user_by_name = "SELECT * FROM `users` WHERE `name`=:name";
            $query_stmt = $conn->prepare($fetch_user_by_name);
            $query_stmt->bindValue(':name', $name,PDO::PARAM_STR);
            $query_stmt->execute();

            // IF THE USER IS FOUNDED BY NAME
            if($query_stmt->rowCount()):
                $row = $query_stmt->fetch(PDO::FETCH_ASSOC);
                $check_password = password_verify($password, $row['password']);

                // VERIFYING THE PASSWORD (IS CORRECT OR NOT?)
                // IF PASSWORD IS CORRECT THEN SEND THE LOGIN TOKEN
                if($check_password):						
					$delete_query = "DELETE from `users` WHERE `name`=:name";
					$delete_stmt = $conn->prepare($delete_query);
					$delete_stmt->bindValue(':name', htmlspecialchars(strip_tags($name)),PDO::PARAM_STR);
					$delete_stmt->execute();

					$returnData = msg(1,201,'You have successfully unregistered.');
                else:
                    $returnData = msg(0,422,'Invalid Password!');					
				endif;
            else:
                $returnData = msg(0,422,'Invalid Name!');				
			endif;
        }
        catch(PDOException $e){
            $returnData = msg(0,500,$e->getMessage());
        }
    endif;
    
endif;

echo json_encode($returnData);