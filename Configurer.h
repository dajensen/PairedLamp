
class Configurer {

  String filename;
  
public:
	Configurer();
	boolean GetConfig(String &ssid, String &password);
	void setup();
	void loop();
};
