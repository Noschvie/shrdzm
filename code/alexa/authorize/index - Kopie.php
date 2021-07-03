<h1>SHRDZM Alexa Account Linking</h1>
<?php
	$client_id = $_GET["client_id"];
	$redirect_uri = $_GET["redirect_uri"];
	$state = $_GET["state"];
	$response_type = $_GET["response_type"];
	$scope = $_GET["scope"];

	require __DIR__.'/middlewares/Auth.php';

	if(isset($_POST['username']) && $_POST['username'] == $username && $_POST['password'] == $password)
	{
		$code = "CA7eaHjIHz5NxeIJoFK9krqaeZrPLwmMmgI_XiQiIkQ";
		$url = $redirect_uri."?state=".$state."&code=".$code;
		
		header("Location: $url");
	}


/*	$code = "CA7eaHjIHz5NxeIJoFK9krqaeZrPLwmMmgI_XiQiIkQ";
	$url = $redirect_uri."?state=".$state."&code=".$code;
	header("Location: $url");

//	echo $url;

	exit; */
?>