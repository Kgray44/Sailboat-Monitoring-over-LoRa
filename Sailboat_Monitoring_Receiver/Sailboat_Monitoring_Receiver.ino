#include <RadioLib.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoOTA.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <ArduinoOTA.h>
#include "webpage.h"

#define LED_BUILTIN 2

int nodeAmount = 2;

int nodenumber=0;
const char* WIFI_SSID = "Sailboat Monitoring";
const char* WIFI_PASS = "iLoveSailing!";

volatile bool receivedFlag = false;
volatile bool enableInterrupt = true;

int state;

int first, second, third, fourth, fifth, sixth, seventh, eighth, ninth, tenth;

float topspeed[2], speed[2], altitude[2], course[2], latitude[2], longitude[2], heel[2], roll[2], battery[2], batterylevel[2];
int sat[2];
int signal[2];

float fullVoltage = 4.20;
float emptyVoltage = 3.20;


// SX1262 has the following connections:
// NSS pin:   10
// DIO1 pin:  2
// NRST pin:  3
// BUSY pin:  9
SX1262 radio = new Module(27, 4, 2, 15);

WebServer server(80);

void handleRoot() {
  String s = MAIN_page; //Read HTML contents 
  server.send(200, "text/html", s); //Send web page
}

void handleReadSpeed() {
  String sp = String(speed[0]) + "," + String(speed[1]);
  server.send(200, "text/plane", sp);
}

void handleReadCourse() {
  String sp = String(course[0]) + "," + String(course[1]);
  server.send(200, "text/plane", sp);
}

void handleReadTopspeed() {
  String sp = String(topspeed[0]) + "," + String(topspeed[1]);
  server.send(200, "text/plane", sp);
}

void handleReadAltitude() {
  String sp = String(altitude[0]) + "," + String(altitude[1]);
  server.send(200, "text/plane", sp);
}

void handleReadHeel() {
  String sp = String(heel[0]) + "," + String(heel[1]);
  server.send(200, "text/plane", sp);
}

void handleReadRoll() {
  String sp = String(roll[0]) + "," + String(roll[1]);
  server.send(200, "text/plane", sp);
}

void handleReadLatitude() {
  String sp = String(latitude[0],6) + "<br>";
  sp += String(latitude[1],6);
  server.send(200, "text/plane", sp);
}

void handleReadLongitude() {
  String sp = String(longitude[0],6) + "<br>";
  sp += String(longitude[1],6);
  server.send(200, "text/plane", sp);
}

void handleReadSat() {
  String sp = String(sat[0]) + "," + String(sat[1]);
  server.send(200, "text/plane", sp);
}

void handleReadGPS() {
//  String sp = "https://www.google.com/maps/place/" + String(latitude,5) + "," + String(longitude,5);
  //String sp = "geo:" + String(latitude,6) + "," + String(longitude,6);
  String sp = "http://maps.apple.com/?q=" + String(latitude[0],6) + "," + String(longitude[0],6) + "<br>";
  sp += "http://maps.apple.com/?q=" + String(latitude[1],6) + "," + String(longitude[1],6);
  server.send(200, "text/plane", sp);
}//<span id="gpsValue">0</span>

void handleReadSignalicon() {
  String sp;
  if (abs(signal[0]) < 65){
    sp = "/5bars.png";
  }
  else if (abs(signal[0]) >= 65 && abs(signal[0]) < 86){
    sp = "/4bars.png";
  }
  else if (abs(signal[0]) >= 86 && abs(signal[0]) < 107){
    sp = "/3bars.png";
  }
  else if (abs(signal[0]) >= 107 && abs(signal[0]) < 128){
    sp = "/2bars.png";
  }
  else if (abs(signal[0]) >= 128 && abs(signal[0]) < 150){
    sp = "/1bars.png";
  }
  else if (abs(signal[0]) >= 150){
    sp = "/0bars.png";
  }

  File capture = SPIFFS.open(sp);
  
  //server.sendHeader("Content-Type","image/png",true);
  server.streamFile(capture, "image/png");
    
  capture.close();


  String se;
  if (abs(signal[1]) < 65){
    se = "/5bars.png";
  }
  else if (abs(signal[1]) >= 65 && abs(signal[1]) < 86){
    se = "/4bars.png";
  }
  else if (abs(signal[1]) >= 86 && abs(signal[1]) < 107){
    se = "/3bars.png";
  }
  else if (abs(signal[1]) >= 107 && abs(signal[1]) < 128){
    se = "/2bars.png";
  }
  else if (abs(signal[1]) >= 128 && abs(signal[1]) < 150){
    se = "/1bars.png";
  }
  else if (abs(signal[1]) >= 150){
    se = "/0bars.png";
  }

  File capture2 = SPIFFS.open(se);
  
  //server.sendHeader("Content-Type","image/png",true);
  server.streamFile(capture2, "image/png");
    
  capture2.close();

  /*File file = SPIFFS.open(sp, "r");
  server.streamFile(file, "image/png");
  file.close();*/
  //server.send(200, "text/plane", sp);
}

