#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>

#include <EEPROM.h>

#include <IRremoteESP8266.h>

int RECV_PIN = D1; //an IR detector/demodulator is connected to GPIO pin 2
IRsend irsend(D2); //an IR led is connected to GPIO pin 0
IRrecv irrecv(RECV_PIN); 
int byte1,byte2,byte3,byte4;
long longByte;
bool learn = HIGH;
int times = 0;
int Bits;


ESP8266WebServer server(80);
const int led = D4;
const int led1 = D8;
const int led2 = D7;
const int led3 = D6;
const int led4 = D5;
const int led5 = D4;


void handleRoot() {
  control();//reply();digitalWrite(led, 1);
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

//    Serial.begin(9600);   // Status message will be sent to PC at 9600 baud
  irrecv.enableIRIn();  // Start the receiver
  EEPROM.begin(512);
  
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

  server.on("/learn", [](){
    learn =  LOW;
    learns();
  });

  server.on("/ON1", [](){
    irsend.begin();
    
    if(EEPROM.read(4) == 12){irsend.sendSony(EEPROMReadlong(0), 12);}
    if(EEPROM.read(4) == 16){irsend.sendNEC(EEPROMReadlong(0), 16);}
    if(EEPROM.read(4) == 32){irsend.sendNEC(EEPROMReadlong(0), 32);}
    if(EEPROM.read(4) == 36){irsend.sendNEC(EEPROMReadlong(0), 36);}
//    digitalWrite(led1,HIGH);reply();
    control();
  });

  server.on("/OFF1", [](){
    irsend.begin();
    if(EEPROM.read(4) == 12){irsend.sendSony(EEPROMReadlong(0), 12);}
    if(EEPROM.read(4) == 16){irsend.sendNEC(EEPROMReadlong(0), 16);}
    if(EEPROM.read(4) == 32){irsend.sendNEC(EEPROMReadlong(0), 32);}
    if(EEPROM.read(4
    ) == 36){irsend.sendNEC(EEPROMReadlong(0), 36);}
    control();
//    digitalWrite(led1,LOW);reply();
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
  HTMLS +="<p>Power--<a href=\"ON1\"><button>ON</button></a>&nbsp;<a href=\"OFF1\"><button>OFF</button></a></p>";
  HTMLS +="</body></html>";  
  server.send(200, "text/html", HTMLS); 
}

void learns()
{
    String HTMLS ="<html><head> <title>ESP8266 Demo</title></head><body>";
  HTMLS +="<p>--------------- Learn Program ---------------</p>";
  HTMLS +="<p> huong remote vao board de cai dat lenh POWER (bat/tat thiet bi)</p>";
  HTMLS +="</body></html>";  
  server.send(200, "text/html", HTMLS); 
}

//+=============================================================================
// Display IR code


//
void  ircode (decode_results *results)
{
  // Panasonic has an Address
  if (results->decode_type == PANASONIC) {
    Serial.print(results->panasonicAddress, HEX);
    Serial.print(":");
  }

  // Print Code
  Serial.print(results->value, HEX);
  longByte = results->value;
}
//+=============================================================================
// Display encoding type
//
void  encoding (decode_results *results)
{
  switch (results->decode_type) {
    default:
    case UNKNOWN:      Serial.print("UNKNOWN");       break ;
    case NEC:          Serial.print("NEC");           break ;
    case SONY:         Serial.print("SONY");          break ;
    case RC5:          Serial.print("RC5");           break ;
    case RC6:          Serial.print("RC6");           break ;
    case DISH:         Serial.print("DISH");          break ;
    case SHARP:        Serial.print("SHARP");         break ;
    case JVC:          Serial.print("JVC");           break ;
    case SANYO:        Serial.print("SANYO");         break ;
    case MITSUBISHI:   Serial.print("MITSUBISHI");    break ;
    case SAMSUNG:      Serial.print("SAMSUNG");       break ;
    case LG:           Serial.print("LG");            break ;
    case WHYNTER:      Serial.print("WHYNTER");       break ;
    case AIWA_RC_T501: Serial.print("AIWA_RC_T501");  break ;
    case PANASONIC:    Serial.print("PANASONIC");     break ;
  }
}

//+=============================================================================
// Dump out the decode_results structure.
//
void  dumpInfo (decode_results *results)
{
//  // Show Encoding standard
//  Serial.print("Encoding  : ");
//  encoding(results);
//  Serial.println("");

  // Show Code & length
  Serial.print("Code      : ");
  ircode(results);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Bits = results->bits;
}


void learncode()
{
  
  decode_results  results;        // Somewhere to store the results

  if (irrecv.decode(&results)&learn == LOW) {  // Grab an IR code
    dumpInfo(&results);           // Output the results
    Serial.print(EEPROMReadlong(0),HEX);
//    dumpRaw(&results);            // Output the results in RAW format
//    dumpCode(&results);           // Output the results as source code
    Serial.println("");           // Blank line between entries
    irrecv.resume();              // Prepare for the next value
  
 
  EEPROMWritelong(0, longByte);learn = HIGH;
  EEPROM.write(4,Bits);
  if(EEPROMReadlong(0) == longByte){Serial.println("OK");}else{Serial.println("Error");}  
  control(); 
    } 
   
}

void EEPROMWritelong(int address, long value)
      {
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);
      }
long EEPROMReadlong(long address)
      {
      //Read the 4 bytes from the eeprom memory.
      long four = EEPROM.read(address);
      long three = EEPROM.read(address + 1);
      long two = EEPROM.read(address + 2);
      long one = EEPROM.read(address + 3);

      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
      }   
//////////////////////////////////////////////////////////////////////////////////////

void loop(void){
  server.handleClient();
  if(learn == LOW)learncode();
}
