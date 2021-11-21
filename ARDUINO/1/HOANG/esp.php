<?php
//Creates new record as per request
    //Connect to database
    $servername = "localhost";
    $username = "root";
    $password = "";
    $dbname = "thuthapdulieu";

    // Create connection
    $conn = new mysqli($servername, $username, $password, $dbname);
    // Check connection
    if ($conn->connect_error) {
        die("Database Connection failed: " . $conn->connect_error);
    }

    //Get current date and time


    if(!empty($_POST['nhiet_do']) && !empty($_POST['do_am']))
    {
    	$nhiet_do = $_POST['nhiet_do'];
    	$snhiet_do = $_POST['do_am'];

	    $sql = "INSERT INTO tbl_messages (nhiet_do, do_am)

		VALUES ('".$nhiet_do."', '".$do_am."')";

		if ($conn->query($sql) === TRUE) {
		    echo "OK";
		} else {
		    echo "Error: " . $sql . "<br>" . $conn->error;
		}
	}


	$conn->close();
?>
