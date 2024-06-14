//1.start server 
//2.create database name: environment_monitor in xampp
//3.import .sql file

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MQ135.h>
#include "DHT.h"
#include "indexpage.h"

#define DHTTYPE DHT11
#define LEDonBoard 2
#define MQ_PIN A0
#define co2Zero 70

const char* ssid = "NgFamily";  // WiFi SSID
const char* password = "Qwerty@14321";  // WiFi Password

ESP8266WebServer server(80);  // Create a web server on port 80
const int DHTPin = 2;  // Pin where DHT sensor is connected
DHT dht(DHTPin, DHTTYPE);  // Initialize DHT sensor
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Initialize LCD with I2C address 0x27
MQ135 gasSensor(MQ_PIN);  // Initialize MQ135 gas sensor on analog pin A0

const int GOOD_THRESHOLD = 500;  // Threshold for good air quality
const int MODERATE_THRESHOLD = 700;  // Threshold for moderate air quality

// Function to classify CO2 concentration
String classifyCO2Concentration(int concentration) {
  if (concentration <= GOOD_THRESHOLD) {
    return "Good";
  } else if (concentration <= MODERATE_THRESHOLD) {
    return "Moderate";
  } else {
    return "Danger";
  }
}

// Handler for the root URL
void handleRoot() {
  String s = MAIN_page;  // HTML content from indexpage.h
  server.send(200, "text/html", s);  // Send HTML content to the client
}

// Handler to read and send temperature from DHT11 sensor
void handleDHT11Temperature() {
  float t = dht.readTemperature();  // Read temperature from DHT sensor
  if (!isnan(t)) {  // Check if reading is valid
    String Temperature_Value = String(t);  // Convert temperature to string
    server.send(200, "text/plain", Temperature_Value);  // Send temperature value to the client
    lcd.clear();  // Clear LCD display
    Serial.print("DHT11 || Temperature : ");
    Serial.print(t);
    Serial.print(" || ");
    lcd.setCursor(0, 0);  // Set cursor to first row
    lcd.print("Temp: ");
    lcd.print(t);
    lcd.print("C");
    sendDataToServer(t, -1, -1, "");  // Send temperature data to server
  } else {
    server.send(500, "text/plain", "Nan");  // Send error message to client
    Serial.println("Failed to read from DHT sensor!");
  }
}

// Handler to read and send humidity from DHT11 sensor
void handleDHT11Humidity() {
  float h = dht.readHumidity();  // Read humidity from DHT sensor
  if (!isnan(h)) {  // Check if reading is valid
    String Humidity_Value = String(h);  // Convert humidity to string
    server.send(200, "text/plain", Humidity_Value);  // Send humidity value to the client
    Serial.print("Humidity: ");
    Serial.println(h);
    lcd.setCursor(0, 1);  // Set cursor to second row
    lcd.print("Humi: ");
    lcd.print(h);
    lcd.print("%");
    sendDataToServer(-1, h, -1, "");  // Send humidity data to server
  } else {
    server.send(500, "text/plain", "Nan");  // Send error message to client
    Serial.println("Failed to read from DHT sensor!");
  }
}

// Handler to read and send CO2 concentration from MQ135 sensor
void handleGasSensor() {
  int co2now[10];  // Array to store multiple readings
  int co2raw = 0;
  int co2ppm = 0;
  int zzz = 0;

  for (int x = 0; x < 10; x++) {
    co2now[x] = analogRead(MQ_PIN);  // Read analog value from MQ135 sensor
    delay(200);  // Delay between readings
  }

  for (int x = 0; x < 10; x++) {
    zzz += co2now[x];  // Sum of all readings
  }

  co2raw = zzz / 10;  // Average of readings
  co2ppm = co2raw - co2Zero;  // Adjust with zero offset
  String gasClassification = classifyCO2Concentration(co2ppm);  // Classify air quality
  server.send(200, "text/plain", String(co2ppm) + "," + gasClassification);  // Send CO2 and classification to client

  Serial.print("Gas: ");
  Serial.print(co2ppm);
  Serial.print(" PPM");
  Serial.print(", Air Quality :");
  Serial.println(gasClassification + "\n");

  lcd.clear();  // Clear LCD display
  lcd.setCursor(0, 0);  // Set cursor to first row
  lcd.print("Gas: ");
  lcd.print(co2ppm);
  lcd.print(" PPM");
  lcd.setCursor(0, 1);  // Set cursor to second row
  lcd.print("Quality:");
  lcd.print(gasClassification);
  delay(2000);  // Delay to allow display update
  sendDataToServer(-1, -1, co2ppm, gasClassification);  // Send CO2 data to server
}

