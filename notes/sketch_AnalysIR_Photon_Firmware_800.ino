//* AnalysIR Firmware
//* Date: 1st August 2015
//* Release: 1.0.x
//* Note: Please use and read in conjunction with the README and the AnalysIR Getting Started Guide.
//* Licence: Free to use & modify without restriction or warranty.
//*          Please acknowledge AnalysIR as the Author in any derivative and include a link to http://www.AnalysIR.com in any publication.
//*
//*
//* This is a port of the original Arduino code. An explanation of the Arduino code is available in the README and is applicable to most parts of this ported code.
//* This version uses a serial USB connection. A WiFI version may be made available in future, if there is sufficient interest and once the Photon system firmware is more robust.


#define Photon true        //
//............................................................................................

//Definitions for Buffers here...size RAM dependent
#define modPULSES 256 //increase until memory is used up, max 256, leave at 256.


//Baud rate is now fixed to 115200 for all devices, to avoid issues with some platforms
#define BAUDRATE 115200


#if Photon
#define IR_Rx_PIN D2   //connect the IR receiver to this Pin
#define IR_Mod_PIN D3  //connect the IR Learner to this Pin
#define ledPin    D7
#define pin2HIGH digitalRead(IR_Rx_PIN)
#define maxPULSES 1024 //More RAM is available on the Photon, increase if neccessary (unlikely)
#endif
//...............................................................................................


unsigned int pulseIR[maxPULSES]; //temp store for pulse durations (demodulated)
volatile byte modIR[modPULSES]; //temp store for modulation pulse durations - changed in interrupt

//General variables
volatile byte state = 127; //defines initial value for state normally HIGH or LOW. Set in ISR.
byte oldState = 127; //set both the same to start. Used to test for change in state signalled from ISR
unsigned long usLoop, oldTime; //timer values in uSecs. usLoops stores the time value of each loop, oldTime remembers the last time a state change was received
volatile unsigned long newMicros;//passes the time a state change occurred from ISR to main loop
unsigned long oldMicros = 0; //passes the time a state change occurred from ISR to main loop
unsigned int countD = 0; // used as a pointer through the buffers for writing and reading (de-modulated signal)
volatile byte countM = 0; // used as a pointer through the buffers for writing and reading (modulated signal)
byte countM2 = 0; //used as a counter, for the number of modulation samples used in calculating the period.
unsigned int i = 0; //used to iterate in for loop...integer
byte j = 0; //used to iterate in for loop..byte
unsigned long sum = 0; //used in calculating Modulation frequency
byte sigLen = 0; //used when calculating the modulation period. Only a byte is required.
volatile boolean intDirection = true; //only used for Fubarino/ChipKit, can be removed for other platforms if neccessary

//Serial Tx buffer - uses Serial.write for faster execution
byte txBuffer[5]; //Key(+-)/1,count/1,offset/4,CR/1   <= format of packet sent to AnalysIR over serial


