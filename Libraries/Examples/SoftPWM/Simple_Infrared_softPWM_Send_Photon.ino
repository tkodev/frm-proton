#define txPinIR A5   //IR carrier output
#define LED D7

unsigned char carrierFreq = 0; //default
unsigned char period = 0; //calculated once for each signal sent in initSoftPWM
unsigned char periodHigh = 0; //calculated once for each signal sent in initSoftPWM
unsigned char periodLow = 0; //calculated once for each signal sent in initSoftPWM

unsigned long sigTime = 0; //used in mark & space functions to keep track of time
unsigned long sigStart = 0; //used to calculate correct length of existing signal, to handle some repeats

#define NEC_HEX_VALUE 0x20DF22DDL
#define NEC_BIT_COUNT 32

//RAW NEC signal -32 bit with 1 repeat - make sure buffer starts with a Mark
unsigned int NEC_RAW[] = {9000, 4500, 560, 560, 560, 560, 560, 1690, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 1690, 560, 1690, 560, 560, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 560, 560, 560, 560, 1690, 560, 560, 560, 560, 560, 560, 560, 1690, 560, 560, 560, 1690, 560, 1690, 560, 560, 560, 1690, 560, 1690, 560, 1690, 560, 560, 560, 1690, 560, 39980, 9000, 2232, 560}; //AnalysIR Batch Export (IRremote) - RAW
unsigned int NEC_RAW2[] = {9000, 4500, 560, 560, 560, 560, 560, 1690, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 1690, 560, 1690, 560, 560, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 560, 560, 560, 560, 1690, 560, 560, 560, 560, 560, 560, 560, 1690, 560, 560, 560, 1690, 560, 1690, 560, 560, 560, 1690, 560, 1690, 560, 1690, 560, 560, 560, 1690, 560, 39980, 9000, 2232, 560}; //AnalysIR Batch Export (IRremote) - RAW
//RAW MITSU 88 bit signal  - make sure buffer starts with a Mark
unsigned int MITSU88AC_RAW[] = {3172, 1586, 394, 394, 394, 1182, 394, 394, 394, 394, 394, 1182, 394, 394, 394, 1182, 394, 394, 394, 394, 394, 1182, 394, 1182, 394, 1182, 394, 394, 394, 1182, 394, 394, 394, 1182, 394, 1182, 394, 1182, 394, 394, 394, 394, 394, 394, 394, 394, 394, 1182, 394, 1182, 394, 394, 394, 1182, 394, 1182, 394, 394, 394, 394, 394, 1182, 394, 394, 394, 394, 394, 1182, 394, 394, 394, 394, 394, 1182, 394, 1182, 394, 394, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 394, 394, 1182, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 1182, 394, 394, 394, 394, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 394, 394, 1182, 394, 1182, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 1182, 394, 394, 394}; //AnalysIR Batch Export (IRremote) - RAW
unsigned char MITSU88AC_Hex[] = {0x4A, 0x75, 0xC3, 0x64, 0x9B, 0xFF, 0x00, 0xFD, 0x02, 0x7D, 0x82};
unsigned int philips_RAW[] = {884,884,1768,884,884,884,884,884,884,884,884,884,884,1768,884,884,884,884,884,884,884,884,884,884,884,884};

void setup() {
  Serial.begin(15200);
  pinMode(txPinIR, OUTPUT);
  pinMode(LED, OUTPUT);
  delay(8000);
}

void loop() {
  // Philips
    sendRawBuf(philips_RAW, sizeof(philips_RAW) / sizeof(philips_RAW[0]), 38);
    delay(1000); //wait 5 seconds between each signal (change to suit)
  // Philips 2
    sendRawBuf(philips_RAW, sizeof(philips_RAW) / sizeof(philips_RAW[0]), 36);
    delay(1000); //wait 5 seconds between each signal (change to suit)
  // Sending NEC_RAW @ 56kHz
    sendRawBuf(NEC_RAW, sizeof(NEC_RAW) / sizeof(NEC_RAW[0]), 30);
    delay(1000); //wait 5 seconds between each signal (change to suit)
  // Sending NEC_RAW @ 56kHz
    sendRawBuf(NEC_RAW2, sizeof(NEC_RAW2) / sizeof(NEC_RAW2[0]), 36);
    delay(1000); //wait 5 seconds between each signal (change to suit)
  // Sending MITSU88AC_RAW @ 40kHz
    sendRawBuf(MITSU88AC_RAW, sizeof(MITSU88AC_RAW) / sizeof(MITSU88AC_RAW[0]), 33);
    delay(1000); //wait 5 seconds between each signal (change to suit)
  // Sending NEC_HEX_VALUE @ 38kHz
    sendHexNEC(NEC_HEX_VALUE, NEC_BIT_COUNT, 1, 36);
    delay(1000); //wait 5 seconds between each signal (change to suit)
  // Sending MITSU88AC_Hex @ 36kHz
    sendHexMITSU88AC(MITSU88AC_Hex, sizeof(MITSU88AC_Hex) / sizeof(MITSU88AC_Hex[0]), 38);
    delay(1000); //wait 5 seconds between each signal (change to suit)
  // Sending NEC_HEX_VALUE @ 33kHz
    sendHexNEC(NEC_HEX_VALUE, NEC_BIT_COUNT, 1, 40);
    delay(1000); //wait 5 seconds between each signal (change to suit)
  // Sending MITSU88AC_Hex @ 30kHz
    sendHexMITSU88AC(MITSU88AC_Hex, sizeof(MITSU88AC_Hex) / sizeof(MITSU88AC_Hex[0]), 56);
    delay(1000); //wait 5 seconds between each signal (change to suit)
}

