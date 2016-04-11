// Initialization
  // System & Constants
    // HTKO
      #include "htko.h"
      htko htko(1); // Lib instantiate
    // Device Constants
      const String dName = "Proton";
      const String dNamel = htko.lowcap(dName);
      const int ON = 1; const int OFF = 0;
      boolean debug = false;
    // Pin Constants - Match index for lights to button
      const int inputs[5] = {D5,A0,A1,D7,A4}; // 0 = DHT, 1&2 = Button, 3 = Door, 4 - IR Capture
      const int outputs[5] = {0,A2,A3,D2,A5}; // 0 = RGB, 1&2 = Lights, 3 = Garage Door, 4 - Null, IR LED (reverse with capture soon)
  // Controllers
    // Lights & Buttons
      retained int outStates[3] = {OFF,OFF,ON}; // RGB, L1, L2
      int newStates[3]; // including zero index
      int oldStates[3]; // including zero index
    // DHT
      #include "PietteTech_DHT.h"
      void dht_wrapper(); // must be declared before the lib initialization
      PietteTech_DHT DHT(inputs[0], 22, dht_wrapper); // Lib instantiate
      void dht_wrapper() { DHT.isrCallback(); } // This wrapper is in charge of calling - must be defined like this for the lib work
      const int maxTemp = 45;
    // IR
      #include "IRremote.h"
      IRsend IRsend(outputs[4]);
  // Timers and Handlers
    // DHT
      Timer dhtUseTimer(2100, dhtUseTimerHandler, true);
      Timer dhtTimer(60000, dhtTimerHandler, false);
      volatile boolean inUseDht = false;
    // Light
      Timer lightTimer(1800000, lightTimerHandler, true);
      volatile boolean lightsExpire = true;
  // Notes
    // Drill Hole for IR
    // See resistor for IR

// Main
  // Setup
    void setup(){
      // Initialization
        // System
          System.on(reset, resetHandler);
        // Initial Event
          Particle.publish(dName, "Status: Setup. Firmware "+String(System.version()), 60, PRIVATE);
        // Pin Modes
          pinMode(inputs[1], INPUT); // Btn 1
          pinMode(inputs[2], INPUT); // Btn 1
          pinMode(inputs[3], INPUT_PULLUP); // Door
          pinMode(inputs[4], INPUT); // IR Capture
          pinMode(outputs[1], OUTPUT); // Relay 1
          pinMode(outputs[2], OUTPUT); // Relay 2
          pinMode(outputs[3], OUTPUT); // Garage
          pinMode(outputs[4], OUTPUT); // IR LED
      // Handlers
          dhtTimer.start();
        // Cloud
          boolean regCloudFunc = Particle.function("cloudFunc", cloudFuncHandler);
      // Initial Commands
        // Lights
          cmdParse("light."+htko.strDigit(outStates[0])+".0","setup",dNamel,false);
          cmdParse("light."+htko.strDigit(outStates[1])+".1","setup",dNamel,false);
          cmdParse("light."+htko.strDigit(outStates[2])+".2","setup",dNamel,false);
        // Delay
          delay(1000);
      // Final Initialization
        // Event
          Particle.publish(dName, "Status: Loop. Cloud Func: "+htko.strBool2(regCloudFunc)+", Light Timer: "+htko.cap(htko.strBool(lightsExpire))+", Memory: "+String(System.freeMemory())+" Bytes", 60, PRIVATE);
    }
  // Loop
    void loop(){
      btnHandler();
    }

// Output / Action Control
  // Lighting
    void lightCtrl(int cmd, int i){
      if( (i==1)||(i==2) ){
        if( cmd==2 ){
          outStates[i] = !outStates[i];
        }else if( (cmd==0)||(cmd==1) ){
          outStates[i] = cmd;
        }
        digitalWrite(outputs[i], !outStates[i]);
        if( outStates[i]==ON ){
          lightTimerCtrl();
        }
      }
    }
    void rgbCtrl(int cmd){
      if( cmd==2 ){
        outStates[0] = !outStates[0];
      }else if( (cmd==0)||(cmd==1) ){
        outStates[0] = cmd;
      }
      if( outStates[0]==ON ){
        RGB.brightness(150);
        RGB.color(2, 157, 215);
        RGB.control(false);
      }else{
        RGB.control(true);
        RGB.brightness(0);
      }
    }
    void flashCtrl(String color, int duration){
      // Initial
        if( outStates[0]==ON ){
          RGB.control(true);
        }
        if( color=="white"){
          RGB.color(255, 255, 255);
        }else if( color=="red"){
          RGB.color(255, 0, 0);
        }else if( color=="green"){
          RGB.color(0, 255, 0);
        }else if( color=="blue"){
          RGB.color(2, 157, 215);
        }
        RGB.brightness(0);
      // Main
        RGB.brightness(0);
        delay(duration/2);
        RGB.brightness(255);
        delay(duration);
        RGB.brightness(0);
      // End
        if( outStates[3]==ON ){
          RGB.brightness(150);
          RGB.color(2, 157, 215);
          RGB.control(false);
        }
    }
  // DHT
    void dhtReport(){
      if( !inUseDht ){
        inUseDht = true;
        int result = DHT.acquireAndWait(0);
        String status = DHT.resultStr(result);
        if( status=="OK" ){
          int temp = DHT.getCelsius();
          int humid = DHT.getHumidity();
          Particle.publish(dName, "Status: "+String(temp)+"°C, "+String(humid)+"%", 60, PRIVATE);
        }else{
          Particle.publish(dName, status, 60, PRIVATE);
        }
      }
      dhtUseTimer.start();
    }
    void dhtReportOverheat(){
      if( !inUseDht ){
        inUseDht = true;
        int result = DHT.acquireAndWait(0);
        String status = DHT.resultStr(result);
        if( status=="OK" ){
          int temp = DHT.getCelsius();
          int humid = DHT.getHumidity();
          if( temp>=maxTemp ){
            Particle.publish(dName, "Overheating Alert! Status: "+String(temp)+"°C, "+String(humid)+"%", 60, PRIVATE);
          }
        }else{
          Particle.publish(dName, status, 60, PRIVATE);
        }
      }
      dhtUseTimer.start();
    }
  // IR
    void irflashCtrl(){
      unsigned int data[67] = {4500, 4550, 500, 1750, 500, 1750, 500, 1700, 550, 600, 500, 600, 550, 600, 500, 600, 500, 650,
                                500, 1750, 500, 1700, 550, 1700, 550, 600, 500, 600, 500, 650, 500, 600, 500, 650, 500, 600,
                                500, 1750, 500, 600, 550, 600, 500, 600, 500, 650, 500, 600, 500, 650, 500, 1750, 500, 600,
                                500, 1750, 500, 1750, 500, 1750, 500, 1750, 500, 1700, 500, 1750, 500};
      IRsend.sendRaw(data, sizeof(data)/sizeof(data[0]), 36);
    }
    void garageCtrl(){
      digitalWrite(outputs[3], OFF);
      digitalWrite(outputs[3], ON);
      delay(500);
      digitalWrite(outputs[3], OFF);
    }

