<?php

// Database configuration
$servername = "localhost"; // Database server name
$username = "root"; // Database username
$password = ""; // Database password
$dbname = "environment_monitor"; // Database name

// Create connection
try {
    $conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
    // Set the PDO error mode to exception
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    // SQL query to fetch last 10 readings with valid sensor data (exclude -1 values)
    $stmt = $conn->prepare("
        SELECT timestamp, temperature, humidity, co2, air_quality 
        FROM sensor_data 
        WHERE temperature != -1 AND humidity != -1 AND co2 != -1
        ORDER BY timestamp DESC 
        LIMIT 10
    ");
    $stmt->execute(); // Execute the SQL query
    $result = $stmt->fetchAll(PDO::FETCH_ASSOC); // Fetch the result as an associative array

    // Close connection
    $conn = null; // Close the database connection

    // Prepare JSON response
    header('Content-Type: application/json'); // Set the response header to JSON
    echo json_encode($result); // Encode the result as JSON and send it
} catch(PDOException $e) {
    echo "Error: " . $e->getMessage(); // Handle any errors by displaying the error message
}
?>
