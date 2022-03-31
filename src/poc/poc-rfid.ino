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
#define LOOP_DELAY 1
#define RFID_SIZE 20
#define SS_PIN 10
#define RST_PIN 9
#define BADGE_ABSENT 1
#define BADGE_ERROR 2
#define BADGE_OK 0

MFRC522 rfid(SS_PIN, RST_PIN);
byte nuidPICC[255];

// Déclaration des variables
unsigned long InternalRefTime;
int inputStatusReadRfid;
String inputRfidBadge;
String inputRfidBadgeUser;


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
    return BADGE_ABSENT;
 
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
  if(inputStatusReadRfid == BADGE_OK)
  {
     Serial.println("Lecture badge OK");
  }

  delay(LOOP_DELAY);
}
