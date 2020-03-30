#include "FastLED.h"
#include <CapacitiveSensor.h>

#include "wifi_config.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "blinker.h"
#include "WifiComm.h"
#include "MqttPublisher.h"
#include "Configurer.h"

const int BUTTON_PIN = D7;
const int LAMP_PIN = 6;
const int NUM_LEDS = 2;
CRGB leds[NUM_LEDS];

uint8_t hue = 0;
int target_hue = -1;
unsigned long state_change_time = 0;
unsigned long last_recv_time = 0;

CapacitiveSensor   cs = CapacitiveSensor(D4, D2);

int brightness = 0;

enum LightState {
  SENDING,
  RECEIVING,
  ASLEEP,
  FADING_ON,
  FADING_OFF,
  HOLD_COLOR,
  CONFIGURING
} light_state;

const int BLUE_LED_PIN = D5;
blinker blinkBlue(BLUE_LED_PIN);

WifiComm wifi_comm(blinkBlue);
MqttPublisher *publisher = NULL;
String uniqueId;

Configurer configurer;

static const char *ONLINE_TOPIC =     "616a7b49-aab4-4cbb-a7a8-ba7ed744dc11/Online";
static const char *OFFLINE_TOPIC =    "616a7b49-aab4-4cbb-a7a8-ba7ed744dc11/Offline";
static const char *HUE_TOPIC =        "616a7b49-aab4-4cbb-a7a8-ba7ed744dc11/Hue";


void setup() 
{
  delay(1000);
  Serial.begin(115200);
  Serial.println("");

  FastLED.addLeds<WS2811, LAMP_PIN>(leds, NUM_LEDS); 

  set_leds(CHSV(0, 255, 255));
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // If there's no configuration or the button is pressed on bootup, go into config mode
  String ssid, password;
  
  if(!configurer.GetConfig(ssid, password) || digitalRead(BUTTON_PIN) == 0) {
    Serial.println("***************** CONFIGURE ******************");
    light_state = CONFIGURING;
    set_leds(CRGB(0, 255, 0));
    FastLED.show();
    configurer.setup();
  }
  else {
    setup_network(ssid, password);
    light_state = ASLEEP;
  }
}

void loop() 
{
  if(light_state == CONFIGURING) {
    configurer.loop();
  }
  else {
    check_timeouts();
    check_button();
    
    switch(light_state) {
      case SENDING:
        hue = get_sensor_value();
        send_update(hue);
        break;
      case RECEIVING:
        hue = target_hue;
        break;
      case ASLEEP:
        brightness = 0;
        break;
      case FADING_ON:
        if(++brightness == 255)
          light_state = SENDING;
        delay(1);
        break;
      case HOLD_COLOR:
        break;
      case FADING_OFF:
        if(brightness == 0)
          light_state = ASLEEP;
        else
          brightness--;
        delay(1);
        break;
      case CONFIGURING:
        // Done above because of shared work for all the other cases.
        break;
    }
  
    set_leds(CHSV(hue, 255, brightness));
    FastLED.show();
  
    loop_network();
  }
}

// *******************************************************************
// Sensor Filtering
// *******************************************************************
int get_sensor_value() {
    static int smoothed = 0;  
    int total =  cs.capacitiveSensor(20);
    smoothed = smooth(total, smoothed);
    int clipped = clip(smoothed, 0, 255, 50);
  //  int scaled = scale(clip(clipped, 0, 200), 255, 200);
  //  Serial.println(String(clipped));
  return clipped;
}

int smooth(int newval, int prev){
  int rv = newval;
  const int MAX_MOVE = 2;

  if(newval > prev) {
    if(newval - prev > MAX_MOVE)
      rv = prev + MAX_MOVE;
  }
  else if(newval < prev) {
    if(prev - newval > MAX_MOVE)
      rv = prev - MAX_MOVE;
  }

  return rv;
}

int clip(int data, int minval, int maxval, int bias) {
  data -= bias;
  
  if(data < minval)
    data = minval;
  if(data > maxval)
    data = maxval;

  return data;
}

int scale(int data, int mul, int div) {
  return data * mul / div;
}


// *******************************************************************
// State Transitions
// *******************************************************************
void check_button() {
  if(digitalRead(BUTTON_PIN) == 0) {    // button pressed
    if(light_state == ASLEEP)
      light_state = FADING_ON;
  }
  else {
    if(light_state == SENDING) {
      light_state = HOLD_COLOR;
      state_change_time = millis();
    }
  }   
}

void check_timeouts(){
  unsigned long current_time = millis();
  
  switch(light_state) {
    case HOLD_COLOR:
      if(current_time - state_change_time > 5000)
        light_state = FADING_OFF;
      break;
    case RECEIVING:
      if(current_time - last_recv_time > 5000) {
        light_state = FADING_OFF;
      }
      break;
  }  
}


// *******************************************************************
// Message Publishers
// *******************************************************************
void send_update(int newval) {
  const unsigned long UPDATE_FREQUENCY = 200;
  static unsigned long lastUpdate = 0;
  int thisUpdate = millis();
  if(thisUpdate - lastUpdate < UPDATE_FREQUENCY)
    return;
  lastUpdate = thisUpdate;

  static int lastval;
  if(newval == lastval)
    return;
  lastval = newval;

  publisher->Publish(HUE_TOPIC, (uniqueId + "," + String(newval)).c_str());
}


// *******************************************************************
// Message Handlers
// *******************************************************************
void onHueMessage(byte *msg, unsigned int len) {
  char newval[80];

  // Only react to messages sent by a different device (not your own)
  if(strncmp((const char *)msg, uniqueId.c_str(), uniqueId.length()) != 0) {
    char *valptr = strchr((const char *)msg, ',') + 1;
    int copylen = len - (valptr - (char *)msg);
    strncpy(newval, valptr, copylen);
    newval[copylen] = 0;
//    Serial.println(String("Newval: ") + String(newval));
    
    target_hue = atoi(newval);

    last_recv_time = millis();
    if(light_state == ASLEEP) {
      light_state = RECEIVING;
      state_change_time = last_recv_time;
      brightness = 255;
    }
//    Serial.println(String("OnBrightnessMessage: ") + String(target_brightness));
  }
}


// *******************************************************************
// Network
// *******************************************************************
void setup_network(String &ssid, String &password) {
  
  // We start by connecting to a WiFi network
  wifi_comm.Connect(ssid.c_str(), password.c_str());

  // This fills uniqueId with a string that includes a few bytes of the MAC address, so it's pretty unique.
  wifi_comm.GetUniqueId(uniqueId, "friendlight");

  publisher = new MqttPublisher(wifi_comm.GetClient(), blinkBlue);
  publisher->Configure(cfg.mqtt_server, cfg.mqtt_port, uniqueId);
  publisher->SetWill(String(OFFLINE_TOPIC), uniqueId);
}

void loop_network() {
  if (!publisher->Connected()) {
    publisher->Reconnect(ONLINE_TOPIC, getOnlineMessage().c_str());
    publisher->Subscribe(HUE_TOPIC, onHueMessage);
  }
  publisher->Loop();
}

String getOnlineMessage() {
  return uniqueId;
}


// *******************************************************************
// Utility Functions
// *******************************************************************
void set_leds(CRGB color) {
  for(int n = 0; n < NUM_LEDS; n++) {
    leds[n] = color;
  }
}
