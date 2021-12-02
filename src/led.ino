#include <TinkerKit.h> //On inclut l'utilisation du TinkerKit

// On déclare les OUTPUT des leds
TKLed orangeled(O0); 
TKLed greenled(O1);
TKLed redled(O2);

void setup() {

}

void loop() {
  //utilisation des leds
  orangeled.on();
  delay(1000);
  orangeled.off();
  greenled.on();
  delay(1000);
  greenled.off();
  redled.on();
  delay(1000);
  redled.off();
}
