
struct wifi_config {
    const char *ssid;
    const char *password;
    const char *mqtt_server;
    const int mqtt_port;
};

// Configuration for the local network

/*
wifi_config cfg {
  "firecrestTV",
  "ghostbusters698",
  "broker.hivemq.com",
  1883
};
*/

wifi_config cfg {
  "monstrosity",          // ssid
  "91Manassas!",          // password
  "broker.hivemq.com",        // mqtt_server
  1883                   // mqtt_port, default is 1883
};



// Using the music teacher wifi, talking to the cloud hub
/*
wifi_config cfg {
  "MusicClosedWifi",          // ssid
  "ong8m!53S30L",          // password
  "hub.screamingchameleons.com",  // it's an alias for "ec2-34-238-52-221.compute-1.amazonaws.com",
  1883                   // mqtt_port, default is 1883
};
*/

// Using the school's local wifi
/*
wifi_config cfg {
  "MCS",          // ssid
  "hookemhorns",          // password
  "hub.screamingchameleons.com",  // it's an alias for "ec2-34-238-52-221.compute-1.amazonaws.com",
  1883                   // mqtt_port, default is 1883
};
*/



// At home, talking to the cloud hub
/*
wifi_config cfg {
  "monstrosity",          // ssid
  "91Manassas!",          // password
  "hub.screamingchameleons.com",  // it's an alias for "ec2-34-238-52-221.compute-1.amazonaws.com",
  1883                   // mqtt_port, default is 1883
};
*/


// At work, talking to the cloud hub
/*
wifi_config cfg {
  "firecrestTV",
  "ghostbusters698",
  "ec2-34-238-52-221.compute-1.amazonaws.com",
  1883                   // mqtt_port, default is 1883
};
*/
/*
// This did NOT work.  I was trying to use the raspberry pi as the access point and mqtt broker both.  It connects to wifi but not mqtt.
wifi_config cfg {
  "music-teacher-hub",     // ssid
  "pDNDOKD1xN4u",          // password
  "192.168.4.1",           // the raspberry pi mqtt_server
  1883                     // mqtt_port, default is 1883
};
*/
