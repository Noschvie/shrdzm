<h1>SHRDZM Alexa Account Linking</h1>
<?php
	$clientid = $_GET["client_id"];
	$redirect = $_GET["redirect_uri"];
	$state = $_GET["state"];

	

	$access_token = "CA7eaHjIHz5NxeIJoFK9krqaeZrPLwmMmgI_XiQiIkQ";
	$url = $redirect."#state=".$state."&access_token=".$access_token."&token_type=Bearer";
//	header("Location: $url");

	echo $url;

	exit;
?>