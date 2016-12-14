// Library
  #include "IRremote.h"
  IRsend irsend(A5);
  unsigned int data[67] = {4500, 4550, 500, 1750, 500, 1750, 500, 1700, 550, 600, 500, 600, 550, 600, 500, 600, 500, 650,
                            500, 1750, 500, 1700, 550, 1700, 550, 600, 500, 600, 500, 650, 500, 600, 500, 650, 500, 600,
                            500, 1750, 500, 600, 550, 600, 500, 600, 500, 650, 500, 600, 500, 650, 500, 1750, 500, 600,
                            500, 1750, 500, 1750, 500, 1750, 500, 1750, 500, 1700, 500, 1750, 500};
// Setup
  void setup() {
    //
  }

// Loop
  void loop() {
    //irsend.sendRC5(IRcode, 36);
    //delay(1000);
    //irsend.sendRC6(IRcode, 36);
    //delay(1000);
    //irsend.sendRaw(data2, sizeof(data)/sizeof(data[0]), 38);
    delay(1000);
  }
