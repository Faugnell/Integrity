/*
Auteur : Victor Petit
Licence :
Summary : (pourquoi ce fichier et pas autre chose)
*/

#include <SPI.h> // inclut les bibliothèques RFID
#include <MFRC522.h>
#include <TinkerKit.h> // inclut la bibliothèque TinkerKit

//on défini nos pin
#define SS_PIN 10
#define RST_PIN 9
// definition des pin du RC522
MFRC522 mfrc522(SS_PIN, RST_PIN); 

// on défini les output et input
TKLed greenled(O3);
TKLed yellowled(O5);
TKLed redled(O4);
TKButton button(I1);
// Variable du passage du badge
int passage = 0;

char st[20];

void setup() 
{ 
  // initialisation serie(pour le relevé d'information via le moniteur série) et initialisation des deux bibliotheque SPI et MFRC522
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init(); 
  // Message initial (le message afiché au démarrage dans le moniteur serie de notre programme arduino)
  Serial.println("Test acces via TAG RFID");
  Serial.println();
  // On eteind toutes les leds
  greenled.off();
  yellowled.off();
  redled.off();
}

void loop() // le corp de notre programme
{ 
  // Si le bouton est relaché déclenche la led de présence
  if(button.released() && passage == 1)
  {
    yellowled.on();
  }
  
  // Regarde le numéro de passage pour éteindre ou allumer la led d'état
  switch (passage)
  {
    case 0:
      greenled.off();
      break;
    case 1:
      greenled.on();
      break;
    case 2:
      passage = 0;
      yellowled.off();
      break;
  }
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    // Echec de la lecture RFID donc fin du programme
    return;
  }
  
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  
  Serial.print("UID de tag :");
  String tag= "";
  byte caractere;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     tag.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     tag.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  
  Serial.println();
  Serial.print("Message : ");
  tag.toUpperCase();
  
  // ici on va vérifier l'autorisation
  if (tag.substring(1) == "04 32 89 02 F6 5E 80")
  {
    Serial.println("TAG verifie - Acces Autorise !");
    Serial.println();
    passage += 1;
    delay(1000);
  }
  else
  //sinon si le Tag n'est pas valide
  {
    Serial.println("TAG inconnu - Acces refuse !!!");
    Serial.println();
    // on repete 5fois
    for (int i= 1; i<5 ; i++)
    {
      //LED clignotte ici
      redled.on();
      delay(200);
      redled.off();
      delay(200);
    }
  }
}