// Command Parser
  // Main
    boolean cmdParse(String command, String user, String device, boolean feedback){
      // Convert Data
        int index1 = command.indexOf('.');
        int index2 = command.indexOf('.', index1+1);
        int cmd1 = htko.cmdStr( command.substring(0, index1) );
        int cmd2 = htko.cmdStr( command.substring(index1+1, index2) );
        int cmd3 = htko.cmdStr( command.substring(index2+1) );
      // Initialize Results
        boolean result = false;
        String status = "None";
        String trigger;
      // Reception Feedback
        if( feedback ){
          flashCtrl("white",25);
        }
      // Parse Command
        if( cmd1==1 ){ // Lights
          if( (cmd2==0)||(cmd2==1)||(cmd2==2) ){ // Command Selection
            if( (cmd3==0) ){ // Light Selection - RGB
              rgbCtrl(cmd2);
              result = true; status = htko.strDigit(cmd2);
            }else if( (cmd3==1)||(cmd3==2) ){ // Light Selection - 1 or 2
              lightCtrl(cmd2,cmd3);
              result = true; status = htko.strDigit(cmd2);
            }else if( (cmd3==3)&&(cmd2!=2) ){ // all & not toggle
              lightCtrl(cmd2,1);
              lightCtrl(cmd2,2);
              result = true; status = htko.strDigit(cmd2);
            }
          }
        }else if( cmd1==2 ){ // Temp
          if( (cmd2==1) ){
            dhtReport();
            result = true;
          }
        }else if( cmd1==3 ){ // Reset
          if( (cmd2==1) ){
            System.reset();
            result = true;
          }
        }else if( cmd1==4 ){ // IR
          if( (cmd2==1) ){
            irflashCtrl();
            result = true;
          }
        }else if( cmd1==5 ){ // Garage
          if( (cmd2==1) ){
            garageCtrl();
            result = true;
          }
        }
      // Return Feedback
        if( feedback ){
          status = ", Status: "+htko.cap(status);
          trigger = " "+user+"."+device;
          Particle.publish(dName, "Command "+htko.strBool2(result)+": "+command+trigger+status+", Memory: "+String(System.freeMemory())+" Bytes", 60, PRIVATE);
          flashCtrl("white",25);
        }
      // Return
        return result;
    }

// Input / Sensor Handlers
  // System
    void resetHandler(){ // Toggle pins 1
      uint32_t freemem = System.freeMemory();
      Particle.publish(dName, "Status: Reset. Memory: "+String(freemem)+" Bytes", 60, PRIVATE);
    }
  // DHT Timers
    void dhtUseTimerHandler(){
      inUseDht = false;
    }
    void dhtTimerHandler(){
      dhtReportOverheat();
    }
  // Light Timer
    void lightTimerCtrl(){
      if( lightsExpire ){
        lightTimer.start();
      }
    }
    void lightTimerHandler(){
      cmdParse("light.off.all","timer",dNamel,true);
    }
  // Door
    void btnHandler(){
      for (int i=1; i <= 3; i++){
        oldStates[i] = newStates[i];
        newStates[i] = digitalRead(inputs[i]);
      }
      if( newStates[1]>=oldStates[1] ){
        cmdParse("light.toggle.1","button",dNamel,false);
      }
      if( newStates[2]>=oldStates[2] ){
        cmdParse("light.toggle.2","button",dNamel,false);
      }
      if( newStates[3]!=oldStates[3] ){
        cmdParse("light.toggle.1","door",dNamel,true);
      }
    }
  // Cloud Event / Function
    int cloudFuncHandler(String cloudData){ // must return Int and take a 63 character max String argument
      // Convert Data
        cloudData = cloudData.replace("%20", " ");
        int index1 = cloudData.indexOf(' ');
        String command = cloudData.substring(0, index1);
        String user = cloudData.substring(index1+1);
      // Send to Command
        boolean result = cmdParse(command,user,"cloud",true);
      // Return
        return result;
    }
