#include <SPI.h> // SPI
#include <MFRC522.h> // RFID
#include <string.h>
#include <stdio.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN); 
byte nuidPICC[255];

void setup() 
{ 
  Serial.begin(9600);
  SPI.begin(); 
  rfid.PCD_Init(); 
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

void loop()
{
  String p = "";
  srfid(&p);
  Serial.print(p);
  if(p!="")
  {
    if (p== "432892f65e80")
    {
      Serial.println("");
      Serial.print("oui");
      Serial.println("");
    }
    else
    {
      Serial.println("");
      Serial.print("non");
      Serial.println("");
    }
  }
}
