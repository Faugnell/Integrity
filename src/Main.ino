/*!
* @file arduino.c
* @author Victor Petit
* @license GPL
* @brief machine à état pour contrôler l'intégrité d'un sas antivirus
*/

#include <SPI.h>
#include <MFRC522.h>
#include <TinkerKit.h>

// Déclaration des constantes
#define LOOP_DELAY 100
#define RFID_SIZE 20
#define SS_PIN 10
#define RST_PIN 9
#define BADGE_ABSENT 1
#define BADGE_ERROR 2
#define BADGE_OK 0
#define TIMEOUT_INIT 10000
#define TIMEOUT_SCAN 3000
String RFID_ADMIN = "ID BADGE ADMIN"; // mettre le tag du badge administrateur

MFRC522 rfid(SS_PIN, RST_PIN);
byte nuidPICC[255];

enum TLed { ON, OFF, BLINK };
enum  TState { INIT, BADGE_ERROR_INIT, SCAN_IN_PROGRESS, BADGE_ERROR_SCAN, SYSTEM_COMPROMISED };

// Déclaration des variables
TState internalState = INIT;
TLed outputGreenLed = ON;
TLed outputOrangeLed = OFF;
TLed outputRedLed = OFF;
TKLed GreenLed(O3);
TKLed OrangeLed(O5);
TKLed RedLed(O4);
TKButton inputBtn1(I1);
TKButton inputBtn2(I0);
unsigned long InternalRefTime;
int inputStatusReadRfid;
String inputRfidBadge;
String inputRfidBadgeUser;

/*
  @brief vérifie que le delay a été dépassé depuis le temps de référence
  @param ref : le temps de référence
  @param timeDelay : le temps de delais voulu
  @return true si le temps est dépassé
*/
bool EndOfDelay(unsigned long ref, unsigned long timeDelay)
{
  if (millis() - ref > timeDelay)
  {
    return true;
  }
  else
  {
    return false;
  }
}
/*!
* @brief écriture des leds
* @param TLed outputGreenLed : état de sortie de la led verte
* @param TLed outputOrangeLed : état de sortie de la led orange
* @param TLed outputRedLed : état de sortie de la led rouge
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
      if((int)((millis()/500)&1) == true)
      {   
        GreenLed.off();
      }
      else
      {
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
       if((int)((millis()/500)&1) == true)
       {   
         OrangeLed.off();
       }
       else
       {
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
      if((int)((millis()/500)&1) == true)
      {   
        RedLed.off();
      }
      else
      {
        RedLed.on();
      }
      break;
  }
}

/*!
* @brief détection de la présence et lecture du badge
* @param s, pointeur sur une chaine de caractères
* @return BADGE_OK, seulement si un badge est lu et enregistré
* @return BADGE_ABSENT si aucun badge n'est détecté
* @return BADGE_ERROR Si le contenu du badge n'a pas pu être lu
*/
int ReadRfid(String *s)
{
  if ( !rfid.PICC_IsNewCardPresent())
  {
    return BADGE_ABSENT;
  }
  
  if ( !rfid.PICC_ReadCardSerial())
    return BADGE_ERROR;
 
  for (int i = 0; i < rfid.uid.size; i++)
  {
    nuidPICC[i] = rfid.uid.uidByte[i];
    *s = *s + String(rfid.uid.uidByte[i], HEX);
  }
  
  // Halt PICC
  rfid.PICC_HaltA();
  
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  return BADGE_OK;
}

/*!
* @brief initialisation du moniteur et du lecteur RFID
* @param void
* @return void
*/
void setup()
{
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("INIT");
}

/*!
* @brief boucle de traitement qui prend en compte les entrés et sorties
* @param void
* @return void
*/
void loop()
{
  // Lecture des entrées
  inputRfidBadge = "";
  inputStatusReadRfid = ReadRfid(&inputRfidBadge);
  // Traitement
  switch (internalState)
  {
    // Cas d'initialisation  
    case INIT:
      outputGreenLed = ON;  
      outputOrangeLed = OFF;
      outputRedLed = OFF;
      if(inputStatusReadRfid == BADGE_OK)
      {        
        //if(inputBtn1.held() && inputBtn2.held())
        {
          inputRfidBadgeUser = inputRfidBadge;
          InternalRefTime = millis();
          internalState = SCAN_IN_PROGRESS;
          Serial.println("SCAN_IN_PROGRESS");
        }
        //else
        {
          // pas de changement d'état
        }
      }
      else
      {
        // pas de lecture de badge ou erreur
      }      
      break;
    // Cas où le scan est en cours
    case SCAN_IN_PROGRESS:
      
      outputGreenLed = ON;
      outputOrangeLed = BLINK;
      outputRedLed = OFF;
      if(inputStatusReadRfid == BADGE_OK)
      {
        if((inputRfidBadge == inputRfidBadgeUser || inputRfidBadge == RFID_ADMIN) && (EndOfDelay(InternalRefTime, TIMEOUT_INIT) == true))
        {
          inputRfidBadgeUser = "";
          internalState = INIT;
          Serial.println("INIT");
        }
        else
        {
          internalState = BADGE_ERROR_SCAN;
          Serial.println("BADGE_ERROR_SCAN");
          InternalRefTime = millis();
        }
      }
      else
      {
        // pas de lecture de badge ou erreur
      }
      if(inputBtn1.released() || inputBtn2.released())
      {
        internalState = SYSTEM_COMPROMISED;
        Serial.println("SYSTEM_COMPROMISED");
      }
      break;
    // Cas où un badge non valide est passé durant le scan
    case BADGE_ERROR_SCAN:
      outputGreenLed = ON;  
      outputOrangeLed = BLINK;
      outputRedLed = BLINK;
      if(EndOfDelay(InternalRefTime, TIMEOUT_SCAN) == true)
      {
        internalState = SCAN_IN_PROGRESS;
        Serial.println("SCAN_IN_PROGRESS");
      }
      else
      {
        // le timeout n'est pas atteint
      }
      break;
    // Cas ou le système est compromis
    case SYSTEM_COMPROMISED:
      outputGreenLed = OFF;  
      outputOrangeLed = OFF;
      outputRedLed = ON;
      if(inputStatusReadRfid == BADGE_OK)
      {
        if(inputRfidBadge == RFID_ADMIN)
        {
          internalState = INIT;
          Serial.println("INIT");
        }
        else
        {
          // La compromission reste inchangé
        }
      }      
      break;
  }
  
  // Ecriture des sorties
  WriteLeds(outputGreenLed, outputOrangeLed, outputRedLed);

  delay(LOOP_DELAY);
}
