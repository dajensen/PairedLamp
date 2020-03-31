# PairedLamp
Paired lamps for people isolated by COVID-19.  Uses NodeMCU 1.0.

Build dependencies:
Install FastLED, CapacitiveSensor from Sketch->IncludeLibrary->ManageLibraries
Then install ESPAsyncTCP, ESPAsyncWebServer as zip files from github repos.

After uploading the firmware and then run the ESP8266 Sketch Data Upload.  That requires a special install to get the SPIFFS uploader installed in the Arduino IDE.  But that's how you get the super simple web page installed on the Arduino so you can configure it.  Don't forget that step!

To configure a lamp, hold the button when you plug the lamp in.  The light in the lamp will change to green.  Then you can connect to the PairedLampConfig access point (meaning that you're connecting to the lamp itself, which is acting as the access point) and give it the super secret password 88888888.  The operating system of your phone or computer may probe the access point and determine that you can't get to the Internet through it.  Make sure to respond to that prompt by instructing the operating system to "Connect Anyway" or "Stay Connected".  Otherwise it will just go back to your home WiFi. After you've connected to that access point, browse to http://192.168.4.1/.  That's the IP address of the access point.  You'll get a super simple (ugliest ever) web page that allows you to enter your home WiFi credentials (SSID and password) and then it will tell you that it's ok to unplug the lamp and plug it back in.  When you plug it back in, the lamp will use the SSID and password that you typed on that simple web page to connect to your home WiFi.

If the configuration is correct, then when you plug in the lamp, it will light up blue for about 20 to 60 seconds while it connects to WiFi and the MQTT broker.  When it has successfully connected, the lamp will go dark.

When both lamps are dark, either person can hold the button on their lamp and move their hand toward or away from the top of their lamp (the center of the top of the lamp is where the sensor is hidden). The other lamp will mirror the one that is being controlled.  After the light fades, the lights are available to be controlled again.
