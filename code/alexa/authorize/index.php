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
	</head>
	<body>
		<div class="login">
			<h1>Login</h1>
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