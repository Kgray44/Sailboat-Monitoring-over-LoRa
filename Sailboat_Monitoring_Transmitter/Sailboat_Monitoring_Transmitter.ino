#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <RadioLib.h>
#include <DFRobot_BMI160.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define batteryPin A1

char ssid[] = "Apple Network 85064d";
char pass[] = "12344321";

int nodenumber = 0;//0 or 1

float R1 = 4700.0;
float R2 = 2700.0;

static const int RXPin = 12, TXPin = 14;
static const uint32_t GPSBaud = 9600;

int rfPower = 30;
int state;
int rslt;

float speed, altitude, course, latitude, longitude, heel, roll;
int sat;

float x, y, z;

float batteryVoltage, battery;
float ref_voltage = 3.30;

boolean accelresult = false;
boolean gpsresult = false;

SX1262 radio = new Module(27, 4, 2, 15);
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
DFRobot_BMI160 bmi160;
const int8_t i2c_addr = 0x69;

unsigned long last = 0UL;


void setup() {
  Serial.begin(115200);
  ss.begin(GPSBaud);

  pinMode(batteryPin, INPUT);
  
  initialiseRF();

  if (bmi160.softReset() != BMI160_OK){
    Serial.println("reset false");
    while(1);
  }
  if (bmi160.I2cInit(i2c_addr) != BMI160_OK){
    Serial.println("init false");
    while(1);
  }

  Serial.println("WiFi Connecting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(1000);
    yield();
    ESP.restart();
  }
  Serial.println("OTA Starting...");
  OTAStart();
}

void loop() {
  ArduinoOTA.handle();

  battery = (analogRead(batteryPin) * ref_voltage) / 4095.0;
  batteryVoltage = ((battery * (R1/(R1+R2)))*1.4)+1;//battery / (R2/(R1+R2)); 
  Serial.println(batteryVoltage);
  while (ss.available() > 0)
    gps.encode(ss.read());

  if (gps.location.isUpdated()){
    speed = gps.speed.knots();
    altitude = gps.altitude.feet();
    course = gps.course.deg();
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    sat = gps.satellites.value();
    gpsresult = true;

    int16_t onlyAccel[3]={0};
    rslt = bmi160.getAccelData(onlyAccel);
    if(rslt == 0){
      //Serial.print("X: " + String(onlyAccel[0]/163.84));
      //Serial.print(" Y: " + String(onlyAccel[1]/163.84));
      //Serial.print(" Z: " + String(onlyAccel[2]/163.84));
      roll = onlyAccel[0]/163.84;
      heel = onlyAccel[1]/163.84;
      z = onlyAccel[2]/163.84;
      accelresult = true;
    }

  }
  
  if (gpsresult && accelresult){
    if (nodenumber == 1){
      delay(800);
    }
    //Serial.println("Latitude: " + String(latitude) + " Longitude: " + String(longitude) + " Heel: " + String(heel) + " Roll: " + String(roll));
    int state = radio.transmit(String(nodenumber) + "," + String(latitude,6) + "," + String(longitude,6) + "," + String(speed) + "," + String(course) + "," + String(altitude) + "," + String(heel) + "," + String(roll) + "," + String(sat) + "," + String(batteryVoltage));
    checkTransmit(state);
    accelresult = false;
    gpsresult = false;
  }

  printGPS();
  
  //delay(1000);
}

void checkTransmit(int s){
  if (s == RADIOLIB_ERR_NONE) {
    // the packet was successfully transmitted
    Serial.println(F("success!"));

    // print measured data rate
    Serial.print(F("[SX1262] Datarate:\t"));
    Serial.print(radio.getDataRate());
    Serial.println(F(" bps"));

  } else if (s == RADIOLIB_ERR_PACKET_TOO_LONG) {
    // the supplied packet was longer than 256 bytes
    Serial.println(F("too long!"));

  } else if (s == RADIOLIB_ERR_TX_TIMEOUT) {
    // timeout occured while transmitting packet
    Serial.println(F("timeout!"));

  } else {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(s);
  }
}

void initialiseRF(){
  // initialize the second LoRa instance with
  // non-default settings
  // this LoRa link will have high data rate,
  // but lower range
  Serial.print(F("[SX1268] Initializing ... "));
  // carrier frequency:           915.0 MHz
  // bandwidth:                   125.0 kHz
  // spreading factor:            10
  // coding rate:                 5
  // sync word:                   0x34 (public network/LoRaWAN)
  // output power:                2 dBm
  // preamble length:             20 symbols
  state = radio.begin(915.0, 125.0, 10, 5, 0x34, 20);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
  radio.setOutputPower(rfPower);
  radio.setRfSwitchPins(25, 26);
}

