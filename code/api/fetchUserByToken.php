<?php

function fetchUserByToken($db, $token)
{
	try
	{
		$fetch_user_by_id = "SELECT `name`,`email`,`id` FROM `users` WHERE `alexa_userid`='".$token."'";

		$query_stmt = $db->prepare($fetch_user_by_id);
		$query_stmt->execute();

		if($query_stmt->rowCount())
		{
			$row = $query_stmt->fetch(PDO::FETCH_ASSOC);

			return [
				'success' => 1,
				'status' => 200,
				'user' => $row
			];
		}
		else
		{
			logging2file( "Wrong token : ".$token);
			return null;
		}
	}
	catch(PDOException $e){
		return null;
	}
}

?>