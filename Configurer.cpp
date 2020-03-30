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
Configurer::Configurer() : filename("/wifi.cfg") {
}

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
    [this](AsyncWebServerRequest *request){
      Serial.println("POST to /config");
      Serial.println("Data: " + String((char *)request->_tempObject));
      if(SaveSettings((char *)request->_tempObject))
        request->send_P(200, "text/html", "Ok");
      else
        request->send_P(500, "text/html", "Unable to save settings");      
    },
    [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
      // This should never happen.  It would be a big surprise.
      Serial.println("************ It was an upload????");
    },
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
        if(!index){
          Serial.printf("BodyStart: %u\n", total);
          request->_tempObject = new uint8_t[total + 1];
          ((char *)(request->_tempObject))[total] = 0;
        }
        
        memcpy(&((char *)request->_tempObject)[index], data, len);
        
        if(index + len == total){
          Serial.printf("BodyEnd: %u\n", total);
        }
    }
    );

    pServer->begin();
}

void Configurer::loop() {

}

boolean Configurer::SaveSettings(char *settings){
  boolean rv = false;
  File f = SPIFFS.open(filename, "w");
  if(f) {
      f.write(settings, strlen(settings));
      f.close();
      rv = true;
  }
  else {
      Serial.println("Open file for writing failed");
  }
  return rv;
}
