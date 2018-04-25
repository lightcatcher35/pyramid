/*
  DigitalReadSerial

  Reads a digital input on pin 2, prints the result to the Serial Monitor

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/DigitalReadSerial
*/

// digital pin 2 has a pushbutton attached to it. Give it a name:
int buton1 = 2;
int buton2 = 3;
int buton3 = 4;
int buton4 = 5;
String metin;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(buton1, OUTPUT);
  pinMode(buton2, OUTPUT);
  pinMode(buton3, OUTPUT);
  pinMode(buton4, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input pin:
  if(digitalRead(buton1)==true)
  {
    metin="1. butona basıldı";
    
  }else if(digitalRead(buton2)==true)
  {
    metin="2. butona basıldı";
    
  }else if(digitalRead(buton3)==true)
  {
    metin="3. butona basıldı";
    
  }else if(digitalRead(buton4)==true)
  {
    metin="4. butona basıldı";
    
  }
  // print out the state of the button:
  Serial.println(metin);
  delay(100);        // delay in between reads for stability
}
