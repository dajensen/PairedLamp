#include <arduino.h>
#include <Esp8266WiFi.h>
#include "blinker.h"
#include "WifiComm.h"

WifiComm::WifiComm(blinker &blink) : blink(blink), espClient() {
}

void WifiComm::Connect(const char *ssid, const char *password) {
	
	Serial.print("\n\nConnecting to ");
	Serial.println(ssid);
	  
  WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);

  blink.startBlinking(100);
	while (WiFi.status() != WL_CONNECTED) {
		delay(100);
    blink.check();
	}
	blink.on(-1);
	
	Serial.println("\nWiFi connected");  
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
}

WiFiClient &WifiComm::GetClient() {
	return espClient;
}

void WifiComm::GetUniqueId(String &id, const char *prefix)
{
   // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "<prefix>-"
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.macAddress(mac);
  String macID = "";
  
  if(mac[WL_MAC_ADDR_LENGTH - 2] < 0x10)
    macID += '0';
  macID += String(mac[WL_MAC_ADDR_LENGTH - 2], HEX);
  if(mac[WL_MAC_ADDR_LENGTH - 1] < 0x10)
    macID += '0';
  macID += String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  
  macID.toUpperCase();
  id = String(prefix) + "-" + macID;
}
