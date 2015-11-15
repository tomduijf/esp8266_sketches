#include <ESP8266WiFi.h>
#include <PubSubClient.h>  // https://github.com/Imroy/pubsubclient
#include <WiFiClient.h>

// Update these with values suitable for your network.
IPAddress mqtt_server(10, 10, 10, 10);

const char *ssid =  "ssid";
const char *pass =  "password";

const char *mqtt_device = "home/test/light_RGB_01";
const char *mqtt_bri    = "home/test/light_RGB_01/Brightness";
const char *mqtt_col    = "home/test/light_RGB_01/Color";
const char *mqtt_sw1    = "home/test/light_RGB_01/Switch1";
const char *mqtt_sw2    = "home/test/light_RGB_01/Switch2";


// RGB FET
#define redPIN    15
#define greenPIN  13
#define bluePIN   12

// W FET - FIXME - No idea what these are for yet.
#define w1PIN     14
#define w2PIN     4

// onbaord green LED D1
#define LEDPIN    5
// onbaord red LED D2
#define LED2PIN   1

// note 
// TX GPIO2 @Serial1 (Serial ONE)
// RX GPIO3 @Serial    


// #####################
// #define PWMRANGE 255
// #define PWM_VALUE 31
//int gamma_table[PWM_VALUE+1] = {
//    0, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 8, 10, 11, 13, 16, 19, 23,
//    27, 32, 38, 45, 54, 64, 76, 91, 108, 128, 152, 181, 215, 255
//};
// #define PWM_VALUE 63
//int gamma_table[PWM_VALUE+1] = {
//    0, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 7, 8, 9, 10,
//    11, 12, 13, 15, 17, 19, 21, 23, 26, 29, 32, 36, 40, 44, 49, 55,
//    61, 68, 76, 85, 94, 105, 117, 131, 146, 162, 181, 202, 225, 250,
//    279, 311, 346, 386, 430, 479, 534, 595, 663, 739, 824, 918, 1023
//};
#define PWM_VALUE 255
int gamma_table[PWM_VALUE+1] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255
};
// #####################

#define LEDoff digitalWrite(LEDPIN,HIGH)
#define LEDon digitalWrite(LEDPIN,LOW)

#define LED2off digitalWrite(LED2PIN,HIGH)
#define LED2on digitalWrite(LED2PIN,LOW)

WiFiClient wclient;
PubSubClient client(wclient,mqtt_server);


void callback(const MQTT::Publish& pub) {
  Serial1.print(pub.topic());
  Serial1.print(" => ");
  Serial1.println(pub.payload_string());
  
  String payload = pub.payload_string();

  if (String(pub.topic()) == mqtt_col) {
    int c1 = payload.indexOf(',');
    int c2 = payload.indexOf(',', c1+1);

    int red = map(payload.toInt(), 0, 255, 0, PWM_VALUE);
    red = constrain(red, 0, PWM_VALUE);
    int green = map(payload.substring(c1+1,c2).toInt(), 0, 255, 0, PWM_VALUE);
    green = constrain(green, 0, PWM_VALUE);
    int blue = map(payload.substring(c2+1).toInt(), 0, 255, 0, PWM_VALUE);
    blue = constrain(blue, 0, PWM_VALUE);
    
    Serial1.print("Received values:");
    Serial1.println("Red: " + String(payload.toInt()) + ", Green: " + String(payload.substring(c1+1,c2).toInt()) + ", Blue: " + String(payload.substring(c2+1).toInt()));
    Serial1.print("Target values:");
    Serial1.println("Red: " + String(red) + ", Green: " + String(green) + ", Blue: " + String(blue));
    Serial1.println("");
  
    red = gamma_table[red];
    green = gamma_table[green];
    blue = gamma_table[blue];

    analogWrite(redPIN, red);
    analogWrite(greenPIN, green);
    analogWrite(bluePIN, blue);
  }
  else if (String(pub.topic()) == mqtt_sw1) {
    int value = map(payload.toInt(),0,255,0,PWM_VALUE);
    value = constrain(value,0,PWM_VALUE);
    value = gamma_table[value];
    analogWrite(w1PIN, value);
    //Serial1.println(value);
  }
  else if (String(pub.topic()) == mqtt_sw2) {
    int value = map(payload.toInt(),0,255,0,PWM_VALUE);
    value = constrain(value,0,PWM_VALUE);
    value = gamma_table[value];
    analogWrite(w2PIN, value);
    //Serial1.println(value);
  }
}


void setup()
{
  pinMode(LEDPIN, OUTPUT);  
  pinMode(LED2PIN, OUTPUT);  
  
  pinMode(redPIN, OUTPUT);
  pinMode(greenPIN, OUTPUT);
  pinMode(bluePIN, OUTPUT);
  pinMode(w1PIN, OUTPUT);
  pinMode(w2PIN, OUTPUT);    
  
  // Setup console
  Serial1.begin(115200);
  delay(10);
  Serial1.println();
  Serial1.println();

  client.set_callback(callback);

  WiFi.begin(ssid, pass);

  LEDon;
  
  while (WiFi.status() != WL_CONNECTED) {
    LED2off;
    delay(500);
    Serial1.print(".");
    LED2on;
  }

  Serial1.println("");

  Serial1.println("WiFi connected");
  Serial1.println("IP address: ");
  Serial1.println(WiFi.localIP());

  if (client.connect(mqtt_device)) {
    client.subscribe(mqtt_col);
    client.subscribe(mqtt_sw1);
    client.subscribe(mqtt_sw2);
    Serial1.println("MQTT connected:");  

    Serial1.println(mqtt_device);
    Serial1.println(mqtt_bri);
    Serial1.println(mqtt_col);
    Serial1.println(mqtt_sw1);
    Serial1.println(mqtt_sw2);
  }

  Serial1.println("");
}


void loop()
{
  client.loop();
}

