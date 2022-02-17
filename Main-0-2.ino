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
#define NO_BADGE 1
#define ERROR_BADGE 2
#define BADGE 0
#define WAIT_ERROR_INIT 30
#define WAIT_ERROR_SCAN 30

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

// Fonction secondaire qui s'occupe de la sortie des leds (allumée/éteinte/clignotante)
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
      if (GreenLed.state() == HIGH)
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
      if (OrangeLed.state() == HIGH)
      {
        OrangeLed.off();
      }
      else{
        OrangeLed.on();
      }
      break;
  }
  // Cas led rouge
  switch(outputRedLed)
  {
    case ON:
      RedLed.on();
      break;   
    case OFF:
      RedLed.off();
      break; 
    case BLINK:
      if (RedLed.state() == HIGH)
      {
        RedLed.off();
      }
      else{
        RedLed.on();
      }
      break;
  }
}

// Fonction secondaire qui s'occupe de savoir si un tag est lu ou non et l'enregistre
int ReadRfid(String *s) 
{
  if ( !rfid.PICC_IsNewCardPresent())
    return NO_BADGE; 
  if ( !rfid.PICC_ReadCardSerial())
    return ERROR_BADGE;
  for (int i = 0; i < rfid.uid.size; i++) 
  {
    nuidPICC[i] = rfid.uid.uidByte[i];
    *s = *s + String(rfid.uid.uidByte[i],HEX);
  }
  return BADGE;
}

// Fonction d'initialisation du moniteur et du lecteur RFID
void setup()
{
  Serial.begin(9600);
  SPI.begin(); 
  rfid.PCD_Init(); 
}

// Fonction principale qui récupère en entrée le tag du badge RFID puis sort la bonne couleur de led suivant le cas
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
    // Cas où un badge n'est pas valide lors de la phase d'initialisation
    case BADGE_ERROR_INIT:
      outputGreenLed = ON;  
      outputOrangeLed = OFF;
      outputRedLed = BLINK;
      count++;
      if(count == WAIT_ERROR_INIT)
      {
        internalState = INIT;
        count = 0;
      }
      break;
    // Cas où le scan est en cours
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
    // Cas où un badge non valide est passé durant le scan
    case BADGE_ERROR_SCAN:
      outputGreenLed = ON;  
      outputOrangeLed = BLINK;
      outputRedLed = BLINK;
      count++;
      if(count == WAIT_ERROR_SCAN)
      {
        internalState = INIT;
        count = 0;
      }
      break;
    // Cas ou le système est compromis
    case SYSTEM_COMPROMISED:
      if (inputRfid == MainID)
      {
        internalState = SCAN_IN_PROGRESS;
      }
      outputGreenLed = OFF;  
      outputOrangeLed = OFF;
      outputRedLed = ON;
      break;
  }
  // Ecriture des sorties
  WriteLeds(outputGreenLed, outputOrangeLed, outputRedLed);
  // Le délais qui permet le clignotement des leds et un retour au début de la loop pour ne pas relire un badge immédiatement
  delay(LOOP_DELAY);
}
