<?php
	require_once('dbConnect.php');
	mysqli_set_charset($con,'utf8');
	/** Array for JSON response*/
	$response = array();
	if($_SERVER['REQUEST_METHOD']=='POST'){
		$username = $_POST['username'];
		$password = $_POST['password'];
		$sql = "SELECT user_name, email FROM account WHERE user_name = '$username' AND password='$password'";
		if(mysqli_num_rows(@mysqli_query($con,$sql)) > 0){
			    $result= mysqli_query($con,$sql);
                $row = mysqli_fetch_array($result);
				$user_name = $row["user_name"];
				$email = $row["email"];

				$response["success"] = 1;
				$response["message"] = "Đăng nhập thành công";
				$response["user_name"] = $user_name;
				$response["email"] = $email;
		}else{
			$response["success"] = 0;
			$response["message"] = "Đăng nhập thất bại.";
		}
		/**Return json*/
		echo json_encode($response);
	}
?>