void handleMap(){
  //String sp = "http://maps.apple.com/?q=" + String(latitude,6) + "," + String(longitude,6);
  String se = "<h1><a href='http://maps.apple.com/?q=" + String(latitude[0],6) + "," + String(longitude[0],6) + "'>Node 1 Map</a></h1><br>";
  se += "<h1><a href='http://maps.apple.com/?q=" + String(latitude[1],6) + "," + String(longitude[1],6) + "'>Node 2 Map</a></h1><br>";
  se += "<h2>Directions:<h2>";
  se += "<h3>Click on the correct map link.<br>";
  se += "Go to your iPhone control menu, and shut off the WiFi.<br>";
  se += "Enjoy your map view!<br>";
  se += "To go back to the sailboat monitoring page, turn the WiFi back on, and check to make sure you are connected to 'Sailboat Monitoring'.";
  server.send(200, "text/html", se);
}

void handleDisplaysignalicon() {
  String sp = "<img src='/readSignalicon'>";
  server.send(200, "text/html", sp);
}

void handleReadRSSI() {
  String sp = String(abs(signal[0])) + "," + String(abs(signal[1]));
  server.send(200, "text/plane", sp);
}

void handleReadBattery() {
  String sp = String(battery[0]) + "," + String(battery[1]);
  server.send(200, "text/plane", sp);
}

