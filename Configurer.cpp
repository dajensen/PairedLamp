#include <Arduino.h>
#include "Configurer.h"
#include "FS.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid     = "PairedLampConfig";
const char* password = "88888888";  // eight eights.

AsyncWebServer *pServer;


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP8266 DHT Server</h2>
  <p>
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">%</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";


// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String("This would be the TEMPERATURE");
  }
  else if(var == "HUMIDITY"){
    return String("This would be the HUMIDITY");
  }
  return String();
}



//*******************************************************************
// Class: Configurer
//*******************************************************************
Configurer::Configurer() : filename("/test.cfg") {
}

/*
boolean Configurer::GetConfig(String &ssid, String &password) {
    boolean rv = false;
    if(SPIFFS.begin()) {
        File f = SPIFFS.open(filename.c_str(), "r");
        if (f) {
            Serial.println(filename + " file size: " + String(f.size()));
            char cfgData[240];
            int datalen = f.readBytes(cfgData, sizeof(cfgData) - 1);
            cfgData[datalen] = 0;
            Serial.println("DATA: [");
            Serial.println(String(cfgData));
            Serial.println("]");

            char *p1 = cfgData;
            char *p2 = strchr(cfgData, ',');
            if(p2) {
              *p2 = 0;
              p2++;
              ssid = p1;
              password = p2;
              rv = true;
            }
        }
        else
            Serial.println("Open file for reading failed");

        SPIFFS.end();
    }
    return rv;
}
*/

void Configurer::setup() {
  pServer = new AsyncWebServer(80);

  WiFi.softAP(ssid, password);
    
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  SPIFFS.begin();
  
    // Route for root / web page
  pServer->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Get the root");
    request->send(SPIFFS, "/index.htm");
    // request->send_P(200, "text/html", index_html, processor);
//    request->send_P(200, "text/html", "This should be index.htm");
  });
  pServer->on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Get hello");
    request->send_P(200, "text/html", "Hello there, Dave!");
  });


    pServer->begin();
}

void Configurer::loop() {

}


// Some sample SPIFFS code that WORKS, for comparison
boolean Configurer::GetConfig(String &ssid, String &password) {
  Serial.println("");
  
    if(!SPIFFS.begin()){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return false;
  }
  
  File file = SPIFFS.open("/index.htm", "r");
  if(!file){
    Serial.println("Failed to open file for reading");
    return false;
  }
  
  Serial.println();
  Serial.println("File Content:");
  while(file.available()){
    Serial.write(file.read());
  }
  file.close();
  return false;
}

/*
// And some initial file writing
        if(!SPIFFS.exists(filename.c_str())) {
            Serial.println(filename + " does not exist, writing it");
            char *sampledata = "monstrosity,91Manassas!";
            File f = SPIFFS.open(filename, "w");
            if(f) {
                f.write(sampledata, strlen(sampledata));
                f.close();
            }
            else {
                Serial.println("Open file for writing failed");
            }          
        }
        else {
            Serial.println("IT EXISTS:");
        }

 */
