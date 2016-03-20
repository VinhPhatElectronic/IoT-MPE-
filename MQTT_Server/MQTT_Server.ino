//****************************************************/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

ESP8266WebServer server(80);
/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "VinhPhatElectronic"
#define AIO_KEY         "7fe188a8e80c7370e3592712f2cce4c5f53e0489"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Store the MQTT server, username, and password in flash memory.
// This is required for using the Adafruit MQTT library.
const char MQTT_SERVER[] PROGMEM    = AIO_SERVER;
const char MQTT_USERNAME[] PROGMEM  = AIO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = AIO_KEY;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_USERNAME, MQTT_PASSWORD);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
const char PHOTOCELL_FEED[] PROGMEM = AIO_USERNAME "/feeds/photocell";
Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, PHOTOCELL_FEED);

// Setup a feed called 'feelback' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
const char LEDFb_FEED[] PROGMEM = AIO_USERNAME "/feeds/feelback";
Adafruit_MQTT_Publish feelback = Adafruit_MQTT_Publish(&mqtt, LEDFb_FEED);


// Setup a feed called 'onoff' for subscribing to changes.
const char ONOFF_FEED[] PROGMEM = AIO_USERNAME "/feeds/onoff";
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, ONOFF_FEED);

// Setup a feed called 'Light' for subscribing to changes.
const char LIGHT_FEED[] PROGMEM = AIO_USERNAME "/feeds/Light";
Adafruit_MQTT_Subscribe LIGHTCONTROL = Adafruit_MQTT_Subscribe(&mqtt, LIGHT_FEED);
/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

#define LEDB 2
bool LEDf;

void setup() {
  Serial.begin(115200);
  
  WiFiManager wifiManager;
  wifiManager.setTimeout(180);
  if(!wifiManager.autoConnect("Wifi Settup Device1")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }   
  
  delay(10);
  Serial.println(); 
  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
Serial.println("SSID in EEProm");

//  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&LIGHTCONTROL);
  mqtt.subscribe(&onoffbutton);
  
 pinMode(LEDB,OUTPUT);
}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(1000))) {
    if (subscription == &LIGHTCONTROL) {
      Serial.print(F("GotLight: "));
      Serial.println((char *)LIGHTCONTROL.lastread);
    }
    if (subscription == &onoffbutton) {
      Serial.print(F("GotLED: "));
      Serial.print((char *)onoffbutton.lastread);
     }
    if (strcmp((char *)onoffbutton.lastread, "OFF")) {
        digitalWrite(LEDB, LOW); 
      }
      if (strcmp((char *)onoffbutton.lastread, "ON")) {
        digitalWrite(LEDB, HIGH); 
      }
    
  // Now we can publish stuff!
  Serial.print(F("\nLEDB "));
  if(digitalRead(LEDB) == LOW){Serial.print("ON");}
  if(digitalRead(LEDB) == HIGH){Serial.print("OFF");}
  Serial.print("...");
  if(digitalRead(LEDB) == LOW){if (! feelback.publish("ON")) {Serial.println(F("Failed"));} else {Serial.println(F("OK!"));}}
  if(digitalRead(LEDB) == HIGH){if (! feelback.publish("OFF")) {Serial.println(F("Failed"));} else {Serial.println(F("OK!"));}}

  }
  

  // Now we can publish stuff!
  Serial.print(F("\nSending photocell val "));
  Serial.print(x);
  Serial.print("...");
  if (! photocell.publish(x++)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
