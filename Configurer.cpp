#include <Arduino.h>
#include "Configurer.h"
#include "FS.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid     = "PairedLampConfig";
const char* password = "88888888";  // eight eights.

AsyncWebServer *pServer;


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
  });
  pServer->on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Get hello");
    request->send_P(200, "text/html", "Hello there, Dave!");
  });
  pServer->on("/config", HTTP_POST, 
    [](AsyncWebServerRequest *request){
      Serial.println("POST config");
      request->send_P(200, "text/html", "Ok");
    },
    [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
      Serial.println("It was an upload????");
    },
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
        if(!index){
          Serial.printf("BodyStart: %u B\n", total);
        }
        for(size_t i=0; i<len; i++){
          Serial.write(data[i]);
        }
        if(index + len == total){
          Serial.printf("BodyEnd: %u B\n", total);
        }    
    }
    );

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
