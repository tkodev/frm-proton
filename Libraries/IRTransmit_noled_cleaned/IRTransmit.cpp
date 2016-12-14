#include "application.h"
#include "IRTransmit.h"

const int IRTransmit::duty_cycle = 50;
const int IRTransmit::carrier_frequency = 38000;
const int IRTransmit::period = (1000000 + carrier_frequency / 2) / carrier_frequency;
const int IRTransmit::high_time = period * duty_cycle / 100;
const int IRTransmit::low_time = period - high_time;

IRTransmit::IRTransmit(unsigned int ir_output_pin) {
    ir_output_pin_ = ir_output_pin;
    pinMode(ir_output_pin_, OUTPUT);
    sig_time_ = 0;
}

void IRTransmit::Mark(unsigned int mLen) {
    sig_time_ += mLen; // mark ends at new sigTime
    unsigned long now = micros();
    unsigned long dur = sig_time_ - now; // allows for rolling time adjustment due to code execution delays
    if (dur == 0) return;
    while ((micros() - now) < dur) { // just wait here until time is up
        digitalWrite(ir_output_pin_, HIGH);
        delayMicroseconds(high_time - 3);
        digitalWrite(ir_output_pin_, LOW);
        delayMicroseconds(low_time - 4);
    }
}

void IRTransmit::Space(unsigned int sLen) {
    sig_time_ += sLen; //space ends at new sigTime
    unsigned long now = micros();
    unsigned long dur = sig_time_ - now; // allows for rolling time adjustment due to code execution delays
    if (dur == 0) return;
    while ( (micros() - now) < dur ); // just wait here until time is up
}

void IRTransmit::Transmit(unsigned int *data, size_t length) {
    // First send the NEC RAW signal
    noInterrupts();
    sig_time_ = micros(); // keeps rolling track of signal time to avoid impact of loop & code execution delays
    for (int i = 0; i < length; i++) {
        Mark(data[i++]); // also move pointer to next position
        if (i < length) Space(data[i]); // pointer will be moved by for loop
    }
    interrupts();
}
