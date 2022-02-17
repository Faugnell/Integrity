/*!
* @file arduino.c
* @author Victor Petit
* @license GPL
* @brief machine à état pour le control de compromission du scan de disque
*/

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
#define TIMEOUT 3000

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
String MainID = "432892f65e80";
unsigned long currentTime = 0;
unsigned long previousTime = 0;

/*!
* @brief écriture des leds
* @param TLed outputGreenLed, TLed outputOrangeLed, TLed outputRedLed : output des leds
* @return void
*/
void WriteLeds(TLed outputGreenLed, TLed outputOrangeLed, TLed outputRedLed)
{
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

/*!
* @brief lecture du badge et enregistrement du tag
* @param s, pointeur sur une chaine de caractères
* @return BADGE, seulement si un badge est lu et enregistré
*/
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

/*!
* @brief setup le moniteur et le lecteur RFID
* @param void
* @return void
*/
void setup()
{
  Serial.begin(9600);
  SPI.begin(); 
  rfid.PCD_Init(); 
}

/*!
* @brief boucle de traitement qui prend en compte les entrés et sorties
* @param void
* @return void
*/
void loop()
{
  currentTime = millis();
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
      if((currentTime-previousTime) > TIMEOUT)
      {
        previousTime = currentTime;
        internalState = INIT;
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
      if((currentTime-previousTime) > TIMEOUT)
      {
        previousTime = currentTime;
        internalState = INIT;
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
