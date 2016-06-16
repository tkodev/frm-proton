/*
Misc HTKO functions
*/

#ifndef htko_h
#define htko_h

  // Include
    #include "application.h"
  // Class
    class htko{
      public:
        htko(int woo);
        String strDigit(int inputInt);
        String strBool(boolean inputBool);
        String strBool2(boolean inputBool);
        int digitStr(String inputStr);
        int cmdStr(String inputStr);
        String cap(String inputStr);
        String lowcap(String inputStr);
        int countIntArray(int data[]);
      private:
        //
    };

#endif