void setup() {
   
   Spark.connect();             // non-blocking attempt to connect to Wifi
    for (i=0;i<20;i++){ //wait for new flash instructions from cloud
        Spark.process();
         delay(1000);   // wait 20 seconds at startup, in case we need to reflash
    }
  
  if (Spark.connected) Spark.disconnect();             // no more cloud, as not needed

  Serial.begin(BAUDRATE);//fixed at 115200 bps for AnalysIR
  delay(500);//to avoid potential conflict with boot-loader on some systems
  txBuffer[4] = 13; //init ascii decimal value for CR in tx buffer (small bufferes used historically in Arduino with less SRAM)

  pinMode(IR_Rx_PIN, INPUT);
  pinMode(IR_Mod_PIN, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(D4, OUTPUT);
 

  //digitalWrite(D4, HIGH); //***************** temp to power receiver
  
  digitalWrite(ledPin, HIGH); //if LED stays on after reset, then serial not recognised PC
  delay(500);//time to see blink
  digitalWrite(ledPin, LOW);

  Serial.println(F("!AnalysIR!")); // HELLO STRING - ALL COMMENTS SENT IN !....! FORMAT & ignored by AnalysIR

  //Initialise State
  oldState = digitalRead(IR_Rx_PIN); //should be HIGH normally
  state = oldState;

  //Initialise Times
  oldTime = 0; //init
  newMicros = micros(); //init
  oldMicros = newMicros;


  //turn on interrupts and GO!

  attachInterrupt(IR_Rx_PIN, rxIR_Interrupt_Handler, CHANGE); //set up interrupt handler for IR rx on pin D2 - demodulated signal
  attachInterrupt(IR_Mod_PIN, rxIR3_Interrupt_Handler, FALLING); //set up interrupt handler for modulated IR rx on pin D3 - full signal with carrier

}

void loop() {
  while (true) { //avoid any extra stuff inserted by Arduino IDE at end of each normal Loop

    usLoop = micros(); //used once every loop rather than multiple calls
    
    if (oldState != state && countD < maxPULSES) {
      oldState = state;
      if (oldState) { //if the duration is longer than 0xFFFF(65535 uSecs) then multiple repeat pulses are stored, whith LSB used to signal mark or space
        sum = (newMicros - oldMicros); //re-use sum var here, to save RAM (normally used in reportperiod)
        while (sum > 0xFFFF && countD < (maxPULSES - 1) && countD) { //this allows for a mark/space of greater than 65535 uSecs (0xFFFF), ignore first signal
          sum -= 65535;//this assumes the length is not longer than 131070
          pulseIR[countD++] = 65535 | 0x0001; //store for later & include state
        }
        pulseIR[countD++] = sum | 0x0001; //store for later & include state
      }
      else {
        sum = (newMicros - oldMicros); //re-use sum var here, to save RAM (normally used in reportperiod)
        while (sum > 0xFFFF && countD < (maxPULSES - 1) && countD) { //this allows for a mark/space of greater than 65535 uSecs (0xFFFF), ignore first signal
          sum -= 65535;//this assumes the length is not longer than 131070
          pulseIR[countD++] = 65535 & 0xFFFE; //store for later & include state
        }
        pulseIR[countD++] = sum & 0xFFFE; //store for later & include state
      }
      oldMicros = newMicros; //remember for next time
      oldTime = usLoop; //last time IR was received
    }

    if (state && countD > 0 && (countD == maxPULSES ||  (usLoop - oldTime) > 100000)) { //if we have received maximum pulses or its 100ms since last one
      reportPulses();
      reportPeriod();//reports modulation frequency to host over serial
      countD = 0; //reset value for next time
    }

  }
}

void  reportPulses() {
  for (i = 0; i < countD; i++) {
    //the following logic takes care of the inverted signal in the IR receiver
    if (pulseIR[i] & 0x01) txBuffer[0] = '+'; //Mark is sent as +...LSB bit of pulseIR is State(Mark or Space)
    else txBuffer[0] = '-';           //Space is sent as -
    txBuffer[1] = (byte) (i & 0xFF); //count
    txBuffer[3] = pulseIR[i] >> 8; //byte 1
    txBuffer[2] = pulseIR[i] & 0xFE; //LSB 0 ..remove lat bit as it was State
    Serial.write(txBuffer, 5);
  }
}

void  reportPeriod() { //report period of modulation frequency in nano seconds for more accuracy
  sum = 0; // UL
  sigLen = 0; //byte
  countM2 = 0; //byte

  for (j = 1; j < (modPULSES - 1); j++) { //i is byte
    sigLen = (modIR[j] - modIR[j - 1]); //siglen is byte
    if (sigLen > 50 || sigLen < 10) continue; //this is the period range length exclude extraneous ones
    sum += sigLen; // sum is UL
    countM2++; //countM2 is byte
    modIR[j - 1] = 0; //finished with it so clear for next time
  }
  modIR[j - 1] = 0; //now clear last one, which was missed in loop

  if (countM2 == 0) return; //avoid div by zero = nothing to report
  sum =  sum * 1000 / countM2; //get it in nano secs
  // now send over serial using buffer
  txBuffer[0] = 'M'; //Modulation report is sent as 'M'
  txBuffer[1] = countM2; //number of samples used
  txBuffer[3] = sum >> 8 & 0xFF; //byte Period MSB
  txBuffer[2] = sum & 0xFF; //byte Period LSB
  Serial.write(txBuffer, 5);
  return;
}

void rxIR_Interrupt_Handler() { //important to use few instruction cycles here
  //digital pin D2 on Photon
  newMicros = micros(); //record time stamp for main loop
  state = pin2HIGH; //read changed state of interrupt pin 2 

}


void rxIR3_Interrupt_Handler() { //important to use few instruction cycles here
  //digital pin D3 on Photon - FALLING edge only
  modIR[countM++] = micros(); //just continually record the time-stamp, will be mostly modulations
  //just save LSB as we are measuring values of 20-50 uSecs only - so only need a byte (LSB)
}



