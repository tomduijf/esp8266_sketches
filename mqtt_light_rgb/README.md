# MQTT RGB Light - led controller

Inspired by
 * http://chaozlabs.blogspot.de/2015/08/esp8266-in-wild-wifi-led-controller-hack.html

### What is this?
This sketch is for controlling a multicolor lightstrip (RGB, whites or combined), through automation tools like HomeAssistant, OpenHAB, etc. This written for a prefab controller with an ESP8266 microcontroler (see parts below), but can also be used for a self built controller.

### Main configurables:
If you use the exact print, you can probably get away by only changing the following paramters
```
// Update these with values suitable for your network.
IPAddress mqtt_server(10, 10, 10, 10);

const char *ssid =  "ssid";    // cannot be longer than 32 characters!
const char *pass =  "password";    //

const char *mqtt_device = "/HASS/light_RGB_01";
const char *mqtt_bri = *mqtt_device + "/Brightness";
const char *mqtt_col = *mqtt_device + "/Color";
const char *mqtt_sw1 = *mqtt_device + "/Switch1";
const char *mqtt_sw2 = *mqtt_device + "/Switch2";
```

### A WiFi RGB LED controller, using MQTT, controlled by HASS
My parts?
 * http://www.aliexpress.com/item/rgb-WiFi-controller-1-port-control-15-rgb-lights-communicate-with-Android-phone-to-dim-output/32301423622.html
 * http://www.aliexpress.com/item/Black-PCB-LED-Strip-5050-DC12V-Black-PCB-Board-IP65-Waterproof-60LED-m-5m-300LED-RGB/2033629286.html
or
 * http://www.aliexpress.com/item/5050-Double-Color-Temperature-LED-Strip-One-Chip-with-Warm-White-and-Cold-White-DC12V-60LED/32298725565.html

