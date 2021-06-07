<?php

function fetchUserByToken($db, $token)
{
	try
	{
		$fetch_user_by_id = "SELECT `name`,`email`,`id` FROM `users` WHERE `alexa_userid`=:alexa_userid";

		$query_stmt = $db->prepare($fetch_user_by_id);
		$query_stmt->bindValue(':alexa_userid', $token,PDO::PARAM_INT);
		$query_stmt->execute();

		if($query_stmt->rowCount()):
			$row = $query_stmt->fetch(PDO::FETCH_ASSOC);
			return [
				'success' => 1,
				'status' => 200,
				'user' => $row
			];
		else:
			return null;
		endif;
	}
	catch(PDOException $e){
		return null;
	}
}

?>