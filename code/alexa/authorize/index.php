<?php
	$client_id = $_GET["client_id"];
	$redirect_uri = $_GET["redirect_uri"];
	$state = $_GET["state"];
	$response_type = $_GET["response_type"];
	$scope = $_GET["scope"];
?>

<!DOCTYPE html>
<html>
	<head>
		<meta charset="utf-8">
		<title>Login</title>
		<link href="style.css" rel="stylesheet" type="text/css">
		<link rel="icon" type="image/svg+xml" href="https://shrdzm.pintarweb.net/Logo_min_blue.svg" sizes="any">	
		<link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.1/css/all.css">
	</head>
	<body>
		<div class="login">
			<h1>SHRDZM Login</h1>
			<h2><center>for Alexa Account Linking</center></h2>
			<form action="authenticate.php" method="post">
<?php
				echo "<input type='hidden' id='client_id' name='client_id' value='".$client_id."'/>";
				echo "<input type='hidden' id='state' name='state' value='".$state."'/>";
				echo "<input type='hidden' id='redirect_uri' name='redirect_uri' value='".$redirect_uri."'/>";
?>
				<label for="username">
					<i class="fas fa-user"></i>
				</label>
				<input type="text" name="username" placeholder="Username" id="username" required>
				<label for="password">
					<i class="fas fa-lock"></i>
				</label>
				<input type="password" name="password" placeholder="Password" id="password" required>
				<input type="submit" value="Login">
			</form>
		</div>
	</body>
</html>