void sendRawBuf(unsigned int *sigArray, unsigned int sizeArray, unsigned char kHz) { // Raw
  digitalWrite(LED,HIGH);
  noInterrupts();
  if (carrierFreq != kHz)  initSoftPWM(kHz); //we only need to re-initialise if it has changed from last signal sent
  sigTime = micros(); //keeps rolling track of signal time to avoid impact of loop & code execution delays
  for (int i = 0; i < sizeArray; i++) {
    mark(sigArray[i++]); //also move pointer to next position
    if (i < sizeArray) { //check we have a space remaining before sending it
      space(sigArray[i]); //pointer will be moved by for loop
    }
  }
  interrupts();
  digitalWrite(LED,LOW);
}

void sendHexNEC(unsigned long sigCode, byte numBits, unsigned char repeats, unsigned char kHz) {
  /*  A basic 32 bit NEC signal is made up of:
   *  1 x 9000 uSec Header Mark, followed by
   *  1 x 4500 uSec Header Space, followed by
   *  32 x bits uSec ( 1- bit 560 uSec Mark followed by 1690 uSec space; 0 - bit 560 uSec Mark follwed by 560 uSec Space)
   *  1 x 560 uSec Trailer Mark
   *  There can also be a generic repeat signal, which is usually not neccessary & can be replaced by sending multiple signals
   */
  #define NEC_HEADER_MARK 9000
  #define NEC_HEADER_SPACE 4500
  #define NEC_ONE_MARK 560
  #define NEC_ZERO_MARK 560
  #define NEC_ONE_SPACE 1690
  #define NEC_ZERO_SPACE 560
  #define NEC_TRAILER_MARK 560
  digitalWrite(LED,HIGH);
  noInterrupts();
  unsigned long bitMask = (unsigned long) 1 << (numBits - 1); //allows for signal from 1 bit up to 32 bits
  //
  if (carrierFreq != kHz)  initSoftPWM(kHz); //we only need to re-initialise if it has changed from last signal sent
  sigTime = micros(); //keeps rolling track of signal time to avoid impact of loop & code execution delays
  sigStart = sigTime; //remember for calculating first repeat gap (space), must end 108ms after signal starts
  // First send header Mark & Space
  mark(NEC_HEADER_MARK);
  space(NEC_HEADER_SPACE);
  while (bitMask) {
    if (bitMask & sigCode) { //its a One bit
      mark(NEC_ONE_MARK);
      space(NEC_ONE_SPACE);
    }
    else { // its a Zero bit
      mark(NEC_ZERO_MARK);
      space(NEC_ZERO_SPACE);
    }
    bitMask = (unsigned long) bitMask >> 1; // shift the mask bit along until it reaches zero & we exit the while loop
  }
  // Last send NEC Trailer MArk
  mark(NEC_TRAILER_MARK);
  //now send the requested number of NEC repeat signals. Repeats can be useful for certain functions like Vol+, Vol- etc
  /*  A repeat signal consists of
   *   A space which ends 108ms after the start of the last signal in this sequence
  *  1 x 9000 uSec Repeat Header Mark, followed by
  *  1 x 2250 uSec Repeat Header Space, followed by
  *  32 x bits uSec ( 1- bit 560 uSec Mark followed by 1690 uSec space; 0 - bit 560 uSec Mark follwed by 560 uSec Space)
  *  1 x 560 uSec repeat Trailer Mark
  */
  //First calcualte length of space for first repeat
  //by getting length of signal to date and subtracting from 108ms
  if (repeats == 0) return; //finished - no repeats
  else if (repeats > 0) { //first repeat must start 108ms after first signal
    space(108000 - (sigTime - sigStart)); //first repeat Header should start 108ms after first signal
    mark(NEC_HEADER_MARK);
    space(NEC_HEADER_SPACE / 2); //half the length for repeats
    mark(NEC_TRAILER_MARK);
  }
  while (--repeats > 0) { //now send any remaining repeats
    space(108000 - NEC_HEADER_MARK - NEC_HEADER_SPACE / 2 - NEC_TRAILER_MARK); //subsequent repeat Header must start 108ms after previous repeat signal
    mark(NEC_HEADER_MARK);
    space(NEC_HEADER_SPACE / 2); //half the length for repeats
    mark(NEC_TRAILER_MARK);
  }
  interrupts();
  digitalWrite(LED,LOW);
}

