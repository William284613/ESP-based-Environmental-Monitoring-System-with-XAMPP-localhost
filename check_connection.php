<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "environment_monitor";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} else {
    echo "Connected successfully to database!";
}

$conn->close();
?>
