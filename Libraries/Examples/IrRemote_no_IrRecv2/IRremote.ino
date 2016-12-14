#include "IRremote.h"
int LEDpin = D7;
int IRpin = A5;
  IRsend irsend(IRpin);
    unsigned int data[67] = {4500, 4550, 500, 1750, 500, 1750, 500, 1700, 550, 600, 500, 600, 550, 600, 500, 600, 500, 650,
                            500, 1750, 500, 1700, 550, 1700, 550, 600, 500, 600, 500, 650, 500, 600, 500, 650, 500, 600,
                            500, 1750, 500, 600, 550, 600, 500, 600, 500, 650, 500, 600, 500, 650, 500, 1750, 500, 600,
                            500, 1750, 500, 1750, 500, 1750, 500, 1750, 500, 1700, 500, 1750, 500};
    unsigned int data2[26] = {884,884,1768,884,884,884,884,884,884,884,884,884,884,1768,884,884,884,884,884,884,884,884,884,884,884,884};
// Setup
  void setup() {
     pinMode(LEDpin,OUTPUT);
     pinMode(IRpin,OUTPUT);
  }

// Loop
  void loop() {
    digitalWrite(LEDpin, HIGH);
    irsend.sendRaw(data, 67, 36);
    digitalWrite(LEDpin, LOW);
    delay(5000);
    digitalWrite(LEDpin, HIGH);
    irsend.sendRaw(data2, 26, 38);
    digitalWrite(LEDpin, LOW);
    delay(5000);
  }
