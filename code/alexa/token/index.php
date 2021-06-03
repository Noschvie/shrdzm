<?php
	header("Content-Type: application/json; charset=UTF-8");

	$grant_type = $_POST["grant_type"];
	$code = $_POST["code"];
	$client_id = $_POST["client_id"];
		
	$returnData = '{"access_token":"'.$code.'","token_type":"bearer","expires_in":3600,"refresh_token":"'.$code.'"}';

	echo $returnData;
?>