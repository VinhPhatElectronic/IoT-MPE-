/*
 * IRremoteESP8266: IRrecvDumpV2 - dump details of IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 Sept, 2015
 * Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009, Copyright 2009 Ken Shirriff, http://arcfn.com
 */

#include <EEPROM.h>

#include <IRremoteESP8266.h>

int RECV_PIN = D1; //an IR detector/demodulator is connected to GPIO pin 2

IRrecv irrecv(RECV_PIN);
int led = D4; 
int byte1,byte2,byte3,byte4;
long longByte;
bool learn = LOW;
int times = 0;
int Bits;
void  setup ( )
{
  Serial.begin(9600);   // Status message will be sent to PC at 9600 baud
  irrecv.enableIRIn();  // Start the receiver
  EEPROM.begin(512);
  pinMode(led,OUTPUT);
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



//+=============================================================================
// The repeating section of the code
//
void  loop ( )
{
  decode_results  results;        // Somewhere to store the results
  
  if (irrecv.decode(&results)) {  // Grab an IR code
    dumpInfo(&results);           // Output the results
    Serial.print(EEPROMReadlong(0),HEX);
//    dumpRaw(&results);            // Output the results in RAW format
//    dumpCode(&results);           // Output the results as source code
    Serial.println("");           // Blank line between entries
    irrecv.resume();              // Prepare for the next value
  
 if(learn == LOW&Bits > 0){
  EEPROMWritelong(0, longByte);learn = HIGH;
  }else{
  if(EEPROMReadlong(0) == longByte){Serial.println("OK");}else{Serial.println("Error");}  
    } 
   } 
}
void EEPROMWritelong(int address, long value)
      {
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte4 = (value & 0xFF);
      byte3= ((value >> 8) & 0xFF);
      byte2= ((value >> 16) & 0xFF);
      byte1= ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, byte4);
      EEPROM.write(address + 1, byte3);
      EEPROM.write(address + 2, byte2);
      EEPROM.write(address + 3, byte1);
      EEPROM.commit();
      }
 long EEPROMReadlong(long address)
      {
      //Read the 4 bytes from the eeprom memory.
      long four = EEPROM.read(address);
      long three = EEPROM.read(address + 1);
      long two = EEPROM.read(address + 2);
      long one = EEPROM.read(address + 3);
      EEPROM.commit();
      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
      }     
