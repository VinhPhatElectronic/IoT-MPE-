#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <MQTT.h>
#include <EEPROM.h>

#define DEBUG

//#define AP_SSID     "cafemoc1"
//#define AP_PASSWORD "888888888"  

#define EIOTCLOUD_USERNAME "VinhPhatElectronic"
#define EIOTCLOUD_PASSWORD "namchaubonbien"

// create MQTT object
#define EIOT_CLOUD_ADDRESS "cloud.iot-playground.com"
MQTT myMqtt("", EIOT_CLOUD_ADDRESS, 1883);

#define CONFIG_START 0
#define CONFIG_VERSION "v01"

struct StoreStruct {
  // This is for mere detection if they are your settings
  char version[4];
  // The variables of your settings
  uint moduleId;  // module id
  bool state;     // state
} storage = {
  CONFIG_VERSION,
  // The default module 0
  0,
  0 // off
};

bool stepOk = false;
String instanceId = "";
int buttonState;

boolean result;
String topic("");
String valueStr("");

boolean switchState;
const int buttonPin = 0;  
const int outPin = D1;  
const int LEDB = 2; 
int lastButtonState = LOW; 

void setup() 
{  
#ifdef DEBUG
  Serial.begin(115200);
#endif
  delay(1000);

  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);
  
  EEPROM.begin(512);

  switchState = false;  

  pinMode(LEDB, OUTPUT);
  digitalWrite(LEDB, HIGH); 
#ifndef DEBUG
  pinMode(outPin, OUTPUT); 
  digitalWrite(outPin, HIGH);
#endif  
  loadConfig();

#ifdef DEBUG
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
//  Serial.println(AP_SSID);
#endif  
  WiFiManager wifiManager;
  wifiManager.setTimeout(60);
  if(!wifiManager.autoConnect("Wifi Settup Device1")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }
//  WiFi.begin(AP_SSID, AP_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef DEBUG
    Serial.print(".");
#endif
  }

#ifdef DEBUG
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Connecting to MQTT server");  
#endif
  //set client id
  // Generate client name based on MAC address and last 8 bits of microsecond counter
  String clientName;
  //clientName += "esp8266-";
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += macToStr(mac);
  clientName += " - ";
  clientName += String(micros() & 0xff, 16);
  myMqtt.setClientId((char*) clientName.c_str());

#ifdef DEBUG
  Serial.print("MQTT client id:");
  Serial.println(clientName);
#endif
  // setup callbacks
  myMqtt.onConnected(myConnectedCb);
  //myMqtt.onData(myDataCb);
  myMqtt.onDisconnected(myDisconnectedCb);
  //myMqtt.onData(myDataCb);
  myMqtt.onPublished(myPublishedCb);
  myMqtt.onData(myDataCb);
  
  Serial.println("connect mqtt...");
  myMqtt.setUserPwd(EIOTCLOUD_USERNAME, EIOTCLOUD_PASSWORD);  
  myMqtt.connect();

  delay(500);
  
  //get instance id
  Serial.println("suscribe: Db/InstanceId");
  myMqtt.subscribe("/Db/InstanceId");

  waitOk();delay(3000);

#ifdef DEBUG
  Serial.print("ModuleId: ");
  Serial.println(storage.moduleId);
