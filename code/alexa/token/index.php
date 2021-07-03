<?php
	include("../logging.php");
	require __DIR__.'/../authorize/classes/Database.php';
	require __DIR__.'/../authorize/classes/JwtHandler.php';

	header("Content-Type: application/json; charset=UTF-8");

	$code = "";
	$refresh_token = "";
	
	if (isset($_POST['code']) )	
		$code = $_POST["code"];
		
	if (isset($_POST['refresh_token']) )	
		$refresh_token = $_POST["refresh_token"];

	if($refresh_token != "" && substr($refresh_token, 0,2) == "r_")
	{
		$code = substr($refresh_token, 2);
	}
		
	$db_connection = new Database();
	$conn = $db_connection->dbConnection();

	try{
		
		$fetch_user_by_token = 'SELECT * FROM `users` WHERE `alexa_userid`="'.$code.'"';
		$query_stmt = $conn->prepare($fetch_user_by_token);
		$query_stmt->execute();

		// IF THE USER IS FOUNDED BY NAME
		if($query_stmt->rowCount())
		{
            $row = $query_stmt->fetch(PDO::FETCH_ASSOC);	

/*			$jwt = new JwtHandler();
			$token = $jwt->_jwt_encode_data(
				'http://localhost/php_auth_api/',
				array("user_id"=> $row['id'])
			);
			
			$codeNew = $token;
			
			$update_query = 'UPDATE `users` SET `alexa_userid` = "'.$codeNew.'" WHERE `id` = "'.$row['id'].'"';
		loging2file($update_query);

			$update_stmt = $conn->prepare($update_query);


			$update_stmt->execute();*/
			$codeNew = $code;
			loging2file('new token for '.$row["name"].' generated');
		}
	}
	catch(PDOException $e)
	{
		return "error";
	}	

		
	$returnData = '{"access_token":"'.$codeNew.'","token_type":"bearer","expires_in":3600,"refresh_token":"r_'.$codeNew.'"}';

//	loging2file('new token generated (old='.$code.' ,new='.$codeNew.')');

	echo $returnData;
?>