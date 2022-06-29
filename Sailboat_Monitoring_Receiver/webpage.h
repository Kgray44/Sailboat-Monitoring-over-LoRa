const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<style>
.greencard{
     width: 300px;
     height: 300px;
     background: #02b875;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
     text-align: center;
}
.bluecard{
     width: 300px;
     height: 300px;
     background: #33ccff;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
     text-align: center;
}

.dbluecard{
     width: 300px;
     height: 300px;
     background: #0000ff;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
     text-align: center;
}

.linkcard{
     width: 900px;
     height: 300px;
     background: #33ccff;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
     text-align: center;
}

.column {
  float: left;
  width: 50%;
}

/* Clear floats after the columns */
.row:after {
  content: "";
  display: table;
  clear: both;
}

/*.smartphone {
  position: relative;
  width: 360px;
  height: 640px;
  margin: auto;
  border: 16px black solid;
  border-top-width: 60px;
  border-bottom-width: 60px;
  border-radius: 36px;
}

.laptop {
  -webkit-transform-origin: 0 0;
  transform-origin: 0 0;
  -webkit-transform: scale(1) translate(-50%);// Scaled down for a better Try-it experience (change to 1 for full scale)
  transform: scale(1) translate(-50%); //Scaled down for a better Try-it experience (change to 1 for full scale)
  left: 50%;
  position: absolute;
  width: 1366px;
  height: 800px;
  border-radius: 6px;
  border-style: solid;
  border-color: black;
  border-width: 24px 24px 80px;
  background-color: black;

}*/


html {font-family: Arial;}h1 {font-family: Arial;}h2 { font-size: 3.0rem;}p

</style>

<body>
<h1>Live Sailboat Monitoring over LoRa</h1><br>
<h3>Satellite Count: <span id="satValue">...</span><br>
Signal Strength: <span id="signalValue">...</span><br><br>
RSSI (50 - 150): <span id="sigValue">0</span></h3><br>
Battery Voltage: <span id="batteryValue">0</span><br>
Battery Level: <span id="batteryLevel">0</span><br>

<div class="row">
  <div class="column">
    <div class="greencard">
      <h2>Speed</h2>
      <h1><span id="speedValue">0</span> </h1><h3>knots</h3>
    </div>
  </div>
  <div class="column">
    <div class="bluecard">
      <h2>Course</h2>
      <h1><span id="courseValue">0</span> </h1><h3>degrees</h3>
    </div>
  </div>
</div>
<div class="row">
  <div class="column">
    <div class="bluecard">
      <h2>Top Speed</h2>
      <h1><span id="topspeedValue">0</span> </h1><h3>knots</h3>
    </div>
  </div>
  <div class="column">
    <div class="greencard">
      <h2>Altitude</h2>
      <h1><span id="altitudeValue">0</span> </h1><h3>feet</h3>
    </div>
  </div>
</div>
<div class="row">
  <div class="column">
    <div class="greencard">
      <h2>Heeling Angle</h2>
      <h1><span id="heelValue">0</span> </h1><h3>degrees</h3>
    </div>
  </div>
  <div class="column">
    <div class="bluecard">
      <h2>Rolling Angle</h2>
      <h1><span id="rollValue">0</span> </h1><h3>degrees</h3>
    </div>
  </div>
</div>
<div class="row">
  <div class="column">
    <div class="bluecard">
      <h2>Latitude</h2>
      <h2><span id="latitudeValue">0</span> </h2>
    </div>
  </div>
  <div class="column">
    <div class="greencard">
      <h2>Longitude</h2>
      <h2><span id="longitudeValue">0</span> </h2>
    </div>
  </div>
</div>
<div class="row">
  <div class="column">
    <div class="linkcard">
      <h2>GPS Location</h2>
      <h2><a href="/map">Click here for map</a></h2>
    </div>
  </div>
</div>


<script>

setInterval(function() {
  // Call a function repetatively with 2 Second interval
  getData();
}, 1000); //1 sec update rate

setInterval(function() {
  getSecondData();
}, 4000);//4 sec

setInterval(function() {
  getIconData();
}, 20000);//20 sec

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("speedValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readSpeed", true);
  xhttp.send();

  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("courseValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readCourse", true);
  xhttp.send();

  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("topspeedValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readTopspeed", true);
  xhttp.send();

  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("heelValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readHeel", true);
  xhttp.send();

  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("rollValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readRoll", true);
  xhttp.send();

}

function getSecondData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("altitudeValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readAltitude", true);
  xhttp.send();

  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("latitudeValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readLatitude", true);
  xhttp.send();

  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("longitudeValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readLongitude", true);
  xhttp.send();

  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("gpsValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readGPS", true);
  xhttp.send();

  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("satValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readSat", true);
  xhttp.send();
  
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("sigValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readRSSI", true);
  xhttp.send();

  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("batteryValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readBattery", true);
  xhttp.send();
  
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("batteryLevel").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readBatterylevel", true);
  xhttp.send();
}

function getIconData() {
  /*var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("signaliconValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readSignalicon", true);
  xhttp.send();*/

  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("signalValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readDisplaysignalicon", true);
  xhttp.send();
}

</script>
</body>
</html>
)=====";
