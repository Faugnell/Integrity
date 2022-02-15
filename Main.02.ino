#include <SPI.h> // RFID
#include <MFRC522.h> // RFID
#include <TinkerKit.h> // TinkerKit

#define LOOP_DELAY 100  // période de scrutation et de clignottement
#define RFID_SIZE 20
#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN); 
byte nuidPICC[255];

enum TLed { ON, OFF, BLINK };
enum  TState { INIT, BADGE_ERROR_INIT, SCAN_IN_PROGRESS, BADGE_ERROR_SCAN, SYSTEM_COMPROMISED };

TState internalState = INIT;
TLed outputGreenLed = ON;
TLed outputOrangeLed = OFF;
TLed outputRedLed = OFF;
TKLed GreenLed(O3);
TKLed OrangeLed(O5);
TKLed RedLed(O4);
TKButton btn(I1);
String inputRfid;

void WriteLeds(TLed outputGreenLed, TLed outputOrangeLed, TLed outputRedLed){
  switch(outputGreenLed) {
    case ON:
      GreenLed.on();
      break;   
    case OFF:
      GreenLed.off();
      break; 
    case BLINK:
      if (GreenLed.state() == HIGH){
        GreenLed.off();
      }
      else{
        GreenLed.on();
      }
      break;
  }
  switch(outputOrangeLed) {
    case ON:
      OrangeLed.on();
      break;   
    case OFF:
      OrangeLed.off();
      break; 
    case BLINK:
      if (OrangeLed.state() == HIGH){
        OrangeLed.off();
      }
      else{
        OrangeLed.on();
      }
      break;
  }
  switch(outputRedLed) {
    case ON:
      RedLed.on();
      break;   
    case OFF:
      RedLed.off();
      break; 
    case BLINK:
      if (RedLed.state() == HIGH){
        RedLed.off();
      }
      else{
        RedLed.on();
      }
      break;
  }
}

int srfid(String *s) 
{
  if ( !rfid.PICC_IsNewCardPresent())
    return 1; 
  if ( !rfid.PICC_ReadCardSerial())
    return 2;
  for (int i = 0; i < rfid.uid.size; i++) 
  {
    nuidPICC[i] = rfid.uid.uidByte[i];
    *s = *s + String(rfid.uid.uidByte[i],HEX);
  }
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  return 0;
}

void setup()
{
  Serial.begin(9600);
  SPI.begin(); 
  rfid.PCD_Init(); 
}

void loop()
{
  // lecture RFID
  String p = "";
  srfid(&p);
  inputRfid = p;
  Serial.print(inputRfid);
  Serial.println(internalState);
  
  // Traitement
  switch (internalState) {
    case INIT:
      if (inputRfid != "432892f65e80" && inputRfid != "") {
        internalState = BADGE_ERROR_INIT;
      }
      if (inputRfid == "432892f65e80") {
        internalState = SCAN_IN_PROGRESS;
      }
      outputGreenLed = ON;  
      outputOrangeLed = OFF;
      outputRedLed = OFF;
      break;
    case BADGE_ERROR_INIT:
      if (inputRfid == "432892f65e80")
        {
          internalState = INIT;
        }
      outputGreenLed = ON;  
      outputOrangeLed = OFF;
      outputRedLed = BLINK;
      break;
    case SCAN_IN_PROGRESS:
      if (inputRfid != "432892f65e80" && inputRfid != "")
      {
        internalState = BADGE_ERROR_SCAN;
      }
      if (inputRfid == "432892f65e80")
      {
        internalState = INIT;
      }
      if(btn.released())
      {
        Serial.print("Compromis !!");
        internalState = SYSTEM_COMPROMISED;
      }
      outputGreenLed = ON;
      outputOrangeLed = BLINK;
      outputRedLed = OFF;
      break;
    case BADGE_ERROR_SCAN:
      if (inputRfid == "432892f65e80")
      {
        internalState = INIT;
      }
      outputGreenLed = ON;  
      outputOrangeLed = BLINK;
      outputRedLed = BLINK;
      break;
    case SYSTEM_COMPROMISED:
      if (inputRfid == "432892f65e80")
      {
        internalState = INIT;
      }
      outputGreenLed = OFF;  
      outputOrangeLed = OFF;
      outputRedLed = ON;
      break;
  }
  
  WriteLeds(outputGreenLed, outputOrangeLed, outputRedLed);
  
  delay(LOOP_DELAY);
}