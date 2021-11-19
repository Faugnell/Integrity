#include <TinkerKit.h>

TKButton button(I0);	// creating the object 'button' that belongs to the 'TKButton' class 
                        // and giving the value to the desired input pin

TKLed led(O0);		// creating the object 'led' that belongs to the 'TKLed' class 
                        // and giving the value to the desired output pin


void setup() {
//nothing here
}

void loop()
{
  // check the switchState of the button
  if (button.readSwitch() == HIGH) {	  
    led.on();			 
  } 
  else {			
    led.off();			
  }
}