#endif

  //create module if necessary 
  if (storage.moduleId == 1)
  {
    //create module
#ifdef DEBUG
    Serial.println("create module: Db/"+instanceId+"/NewModule");
#endif
    myMqtt.subscribe("/Db/"+instanceId+"/NewModule");myMqtt.onData(myDataCb);
    waitOk();delay(300);
    // create Sensor.Parameter1
#ifdef DEBUG    
    Serial.println("/Db/"+instanceId+"/"+String(storage.moduleId)+ "/Sensor.Parameter1/NewParameter");    
#endif
    myMqtt.subscribe("/Db/"+instanceId+"/"+String(storage.moduleId)+ "/Sensor.Parameter1/NewParameter");
    waitOk();

    // set module type
#ifdef DEBUG        
    Serial.println("Set module type");    
#endif
    valueStr = "MT_DIGITAL_OUTPUT";
    topic  = "/Db/" + instanceId + "/" + String(storage.moduleId) + "/ModuleType";
    result = myMqtt.publish(topic, valueStr);
    delay(100);

    // save new module id
    saveConfig();
  }

  
  //publish switch state
  valueStr = String(!storage.state);
  topic  = "/Db/"+instanceId+"/"+String(storage.moduleId)+ "/Sensor.Parameter1";
  result = myMqtt.publish(topic, valueStr);

  switchState = storage.state;
  
  myMqtt.subscribe("/Db/"+instanceId+"/"+String(storage.moduleId)+ "/Sensor.Parameter1");
}

void loop() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef DEBUG        
    Serial.print(".");
#endif
  }

  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    if (reading == LOW)
    {
      switchState = !switchState;            
#ifdef DEBUG
      Serial.println("button pressed");
#endif
    }

    valueStr = String(switchState);
    topic  = "/Db/"+instanceId+"/"+String(storage.moduleId)+ "/Sensor.Parameter1";
    result = myMqtt.publish(topic, valueStr);

    delay(200);


#ifdef DEBUG
      Serial.print("set state: ");
      Serial.println(valueStr);
#endif  

    lastButtonState = reading;
  }
  

  digitalWrite(LEDB, switchState); 
#ifndef DEBUG  
  digitalWrite(outPin, switchState); 
#endif
  if (switchState != storage.state)
  {
    storage.state = switchState;
    // save button state
    saveConfig();
  }  
}


void waitOk()
{
  while(stepOk){
    delay(1000);
  stepOk = false;}
}

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}


/*
 * 
 */
void myConnectedCb() {
#ifdef DEBUG
  Serial.println("connected to MQTT server");
#endif
  myMqtt.subscribe("/Db/" + instanceId + "/" + String(storage.moduleId) + "/Sensor.Parameter1");
}

void myDisconnectedCb() {
#ifdef DEBUG
  Serial.println("disconnected. try to reconnect...");
#endif
  delay(500);
  myMqtt.connect();
}

void myPublishedCb() {
#ifdef DEBUG  
  Serial.println("published.");
#endif
}

void myDataCb(String& topic, String& data) {  
#ifdef DEBUG  
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(data);
#endif
  if (topic == String("/Db/InstanceId"))
  {
    instanceId = data;
    stepOk = true;
  }
  else if (topic ==  String("/Db/"+instanceId+"/NewModule"))
  {
    storage.moduleId = data.toInt();
    stepOk = true;
  }
  else if (topic == String("/Db/"+instanceId+"/"+String(storage.moduleId)+ "/Sensor.Parameter1/NewParameter"))
  {
    stepOk = true;
  }
  else if (topic == String("/Db/"+instanceId+"/"+String(storage.moduleId)+ "/Settings.Icon1/NewParameter"))
  {
    stepOk = true;
  }
  else if (topic == String("/Db/"+instanceId+"/"+String(storage.moduleId)+ "/Sensor.Parameter1"))
  {
    switchState = (data == String("1"))? true: false;

#ifdef DEBUG      
    Serial.print("switch state ");
    Serial.println(switchState);
#endif
  }
}

void loadConfig() {
  // To make sure there are settings, and they are YOURS!
  // If nothing is found it will use the default settings.
  if (EEPROM.read(CONFIG_START + 0) == CONFIG_VERSION[0] &&
      EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[1] &&
      EEPROM.read(CONFIG_START + 2) == CONFIG_VERSION[2])
    for (unsigned int t=0; t<sizeof(storage); t++)
      *((char*)&storage + t) = EEPROM.read(CONFIG_START + t);
}

void saveConfig() {
  for (unsigned int t=0; t<sizeof(storage); t++)
    EEPROM.write(CONFIG_START + t, *((char*)&storage + t));

  EEPROM.commit();
}
