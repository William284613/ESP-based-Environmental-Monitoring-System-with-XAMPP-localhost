<?php

// Database configuration
$servername = "localhost"; // Database server name
$username = "root"; // Database username
$password = ""; // Database password
$dbname = "environment_monitor"; // Database name

// Data received from ESP8266
$temperature = $_POST['temperature']; // Temperature value from POST request
$humidity = $_POST['humidity']; // Humidity value from POST request
$co2 = $_POST['co2']; // CO2 value from POST request
$air_quality = $_POST['air_quality']; // Air quality value from POST request

try {
    // Create connection
    $conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
    
    // Set the PDO error mode to exception
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    
    // SQL query to check if a record exists for the current timestamp
    $stmt_check = $conn->prepare("SELECT id FROM sensor_data WHERE timestamp = :timestamp");
    $stmt_check->bindParam(':timestamp', $timestamp);
    
    // Execute the check query
    $timestamp = date('Y-m-d H:i:s');  // Current timestamp
    $stmt_check->execute();
    
    // Fetch the existing record if available
    $existing_record = $stmt_check->fetch(PDO::FETCH_ASSOC);
    
    if ($existing_record) {
        // Update the existing record if a record with the same timestamp exists
        $stmt_update = $conn->prepare("
            UPDATE sensor_data 
            SET temperature = IF(:temperature != -1, :temperature, temperature),
                humidity = IF(:humidity != -1, :humidity, humidity),
                co2 = IF(:co2 != -1, :co2, co2),
                air_quality = IF(:air_quality != '', :air_quality, air_quality)
            WHERE id = :id
        ");
        
        // Bind parameters for update
        $stmt_update->bindParam(':id', $existing_record['id']);
        $stmt_update->bindParam(':temperature', $temperature);
        $stmt_update->bindParam(':humidity', $humidity);
        $stmt_update->bindParam(':co2', $co2);
        $stmt_update->bindParam(':air_quality', $air_quality);
        
        // Execute the update query
        $stmt_update->execute();
    } else {
        // Insert a new record if no record with the same timestamp exists
        $stmt_insert = $conn->prepare("
            INSERT INTO sensor_data (timestamp, temperature, humidity, co2, air_quality) 
            VALUES (:timestamp, :temperature, :humidity, :co2, :air_quality)
        ");
        
        // Bind parameters for insert
        $stmt_insert->bindParam(':timestamp', $timestamp);
        $stmt_insert->bindParam(':temperature', $temperature);
        $stmt_insert->bindParam(':humidity', $humidity);
        $stmt_insert->bindParam(':co2', $co2);
        $stmt_insert->bindParam(':air_quality', $air_quality);
        
        // Execute the insert query
        $stmt_insert->execute();
    }
    
    // Close connection
    $conn = null; // Close the database connection
    
    // Respond with success message
    echo "Data submitted successfully"; // Send success message
} catch(PDOException $e) {
    // Display error message if connection or query fails
    echo "Error: " . $e->getMessage(); // Print error message
}
?>
