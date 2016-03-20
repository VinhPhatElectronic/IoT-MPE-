#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>

//const char* ssid = "BaRiAVT CWalk";
//const char* password = "deocomatkhau";

ESP8266WebServer server(80);
const int led = D4;
const int led1 = D8;
const int led2 = D7;
const int led3 = D6;
const int led4 = D5;
const int led5 = D4;


void handleRoot() {
  reply();digitalWrite(led, 1);
  //server.send(200, "text/plain", ",3700,100,200,300,0,0,0,0,0,0,VinhPhat Electronic");
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){
  pinMode(led, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  digitalWrite(led1, 0);
  digitalWrite(led2, 0);
  digitalWrite(led3, 0);
  digitalWrite(led4, 0);
  digitalWrite(led5, 0);
  
  
  Serial.begin(115200);
  WiFiManager wifiManager;
  wifiManager.setTimeout(180);
  //wifiManager.
  
    //autoConnect("AutoConnectAP");
  if(!wifiManager.autoConnect("AutoConnectAP")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  } 
  //WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  //Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);

  server.on("/control", [](){
    //server.send(200, "text/plain", "this works as well");
    control();
  });

  server.on("/ON1", [](){
    digitalWrite(led1,HIGH);reply();
  });

  server.on("/OFF1", [](){
    digitalWrite(led1,LOW);reply();
  });
  
  server.on("/ONF1", [](){
    digitalWrite(led2,HIGH);reply();
  });
  
  server.on("/OFFF1", [](){
    digitalWrite(led2,LOW);reply();
  });
  
  server.on("/ONSW1", [](){
    digitalWrite(led3,HIGH);reply();
  });
  
  server.on("/OFFSW1", [](){
    digitalWrite(led3,LOW);reply();
  });
  
  server.on("/ONSP1", [](){
    digitalWrite(led4,HIGH);reply();
  });
  
  server.on("/OFFP1", [](){
    digitalWrite(led4,LOW);reply();
  });
  
  server.on("/ONM1", [](){
    digitalWrite(led5,HIGH);reply();
  });
  
  server.on("/OFFM1", [](){
    digitalWrite(led5,LOW);reply();
  });  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}
void reply()
{
  String messageled =",";
  messageled +="3700";//Power
  messageled +=",1400";
  messageled +=",200";//Heap
  messageled +=",300";//Time
  if(digitalRead(led1) == HIGH){messageled +=",1";}else{messageled +=",0";}
  if(digitalRead(led2) == HIGH){messageled +=",1";}else{messageled +=",0";}
  if(digitalRead(led3) == HIGH){messageled +=",1";}else{messageled +=",0";}
  if(digitalRead(led4) == HIGH){messageled +=",1";}else{messageled +=",0";}
  if(digitalRead(led5) == HIGH){messageled +=",1";}else{messageled +=",0";}
  messageled +=",";
  server.send(200, "text/plain",messageled);
 
}
void control()
{
    String HTMLS ="<html><head> <title>ESP8266 Demo</title></head><body>";
//  HTMLS +="3700";//Power
//  HTMLS +=",0";
//  HTMLS +=",200";//Heap
//  HTMLS +=",300";//Time
//  if(digitalRead(led1) == HIGH){HTMLS +=",1";}else{HTMLS +=",0";}
//  if(digitalRead(led2) == HIGH){HTMLS +=",1";}else{HTMLS +=",0";}
//  if(digitalRead(led3) == HIGH){HTMLS +=",1";}else{HTMLS +=",0";}
//  if(digitalRead(led4) == HIGH){HTMLS +=",1";}else{HTMLS +=",0";}
//  if(digitalRead(led5) == HIGH){HTMLS +=",1,";}else{HTMLS +=",0,";}
  HTMLS +="<p>--------------- Control Online ---------------</p>";
  HTMLS +="<p>Light1--<a href=\"ON1\"><button>ON</button></a>&nbsp;<a href=\"OFF1\"><button>OFF</button></a></p>";
  HTMLS +="<p>Fan1----<a href=\"ONF1\"><button>ON</button></a>&nbsp;<a href=\"OFFF1\"><button>OFF</button></a></p>";
  HTMLS +="<p>Switch1-<a href=\"ONSW1\"><button>ON</button></a>&nbsp;<a href=\"OFFSW1\"><button>OFF</button></a></p>";
  HTMLS +="<p>Spack1--<a href=\"ONS1\"><button>ON</button></a>&nbsp;<a href=\"OFFS1\"><button>OFF</button></a></p>";
  HTMLS +="<p>Pump1---<a href=\"ONP1\"><button>ON</button></a>&nbsp;<a href=\"OFFP1\"><button>OFF</button></a></p>";
  HTMLS +="</body></html>";  
  server.send(200, "text/html", HTMLS); 
}
void loop(void){
  server.handleClient();
}
