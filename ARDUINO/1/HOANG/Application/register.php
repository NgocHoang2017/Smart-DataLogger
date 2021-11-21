<?php
	require_once('dbConnect.php');
	mysqli_set_charset($con,'utf8');
	/** Array for JSON response*/
	$response = array();
	if($_SERVER['REQUEST_METHOD'] == 'POST'){
		$username = $_POST['username'];
		$password = $_POST['password'];
		$email = $_POST['email'];
		$sqlCheck = "SELECT user_name FROM account WHERE user_name = '$username'";
		$result = @mysqli_query($con,$sqlCheck);
		if (mysqli_num_rows($result) != 0) {
				$response["success"] = 0;
				$response["message"] = "Tên đăng nhập đã có người sử dụng!";
			} else {
				$sqlInsert = "INSERT INTO account (user_name,password,email) VALUES ('$username','$password','$email')";
				$resultInsert = @mysqli_query($con,$sqlInsert);
				if ($resultInsert) {
						$sqlGetInfo = "SELECT user_name, email FROM account WHERE user_name = '$username' AND password = '$password'";
						$result = mysqli_query($con,$sqlGetInfo);
						$row = mysqli_fetch_array($result);

						$response["success"] = 1;
						$response["message"] = "Đăng ký thành công!";
						$response["user_name"] = $user_name;
						$response["email"] = $email;
				} else {
					$response["success"] = 0;
					$response["message"] = "Đăng ký thất bại!";
				}
			}
			/**Return json*/
		echo json_encode($response);
	}
?>
