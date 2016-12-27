/*
In actuality this is code from https://www.analysir.com/blog/2015/06/10/simple-infrared-pwm-on-arduino-part-2-raw-ir-signals/
*/

#ifndef IRTransmit_h
#define IRTransmit_h

  // Include
    #include <stddef.h>
  // Class
    class IRTransmit{
      public:
        IRTransmit(unsigned int ir_output_pin);
        void Transmit(unsigned int *data, size_t length);
      private:
        int ir_output_pin_;
        unsigned long sig_time_;
        const static int duty_cycle;
        const static int carrier_frequency;
        const static int period;
        const static int high_time;
        const static int low_time;
        void Mark(unsigned int mLen);
        void Space(unsigned int sLen);
    };

#endif
