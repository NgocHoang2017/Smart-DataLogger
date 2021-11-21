<?php


// Create connection
$conn = new mysqli("localhost","root","","thuthapdulieu");

if ($conn->connect_error) {

 die("Connection failed: " . $conn->connect_error);
}

$sql = "SELECT * FROM data"; // truy xuat bang data trong thuthapdulieu

$result = $conn->query($sql);

if ($result->num_rows >0) {


 while($row[] = $result->fetch_assoc()) {

 $tem = $row;

 $json = json_encode($tem);

 }

} else {
 echo "No Results Found.";
}
 echo $json;
$conn->close();
?>