// Function to send sensor data to server
void sendDataToServer(float temperature, float humidity, int co2, String airQuality) {
  if (WiFi.status() == WL_CONNECTED) {  // Check if connected to WiFi
    WiFiClient client;
    HTTPClient http;
    String serverPath = "http://192.168.1.5/ESP-based-Monitoring-System/submit_data.php";
    Serial.print("Sending request to: ");
    Serial.println(serverPath);

    // Construct the POST request body
    String requestBody = "temperature=" + String(temperature) + "&humidity=" + String(humidity) + "&co2=" + String(co2) + "&air_quality=" + airQuality;

    http.begin(client, serverPath);  // Specify the URL and endpoint of your server
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Send the POST request
    int httpResponseCode = http.POST(requestBody);

    // Check for response
    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.println("Received response:");
      Serial.println(payload);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      Serial.print("Error message: ");
      Serial.println(http.errorToString(httpResponseCode).c_str());
    }

    // End HTTP connection
    http.end();
  } else {
    Serial.println("WiFi not connected.");
  }
}

// Handler to fetch last readings from server
void handleFetchLastReadings() {
  WiFiClient client;
  HTTPClient http;
  String serverPath = "http://192.168.1.5/ESP-based-Monitoring-System/fetch_last_readings.php";
  Serial.print("Fetching data from: ");
  Serial.println(serverPath);

  http.begin(client, serverPath);  // Start connection to server
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    String payload = http.getString();
    Serial.println("Received data:");
    Serial.println(payload);
    server.send(200, "application/json", payload);  // Send JSON response to the client
  } else {
    Serial.print("Error fetching data. Error code: ");
    Serial.println(httpResponseCode);
    server.send(500, "text/plain", "Error fetching data");
  }
  http.end();  // Close connection
}

void setup() {
  lcd.init();  // Initialize LCD
  lcd.backlight();  // Turn on LCD backlight
  lcd.clear();  // Clear LCD display
  Serial.begin(115200);  // Start serial communication at 115200 baud
  delay(500);
  dht.begin();  // Initialize DHT sensor
  delay(500);
  pinMode(LEDonBoard, OUTPUT);  // Set LED pin as output
  digitalWrite(LEDonBoard, HIGH);  // Turn off LED
  WiFi.begin(ssid, password);  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  lcd.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {  // Wait until connected to WiFi
    Serial.print(".");
    digitalWrite(LEDonBoard, LOW);  // Blink LED
    delay(250);
    digitalWrite(LEDonBoard, HIGH);  // Blink LED
    delay(250);
  }
  Serial.println("");
  Serial.println("Successfully connected to WiFi!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  lcd.clear();  // Clear LCD display
  lcd.print("Connected!");  // Print connection message
  lcd.setCursor(0, 1);  // Set cursor to second row
  lcd.print("IP: ");
  lcd.print(WiFi.localIP());
  server.on("/", handleRoot);  // Define handler for root URL
  server.on("/readTemperature", handleDHT11Temperature);  // Define handler for temperature reading
  server.on("/readHumidity", handleDHT11Humidity);  // Define handler for humidity reading
  server.on("/readGasSensor", handleGasSensor);  // Define handler for gas sensor reading
  server.on("/fetchLastReadings", handleFetchLastReadings);  // Define handler to fetch last readings

  server.begin();  // Start the server
  Serial.println("HTTP server started");
  delay(5000);
  lcd.init();  // Reinitialize LCD
  lcd.backlight();  // Turn on LCD backlight
  lcd.clear();  // Clear LCD display
}

void loop() {
  server.handleClient();  // Handle client requests
}
