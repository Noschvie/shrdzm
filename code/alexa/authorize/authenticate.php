<?php
	$state = $_POST["state"];
	$client_id = $_POST["client_id"];
	$redirect_uri = $_POST["redirect_uri"];
	$username = $_POST["username"];
	$password = $_POST["password"];
	
	require __DIR__.'/classes/Database.php';
	require __DIR__.'/classes/JwtHandler.php';
	
	$db_connection = new Database();
	$conn = $db_connection->dbConnection();
	$code = "EMPTY";	
	
	try{
		
		$fetch_user_by_name = "SELECT * FROM `users` WHERE `name`=:username";
		$query_stmt = $conn->prepare($fetch_user_by_name);
		$query_stmt->bindValue(':username', $username,PDO::PARAM_STR);
		$query_stmt->execute();

		// IF THE USER IS FOUNDED BY NAME
		if($query_stmt->rowCount()):
			$row = $query_stmt->fetch(PDO::FETCH_ASSOC);
			$check_password = password_verify($password, $row['password']);

			// VERIFYING THE PASSWORD (IS CORRECT OR NOT?)
			// IF PASSWORD IS CORRECT THEN SEND THE LOGIN TOKEN
			if($check_password):

				$jwt = new JwtHandler();
				$token = $jwt->_jwt_encode_data(
					'http://localhost/php_auth_api/',
					array("user_id"=> $row['id'])
				);
				
				$code = $token;
				
				// STORE alexa user id
                $update_query = "UPDATE `users` SET `alexa_userid` = :client_id WHERE `name` = :username";

                $update_stmt = $conn->prepare($update_query);

                // DATA BINDING
                $update_stmt->bindValue(':username', htmlspecialchars(strip_tags($username)),PDO::PARAM_STR);
                $update_stmt->bindValue(':client_id', $client_id,PDO::PARAM_STR);

                $update_stmt->execute();
			else:
				echo "Invalid password";
				exit;
			endif;

		// IF THE USER IS NOT FOUNDED BY NAME THEN SHOW THE FOLLOWING ERROR
		else:
			echo "User not found";
			exit;
		endif;
	}
	catch(PDOException $e)
	{
		echo "Database connection broken";
		exit;
	}
			
	$url = $redirect_uri."?state=".$state."&code=".$code;
	
//	echo $url;
	header("Location: $url");	
?>
