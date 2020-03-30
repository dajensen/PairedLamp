#include <Arduino.h>
#include <arduino.h>
#include <Esp8266WiFi.h>
#include <PubSubClient.h>

#include "blinker.h"
#include "MqttPublisher.h"

static bool print_messages = true;
MqttPublisher *MqttPublisher::pThis;


MqttPublisher::MqttPublisher(WiFiClient &wifiClient, blinker &blink) : 
  client(wifiClient), blink(blink), numSubscriptions(0)
{
  pThis = this;
}

void MqttPublisher::SetWill(const String &topic, const String &msg) {
  willTopic = topic;
  willMsg = msg;
}

void MqttPublisher::Configure(const char *mqtt_server, const int mqtt_port, String &uniqueId) {

  id = uniqueId;
	client.setServer(mqtt_server, mqtt_port);
  client.setCallback(staticMqttCallback);
}

bool MqttPublisher::AttemptConnection() {
  if(willTopic.length()) {
    return client.connect(id.c_str(), willTopic.c_str(), 0, false, willMsg.c_str());
  }
  return client.connect(id.c_str());
}

void MqttPublisher::Reconnect(const char *online_topic, const char *online_msg) {
	// Loop until we're reconnected
 Serial.println("Connecting MQTT");
 blink.startBlinking(500);
	while (!Connected()) {
		// Attempt to connect
		if (AttemptConnection()) {
			Serial.println("MQTT connected");
       numSubscriptions = 0;
		} else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");

			// Wait 5 seconds before retrying
      int end_time = millis() + 5000;
      while(millis() < end_time) {
        delay(10);
        blink.check();
      }
		}
	}
 Publish(online_topic, online_msg);
	blink.on(-1);
	delay(1000);
}

void MqttPublisher::Publish(const char *topic, const char *msg) {
  Serial.println("Publishing");
  client.publish(topic, msg);
}

void MqttPublisher::Subscribe(const char *topic, subscribefn fn) {
  if(numSubscriptions < sizeof(subscriptions) / sizeof(subscriptions[0])) {
     subscriptions[numSubscriptions].topicname = String(topic);
     subscriptions[numSubscriptions].fn = fn;
     numSubscriptions++;
     client.subscribe(topic);
     Serial.println("Subscribed to: " + String(topic));
  }
}

void MqttPublisher::staticMqttCallback(const char *topic, byte* payload, unsigned int len) {
  Serial.print("Got message for topic: " + String(topic) + " data: ");
  for(int i = 0; i < len; i++)
    Serial.print((char)payload[i]);
  Serial.println("");
  
  pThis->mqttCallback(topic, payload, len);
}

void MqttPublisher::mqttCallback(const char *topic, byte* payload, unsigned int len) {

  String topicname = topic;

  for(int i = 0; i < numSubscriptions; i++) {
    if(topicname == subscriptions[i].topicname) {
      subscriptions[i].fn(payload, len);
      break;
    }
  }
}
