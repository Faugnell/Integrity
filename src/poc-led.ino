#include <TinkerKit.h> // TinkerKit

enum TLed { ON, OFF, BLINK };

TKLed RedLed(O4);
TKLed OrangeLed(O5);
TKLed GreenLed(O3);

TLed outputGreenLed = BLINK;
TLed outputOrangeLed = BLINK;
TLed outputRedLed = BLINK;

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  WriteLeds(outputGreenLed, outputOrangeLed, outputRedLed);
  delay(250);
}

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
