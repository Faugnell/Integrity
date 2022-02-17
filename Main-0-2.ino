/*******************************************************
Role ........ : Système de vérification d'intégrité
********************************************************/

#include <SPI.h>
#include <MFRC522.h>
#include <TinkerKit.h>

// Declaration des constantes
#define LOOP_DELAY 100
#define RFID_SIZE 20
#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN); 
byte nuidPICC[255];

enum TLed { ON, OFF, BLINK };
enum  TState { INIT, BADGE_ERROR_INIT, SCAN_IN_PROGRESS, BADGE_ERROR_SCAN, SYSTEM_COMPROMISED };

// Declaration des variables 
TState internalState = INIT;
TLed outputGreenLed = ON;
TLed outputOrangeLed = OFF;
TLed outputRedLed = OFF;
TKLed GreenLed(O3);
TKLed OrangeLed(O5);
TKLed RedLed(O4);
TKButton btn(I1);
String inputRfid;
String MainID = "432892f65e80";
int count = 0;

// 
void WriteLeds(TLed outputGreenLed, TLed outputOrangeLed, TLed outputRedLed)
{
  // Cas led verte
  switch(outputGreenLed)
  {
    case ON:
      GreenLed.on();
      break;   
    case OFF:
      GreenLed.off();
      break; 
    case BLINK:
      if (GreenLed.state() == HIGH
      {
        GreenLed.off();
      }
      else{
        GreenLed.on();
      }
      break;
  }
  
  // Cas led orange
  switch(outputOrangeLed)
  {
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
  // Cas led rouge
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

// Fonction de la lecture RFID
int ReadRfid(String *s) 
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
  return 0;
}

// Fonction d'initialisation du moniteur et du lecteur RFID
void setup()
{
  Serial.begin(9600);
  SPI.begin(); 
  rfid.PCD_Init(); 
}

// Fonction principale
void loop()
{
  // Lecture des entrées
  String p = "";
  ReadRfid(&p);
  inputRfid = p;
  Serial.print(inputRfid);
  Serial.println(internalState);
  
  // Traitement
  switch (internalState)
  {
    // Cas d'initialisation  
    case INIT:
      if (inputRfid != MainID && inputRfid != "")
      {
        internalState = BADGE_ERROR_INIT;
      }
      if (inputRfid == MainID)
      {
        internalState = SCAN_IN_PROGRESS;
      }
      outputGreenLed = ON;  
      outputOrangeLed = OFF;
      outputRedLed = OFF;
      break;
    // Cas si un badge n'est pas bon lors de la phase d'initialisation
    case BADGE_ERROR_INIT:
      outputGreenLed = ON;  
      outputOrangeLed = OFF;
      outputRedLed = BLINK;
      count++;
      if(count == 30)
      {
        internalState = INIT;
        count = 0;
      }
      break;
    // Cas ou le scan est en cours
    case SCAN_IN_PROGRESS:
      if (inputRfid != MainID && inputRfid != "")
      {
        internalState = BADGE_ERROR_SCAN;
      }
      if (inputRfid == MainID)
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
    // Cas ou un badge non valide est passé durant le scan
    case BADGE_ERROR_SCAN:
      outputGreenLed = ON;  
      outputOrangeLed = BLINK;
      outputRedLed = BLINK;
      count++;
      if(count == 30)
      {
        internalState = INIT;
        count = 0;
      }
      break;
    // Cas ou le systme est compromis
    case SYSTEM_COMPROMISED:
      if (inputRfid == MainID)
      {
        internalState = INIT;
      }
      outputGreenLed = OFF;  
      outputOrangeLed = OFF;
      outputRedLed = ON;
      break;
  }
  // Ecriture des sorties
  WriteLeds(outputGreenLed, outputOrangeLed, outputRedLed);
  // 
  delay(LOOP_DELAY);
}
