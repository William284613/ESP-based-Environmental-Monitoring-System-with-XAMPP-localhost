const char MAIN_page[] PROGMEM = R"=====( 
<!DOCTYPE html> 
<html> 
  <head> 
    <meta name="viewport" content="width=device-width, initial-scale=1"> <!-- Responsive design -->
    <link href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" rel="stylesheet"> <!-- Font Awesome for icons -->
    <style>
      html {
        font-family: Arial; /* Set font style */
        display: inline-block;
        margin: 0px auto; /* Center align content */
        text-align: center;
      }
      h1 { font-size: 2.0rem; } /* Main heading size */
      p { font-size: 2.0rem; } /* Paragraph font size */
      .units { font-size: 1.2rem; } /* Units font size */
      .dht-labels{
        font-size: 1.5rem; /* Labels font size */
        vertical-align:middle; 
        padding-bottom: 15px; /* Space below labels */
      }
      table {
        width: 100%; /* Full width table */
        border-collapse: collapse; /* Remove spacing between table cells */
      }
      table, th, td {
        border: 1px solid black; /* Border for table, table header, and table cells */
      }
      th, td {
        padding: 8px; /* Padding inside table cells */
        text-align: center; /* Center align text in cells */
      }
      th {
        background-color: #f2f2f2; /* Header background color */
      }
    </style>
  </head> 
  <body> 
    <h2>Midterm SKIH3113 Sensor Based System (A232)</h2> 
    <h1>ESP-based Environmental Monitoring System <br>with Database Storage</h1> 

    <!-- Display temperature -->
    <p> 
      <i class="fa fa-thermometer-half" style="font-size:3.0rem;color:#62a1d3;"></i> 
      <span class="dht-labels">Temperature : </span> 
      <span id="TemperatureValue">0</span> 
      <sup class="units">&deg;C</sup>
    </p> 
    <!-- Display humidity -->
    <p> 
      <i class="fa fa-tint" style="font-size:3.0rem;color:#75e095;"></i>
      <span class="dht-labels">Humidity : </span> 
      <span id="HumidityValue">0</span> 
      <sup class="units">%</sup> 
    </p> 

    <!-- Display gas concentration -->
    <p> 
       <span class="dht-labels">Gas Concentration : </span> 
       <span id="GasValue">0</span> 
      <sup class="units">ppm</sup> 
    </p> 

    <!-- Display air quality -->
    <p> 
       <span class="dht-labels">Air Quality : </span> 
       <span id="AirQuality">0</span> 
    </p> 

    <!-- Display current time and date -->
    <p> 
      <i class="far fa-clock" style="font-size:1.0rem;color:#e3a8c7;"></i> 
      <span style="font-size:1.0rem;">Time </span> 
      <span id="time" style="font-size:1.0rem;"></span> 

      <i class="far fa-calendar-alt" style="font-size:1.0rem;color:#f7dc68";></i> 
      <span style="font-size:1.0rem;">Date </span> 
      <span id="date" style="font-size:1.0rem;"></span> 
    </p> 

    <h2>Last 10 Readings</h2> 
    <table> 
      <tr> 
        <th>Temperature (&deg;C)</th> 
        <th>Humidity (%)</th> 
        <th>CO2 (ppm)</th> 
        <th>Air Quality</th> 
        <th>Timestamp</th> 
      </tr> 
      <tbody id="data-table"> 
      </tbody> 
    </table> 

    <script> 
  // Function to fetch data periodically 
  setInterval(function() { 
    getTemperatureData(); 
    getHumidityData(); 
    getGasData(); 
    fetchData(); 
  }, 2500); // Fetch data every 2.5 seconds

  // Function to get temperature data 
  function getTemperatureData() { 
    var xhttp = new XMLHttpRequest(); 
    xhttp.onreadystatechange = function() { 
      if (this.readyState == 4 && this.status == 200) { 
        document.getElementById("TemperatureValue").innerHTML = this.responseText; 
      } 
    }; 
    xhttp.open("GET", "/readTemperature", true); // Send GET request to read temperature
    xhttp.send(); 
  }

  // Function to get humidity data 
  function getHumidityData() { 
    var xhttp = new XMLHttpRequest(); 
    xhttp.onreadystatechange = function() { 
      if (this.readyState == 4 && this.status == 200) { 
        document.getElementById("HumidityValue").innerHTML = this.responseText; 
      } 
    }; 
    xhttp.open("GET", "/readHumidity", true); // Send GET request to read humidity
    xhttp.send(); 
  }

  // Function to get gas data 
  function getGasData() { 
    var xhttp = new XMLHttpRequest(); 
    xhttp.onreadystatechange = function() { 
      if (this.readyState == 4 && this.status == 200) { 
        var gasData = this.responseText.split(','); // Split gas data into two parts
        var gasValue = gasData[0]; 
        var airQuality = gasData[1]; 
        document.getElementById("GasValue").innerHTML = gasValue; 
        document.getElementById("AirQuality").innerHTML = airQuality; 
      } 
    }; 
    xhttp.open("GET", "/readGasSensor", true); // Send GET request to read gas data
    xhttp.send(); 
  }

  // Function to fetch last 10 readings 
  function fetchData() { 
    var xhttp = new XMLHttpRequest(); 
    xhttp.onreadystatechange = function() { 
      if (this.readyState == 4 && this.status == 200) { 
        var data = JSON.parse(this.responseText); // Parse JSON response
        var table = document.getElementById("data-table"); 
        table.innerHTML = ""; // Clear table contents
        for (var i = 0; i < data.length; i++) { 
          var row = `<tr>
            <td>${data[i].temperature}</td>
            <td>${data[i].humidity}</td>
            <td>${data[i].co2}</td>
            <td>${data[i].air_quality}</td>
            <td>${data[i].timestamp}</td>
          </tr>`; 
          table.innerHTML += row; // Append row to table
        } 
      } 
    }; 
    xhttp.open("GET", "/fetchLastReadings", true); // Send GET request to fetch last 10 readings
    xhttp.send(); 
  }

  // Function to update time and date 
  setInterval(function() { 
    Time_Date(); 
  }, 1000); // Update time and date every second

  function Time_Date() { 
    var t = new Date(); 
    document.getElementById("time").innerHTML = t.toLocaleTimeString(); // Display current time
    var d = new Date(); 
    const dayNames = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday","Saturday"]; 
    const monthNames = ["January", "February", "March", "April", "May", "June","July", "August", "September", "October", "November", "December"]; 
    document.getElementById("date").innerHTML = dayNames[d.getDay()] + ", " + d.getDate() + "-" + monthNames[d.getMonth()] + "-" + d.getFullYear(); // Display current date
  } 
</script> 

  </body> 
</html> 
)=====";
