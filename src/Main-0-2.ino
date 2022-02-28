/*!

* @file arduino.c
* @author Victor Petit
* @license GPL
* @brief machine à état pour le control de compromission du scan de disque
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
#define TIMEOUT 3000

MFRC522 rfid(SS_PIN, RST_PIN);
byte nuidPICC[255];

enum TLed { ON, OFF, BLINK };
enum  TState { INIT, BADGE_ERROR_INIT, SCAN_IN_PROGRESS, BADGE_ERROR_SCAN, SYSTEM_COMPROMISED };

// Déclaration des variables
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
unsigned long currentTime;
unsigned long refTime;

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
      if (GreenLed.state() == HIGH)
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
      if (OrangeLed.state() == HIGH)
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
      if (RedLed.state() == HIGH)
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
* @return BADGE_OK(0), seulement si un badge est lu et enregistré
*/
int ReadRfid(String *s)
{
  if ( !rfid.PICC_IsNewCardPresent())
    return BADGE_ABSENT;
 
  if ( !rfid.PICC_ReadCardSerial())
    return BADGE_ERROR;
 
  for (int i = 0; i < rfid.uid.size; i++)
  {
    nuidPICC[i] = rfid.uid.uidByte[i];
    *s = *s + String(rfid.uid.uidByte[i], HEX);
  }
  return BADGE_OK;
}

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
* @brief initialisation du moniteur et du lecteur RFID
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
  // Lecture des entrées
  String rfidBadge = "";
  int statusReadRfid = ReadRfid(&rfidBadge);
  inputRfid = rfidBadge;
  Serial.print(inputRfid);
  // Traitement
  switch (internalState)
  {
    // Cas d'initialisation  
    case INIT:
      Serial.println("INIT");
      if(statusReadRfid == BADGE_OK)
      {
        if(rfidBadge == MainID)
        {
          internalState = SCAN_IN_PROGRESS;
        }
        else
        {
          internalState = BADGE_ERROR_INIT;
          refTime = millis();
        }
      }
      else
      {
        // pas de lecture de badge ou erreur
      }
      outputGreenLed = ON;  
      outputOrangeLed = OFF;
      outputRedLed = OFF;
      break;
    // Cas où un badge n'est pas valide lors de la phase d'initialisation
    case BADGE_ERROR_INIT:
      Serial.println("BADGE_ERROR_INIT");
      outputGreenLed = ON;  
      outputOrangeLed = OFF;
      outputRedLed = BLINK;
      if(EndOfDelay(refTime, TIMEOUT) == true)
      {
        internalState = INIT;
      }
      else
      {
        // le timeout n'est pas atteint
      }
      break;
    // Cas où le scan est en cours
    case SCAN_IN_PROGRESS:
      Serial.println("SCAN_IN_PROGRESS");
      if(statusReadRfid == BADGE_OK)
      {
        if(rfidBadge == MainID)
        {
          internalState = INIT;
        }
        else
        {
          internalState = BADGE_ERROR_SCAN;
          refTime = millis();
        }
      }
      if(btn.released())
      {
        internalState = SYSTEM_COMPROMISED;
      }
      outputGreenLed = ON;
      outputOrangeLed = BLINK;
      outputRedLed = OFF;
      break;
    // Cas où un badge non valide est passé durant le scan
    case BADGE_ERROR_SCAN:
      Serial.println("BADGE_ERROR_SCAN");
      outputGreenLed = ON;  
      outputOrangeLed = BLINK;
      outputRedLed = BLINK;
      if(EndOfDelay(refTime, TIMEOUT) == true)
      {
        internalState = SCAN_IN_PROGRESS;
      }
      else
      {
        // le timeout n'est pas atteint
      }
      break;
    // Cas ou le système est compromis
    case SYSTEM_COMPROMISED:
      Serial.println("SYSTEM_COMPROMISED");
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
  // Période de la boucle principale et de la clignottement
  delay(LOOP_DELAY);
}
