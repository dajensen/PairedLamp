
class Configurer {

  String filename;
  boolean SaveSettings(char *settings);
  
public:
	Configurer();
	boolean GetConfig(String &ssid, String &password);
	void setup();
	void loop();
};
