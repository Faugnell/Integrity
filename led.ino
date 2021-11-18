#define greenLed 7
#define orangeLed 6
#define redLed 5

void setup() {
  // put your setup code here, to run once:
  
  pinMode(greenLed, OUTPUT);
  pinMode(orangeLed, OUTPUT);
  pinMode(redLed, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(greenLed, HIGH); // Allume la led verte
  delay(1000); // Attends une seconde 
  digitalWrite(greenLed, LOW); // Eteint la led verte
  digitalWrite(orangeLed, HIGH);
  delay(1000);
  digitalWrite(orangeLed, LOW);
  digitalWrite(redLed, HIGH);
  delay(1000);
  digitalWrite(redLed, LOW);

}
