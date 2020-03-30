
class WifiComm {

	WiFiClient espClient;
	blinker &blink;

public:
	WifiComm(blinker &blink);
	void Connect(const char *ssid, const char *password);
	WiFiClient &GetClient();
	void GetUniqueId(String &id, const char *prefix);
};


