
class MqttPublisher {

public:
  MqttPublisher(WiFiClient &wifiClient, blinker &blink);

  void SetWill(const String &topic, const String &msg);
  void Configure(const char *mqtt_server, const int mqtt_port, String &uniqueId);
  void Reconnect(const char *online_topic, const char *online_msg);
  bool Connected() { return client.connected();}
  void Loop() {client.loop();}
  void Publish(const char *topic, const char *msg);

  typedef void (*subscribefn)(byte *msg, unsigned int len);
  void Subscribe(const char *topic, subscribefn fn);
  
protected:
	PubSubClient client;
	String id;
  String willTopic;
  String willMsg;
  blinker &blink;
  static MqttPublisher *pThis;
  static const int MAX_SUBSCRIPTIONS = 5;

  struct MqttCallback {
      String topicname;
      subscribefn fn;      
  };
  MqttCallback subscriptions[MAX_SUBSCRIPTIONS];
  int numSubscriptions;

  static void staticMqttCallback(const char* topic,  byte* payload, unsigned int len);
  void mqttCallback(const char* topic,  byte* payload, unsigned int len);
  bool AttemptConnection();
};