void sendHexMITSU88AC(unsigned char *sigArray, unsigned int sizeArray, unsigned char kHz) { //Mitsubish 88 bit Ir protocol format
  /*  A basic 88 bit NEC-'like' signal is made up of:
   *  1 x 3172 uSec Header Mark, followed by
   *  1 x 1586 uSec Header Space, followed by
   *  32 x bits uSec ( 1- bit 394 uSec Mark followed by 1182 uSec space; 0 - bit 394 uSec Mark follwed by 394 uSec Space)
   *  1 x 9000 uSec Trailer Mark
   *  There can also be a generic repeat signal, which is usually not neccessary & can be replaced by sending multiple signals
   */
  #define MITSU88AC_HEADER_MARK 3172
  #define MITSU88AC_HEADER_SPACE 1586
  #define MITSU88AC_ONE_MARK 394
  #define MITSU88AC_ZERO_MARK 394
  #define MITSU88AC_ONE_SPACE 1182
  #define MITSU88AC_ZERO_SPACE 394
  #define MITSU88AC_TRAILER_MARK 394
  digitalWrite(LED,HIGH);
  noInterrupts();
  //
  if (carrierFreq != kHz)  initSoftPWM(kHz); //we only need to re-initialise if it has changed from last signal sent
  sigTime = micros(); //keeps rolling track of signal time to avoid impact of loop & code execution delays
  // First send header Mark & Space
  mark(MITSU88AC_HEADER_MARK);
  space(MITSU88AC_HEADER_SPACE);
  for (unsigned int i = 0; i < sizeArray; i++) { //iterate thru each byte in sigArray
    register unsigned char bitMask =  0x80; //starting value of bitmask fo each Hex byte
    while (bitMask) { //do 8 times for each bit of the 8 bit byte
      if (bitMask & sigArray[i]) { //its a One bit
        mark(MITSU88AC_ONE_MARK);
        space(MITSU88AC_ONE_SPACE);
      }
      else { // its a Zero bit
        mark(MITSU88AC_ZERO_MARK);
        space(MITSU88AC_ZERO_SPACE);
      }
      bitMask = (unsigned char) bitMask >> 1; // shift the mask bit along until it reaches zero & we exit the while loop
    }
  }
  // Last send NEC Trailer Mark
  mark(MITSU88AC_TRAILER_MARK);
  interrupts();
  digitalWrite(LED,LOW);
}

void initSoftPWM(unsigned char carrierFreq) { // Assumes standard 8-bit Arduino, running at 16Mhz
  //supported values are 30, 33, 36, 38, 40, 56 kHz, any other value defaults to 38kHz
  //we will aim for a  duty cycle of circa 33%
  period =  (1000 + carrierFreq / 2) / carrierFreq;
  periodHigh = (period + 1) / 3;
  periodLow = period - periodHigh;
  switch (carrierFreq) {
    case 30  : //delivers a carrier frequency of 29.8kHz & duty cycle of 34.52%
      periodHigh -= 3; //Trim it based on measurement from Oscilloscope
      periodLow  -= 4; //Trim it based on measurement from Oscilloscope
      break;
    case 33  : //delivers a carrier frequency of 32.7kHz & duty cycle of 34.64%
      periodHigh -= 3; //Trim it based on measurement from Oscilloscope
      periodLow  -= 4; //Trim it based on measurement from Oscilloscope
      break;
    case 36  : //delivers a carrier frequency of 36.2kHz & duty cycle of 35.14%
      periodHigh -= 3; //Trim it based on measurement from Oscilloscope
      periodLow  -= 4; //Trim it based on measurement from Oscilloscope
      break;
    case 40  : //delivers a carrier frequency of 40.6kHz & duty cycle of 34.96%
      periodHigh -= 3; //Trim it based on measurement from Oscilloscope
      periodLow  -= 4; //Trim it based on measurement from Oscilloscope
      break;
    case 56  : //delivers a carrier frequency of 53.8kHz & duty cycle of 40.86%
      periodHigh -= 3; //Trim it based on measurement from Oscilloscope
      periodLow  -= 4; //Trim it based on measurement from Oscilloscope
      break;
    case 38  : //delivers a carrier frequency of 37.6kHz & duty cycle of 36.47%
    default :
      periodHigh -= 3; //Trim it based on measurement from Oscilloscope
      periodLow  -= 4; //Trim it based on measurement from Oscilloscope
      break;
  }
}

void mark(unsigned int mLen) { //uses sigTime as end parameter
  sigTime += mLen; //mark ends at new sigTime
  unsigned long now = micros();
  unsigned long dur = sigTime - now; //allows for rolling time adjustment due to code execution delays
  if (dur == 0) return;
  while ((micros() - now) < dur) { //just wait here until time is up
    digitalWrite(txPinIR, HIGH);
    if (periodHigh) delayMicroseconds(periodHigh);
    digitalWrite(txPinIR, LOW);
    if (periodLow)  delayMicroseconds(periodLow);
  }
}

void space(unsigned int sLen) { //uses sigTime as end parameter
  sigTime += sLen; //space ends at new sigTime
  unsigned long now = micros();
  unsigned long dur = sigTime - now; //allows for rolling time adjustment due to code execution delays
  if (dur == 0) return;
  while ((micros() - now) < dur) ; //just wait here until time is up
}