void handleReadBatterylevel() {
  float batPer1 = map(battery[0], emptyVoltage, fullVoltage, 0, 100);
  float batPer2 = map(battery[1], emptyVoltage, fullVoltage, 0, 100);

  String sp = String(batPer1) + "," + String(batPer2);
  server.send(200, "text/plane", sp);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_SSID, WIFI_PASS);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  
  server.on("/", handleRoot);
  server.on("/readSpeed", handleReadSpeed);
  server.on("/readCourse", handleReadCourse);
  server.on("/readTopspeed", handleReadTopspeed);
  server.on("/readAltitude", handleReadAltitude);
  server.on("/readHeel", handleReadHeel);
  server.on("/readRoll", handleReadRoll);
  server.on("/readLatitude", handleReadLatitude);
  server.on("/readLongitude", handleReadLongitude);
  server.on("/readGPS", handleReadGPS);
  server.on("/readSignalicon", handleReadSignalicon);
  server.on("/map", handleMap);
  server.on("/readDisplaysignalicon", handleDisplaysignalicon);
  server.on("/readSat", handleReadSat);
  server.on("/readRSSI", handleReadRSSI);
  server.on("/readBattery", handleReadBattery);
  server.on("/readBatterylevel", handleReadBatterylevel);
  server.begin();

  initialiseRF();

  Serial.println("OTA Starting...");
  OTAStart();
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  
  // check if the flag is set
  if(receivedFlag) {
    // disable the interrupt service routine while
    // processing the data
    enableInterrupt = false;

    // reset flag
    receivedFlag = false;


    // you can read received data as an Arduino String
    String str;
    int state = radio.readData(str);

    first = str.indexOf(',');
    nodenumber = str.substring(0,first).toInt();
    second = str.indexOf(',',first+1);
    latitude[nodenumber] = str.substring(first+1,second+1).toFloat();
    //Serial.println(latitude);
    third = str.indexOf(',',second+1);
    longitude[nodenumber] = str.substring(second+1,third+1).toFloat();
    fourth = str.indexOf(',',third+1);
    speed[nodenumber] = str.substring(third+1,fourth+1).toFloat();
    fifth = str.indexOf(',',fourth+1);
    course[nodenumber] = str.substring(fourth+1,fifth+1).toFloat();
    sixth = str.indexOf(',',fifth+1);
    altitude[nodenumber] = str.substring(fifth+1,sixth+1).toFloat();
    seventh = str.indexOf(',',sixth+1);
    heel[nodenumber] = str.substring(sixth+1,seventh+1).toFloat()*0.7;
    eighth = str.indexOf(',',seventh+1);
    roll[nodenumber] = str.substring(seventh+1,eighth+1).toFloat()*0.7;
    ninth = str.indexOf(',',eighth+1);
    sat[nodenumber] = str.substring(eighth+1,ninth+1).toFloat();
    tenth = str.indexOf(',',ninth+1);
    battery[nodenumber] = str.substring(ninth+1,tenth).toFloat();

    if (speed[0] > topspeed[0]){
      topspeed[0] = speed[0];
    }
    if (speed[1] > topspeed[1]){
      topspeed[1] = speed[1];
    }

    Serial.println(str);
    Serial.println("Node: " + String(nodenumber) + "latitude: " + String(latitude[nodenumber],6) + " longitude: " + String(longitude[nodenumber],6) + " speed: " + String(speed[nodenumber]) + " course: " + String(course[nodenumber]) + " altitude: " + String(altitude[nodenumber]) + " heel: " + String(heel[nodenumber]) + " roll: " + String(roll[nodenumber]));


    // you can also read received data as byte array
    /*
      byte byteArr[8];
      int state = radio.readData(byteArr, 8);
    */

    if (state == RADIOLIB_ERR_NONE) {
      // packet was successfully received
      Serial.println(F("[SX1262] Received packet!"));

      // print data of the packet
      Serial.print(F("[SX1262] Data:\t\t"));
//      Serial.println(str);
//      Serial.println("Node: " + String(nodenumber) + "latitude: " + String(latitude[0],6) + " longitude: " + String(longitude[0],6) + " speed: " + String(speed[0]) + " course: " + String(course[nodenumber]) + " altitude: " + String(altitude[nodenumber]) + " heel: " + String(heel[nodenumber]) + " roll: " + String(roll[nodenumber]));

      // print RSSI (Received Signal Strength Indicator)
      Serial.print(F("[SX1262] RSSI:\t\t"));
      Serial.print(radio.getRSSI());
      Serial.println(F(" dBm"));
      signal[nodenumber] = radio.getRSSI();

      // print SNR (Signal-to-Noise Ratio)
      Serial.print(F("[SX1262] SNR:\t\t"));
      Serial.print(radio.getSNR());
      Serial.println(F(" dB"));

    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
      // packet was received, but is malformed
      Serial.println(F("CRC error!"));

    } else {
      // some other error occurred
      Serial.print(F("failed, code "));
      Serial.println(state);

    }

    // put module back to listen mode
    radio.startReceive();

    // we're ready to receive more packets,
    // enable interrupt service routine
    enableInterrupt = true;

  }

  /*
  Serial.print(F("[SX1262] Starting to listen ... "));
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }*/

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

  // set the function that will be called
  // when new packet is received
  radio.setDio1Action(setFlag);

  // start listening for LoRa packets
  Serial.print(F("[SX1262] Starting to listen ... "));
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  // if needed, 'listen' mode can be disabled by calling
  // any of the following methods:
  //
  // radio.standby()
  // radio.sleep()
  // radio.transmit();
  // radio.receive();
  // radio.readData();
  // radio.scanChannel();

}

#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif
void setFlag(void) {
  // check if the interrupt is enabled
  if(!enableInterrupt) {
    return;
  }
 
  // we got a packet, set the flag
  receivedFlag = true;
}

void OTAStart(){
    // Port defaults to 8266
   ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("SailMonitoringRecESP");

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
}