void printGPS(){
  if (gps.location.isUpdated())
  {
    Serial.print(F("LOCATION   Fix Age="));
    Serial.print(gps.location.age());
    Serial.print(F("ms Raw Lat="));
    Serial.print(gps.location.rawLat().negative ? "-" : "+");
    Serial.print(gps.location.rawLat().deg);
    Serial.print("[+");
    Serial.print(gps.location.rawLat().billionths);
    Serial.print(F(" billionths],  Raw Long="));
    Serial.print(gps.location.rawLng().negative ? "-" : "+");
    Serial.print(gps.location.rawLng().deg);
    Serial.print("[+");
    Serial.print(gps.location.rawLng().billionths);
    Serial.print(F(" billionths],  Lat="));
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(" Long="));
    Serial.println(gps.location.lng(), 6);
  }

  else if (gps.date.isUpdated())
  {
    Serial.print(F("DATE       Fix Age="));
    Serial.print(gps.date.age());
    Serial.print(F("ms Raw="));
    Serial.print(gps.date.value());
    Serial.print(F(" Year="));
    Serial.print(gps.date.year());
    Serial.print(F(" Month="));
    Serial.print(gps.date.month());
    Serial.print(F(" Day="));
    Serial.println(gps.date.day());
  }

  else if (gps.time.isUpdated())
  {
    Serial.print(F("TIME       Fix Age="));
    Serial.print(gps.time.age());
    Serial.print(F("ms Raw="));
    Serial.print(gps.time.value());
    Serial.print(F(" Hour="));
    Serial.print(gps.time.hour());
    Serial.print(F(" Minute="));
    Serial.print(gps.time.minute());
    Serial.print(F(" Second="));
    Serial.print(gps.time.second());
    Serial.print(F(" Hundredths="));
    Serial.println(gps.time.centisecond());
  }

  else if (gps.speed.isUpdated())
  {
    Serial.print(F("SPEED      Fix Age="));
    Serial.print(gps.speed.age());
    Serial.print(F("ms Raw="));
    Serial.print(gps.speed.value());
    Serial.print(F(" Knots="));
    Serial.print(gps.speed.knots());
    Serial.print(F(" MPH="));
    Serial.print(gps.speed.mph());
    Serial.print(F(" m/s="));
    Serial.print(gps.speed.mps());
    Serial.print(F(" km/h="));
    Serial.println(gps.speed.kmph());
  }

  else if (gps.course.isUpdated())
  {
    Serial.print(F("COURSE     Fix Age="));
    Serial.print(gps.course.age());
    Serial.print(F("ms Raw="));
    Serial.print(gps.course.value());
    Serial.print(F(" Deg="));
    Serial.println(gps.course.deg());
  }

  else if (gps.altitude.isUpdated())
  {
    Serial.print(F("ALTITUDE   Fix Age="));
    Serial.print(gps.altitude.age());
    Serial.print(F("ms Raw="));
    Serial.print(gps.altitude.value());
    Serial.print(F(" Meters="));
    Serial.print(gps.altitude.meters());
    Serial.print(F(" Miles="));
    Serial.print(gps.altitude.miles());
    Serial.print(F(" KM="));
    Serial.print(gps.altitude.kilometers());
    Serial.print(F(" Feet="));
    Serial.println(gps.altitude.feet());
  }

  else if (gps.satellites.isUpdated())
  {
    Serial.print(F("SATELLITES Fix Age="));
    Serial.print(gps.satellites.age());
    Serial.print(F("ms Value="));
    Serial.println(gps.satellites.value());
  }

  else if (gps.hdop.isUpdated())
  {
    Serial.print(F("HDOP       Fix Age="));
    Serial.print(gps.hdop.age());
    Serial.print(F("ms raw="));
    Serial.print(gps.hdop.value());
    Serial.print(F(" hdop="));
    Serial.println(gps.hdop.hdop());
  }

  else if (millis() - last > 5000)
  {
    Serial.println();
    if (gps.location.isValid())
    {
      static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;
      double distanceToLondon =
        TinyGPSPlus::distanceBetween(
          gps.location.lat(),
          gps.location.lng(),
          LONDON_LAT, 
          LONDON_LON);
      double courseToLondon =
        TinyGPSPlus::courseTo(
          gps.location.lat(),
          gps.location.lng(),
          LONDON_LAT, 
          LONDON_LON);

      Serial.print(F("LONDON     Distance="));
      Serial.print(distanceToLondon/1000, 6);
      Serial.print(F(" km Course-to="));
      Serial.print(courseToLondon, 6);
      Serial.print(F(" degrees ["));
      Serial.print(TinyGPSPlus::cardinal(courseToLondon));
      Serial.println(F("]"));
    }

    Serial.print(F("DIAGS      Chars="));
    Serial.print(gps.charsProcessed());
    Serial.print(F(" Sentences-with-Fix="));
    Serial.print(gps.sentencesWithFix());
    Serial.print(F(" Failed-checksum="));
    Serial.print(gps.failedChecksum());
    Serial.print(F(" Passed-checksum="));
    Serial.println(gps.passedChecksum());

    if (gps.charsProcessed() < 10)
      Serial.println(F("WARNING: No GPS data.  Check wiring."));

    last = millis();
    Serial.println();
  }
}


void OTAStart(){
    // Port defaults to 8266
   ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  if (nodenumber = 0){
    ArduinoOTA.setHostname("SailMonitoringNode1ESP");
  }
  else if (nodenumber = 1){
    ArduinoOTA.setHostname("SailMonitoringNode2ESP");
  }

  // No authentication by default
   ArduinoOTA.setPassword("maker");
  
  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    //open();
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
