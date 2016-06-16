// Initialization
  // HTKO
    #include "lib_htko.h"
    htko htko(1); // Lib instantiate
  // Device Constants
    const String dName = "Proton";
    const int ON = 1; const int OFF = 0;
  // Pin Constants  Match index for lights to button
    const int inputs[5] = {D6,A0,A1,D7,A4}; // 0 = DHT, 1&2 = Button, 3 = Door, 4  IR Capture
    const int outputs[5] = {0,A2,A3,D2,A5}; // 0 = RGB, 1&2 = Lights, 3 = Garage Door, 4  Null, IR LED (reverse with capture soon)
// Event System
  // System
    int firstRun = 1;
  // Lights & Buttons
    retained int outStates[4] = {OFF,OFF,ON}; // RGB, L1, L2. 0 to 2 (lights)
    int newStates[4]; // 1 to 3 (buttons)
    int oldStates[4]; // including zero index
  // DHT
    #include "lib_PietteTech_DHT.h"
    void dht_wrapper(); // must be declared before the lib initialization
    PietteTech_DHT DHT(inputs[0], 22, dht_wrapper); // Lib instantiate
    void dht_wrapper() { DHT.isrCallback(); } // This wrapper is in charge of calling  must be defined like this for the lib work
    const int maxTemp = 45;
  // IR
    #include "lib_IRremote.h"
    IRsend IRsend(outputs[4]);
// Timers and Handlers
  // Light
    const int shortTimer = 600000;
    const int longTimer = 14400000;
    Timer lightTimer(600000, timerLight, true);

// Setup
  // Main
    void setup(){
      // Set up
        setupPins();
        setupLast();
        setupCloud();
      // Publish
        delay(2500);
        Particle.publish(dName, "Status: Setup Complete. Firmware "+String(System.version()), 60, PRIVATE);
    }
    void setupPins(){
      // Setup PinModes
        pinMode(inputs[1], INPUT); // Btn 1
        pinMode(inputs[2], INPUT); // Btn 1
        pinMode(inputs[3], INPUT_PULLUP); // Door
        pinMode(inputs[4], INPUT); // IR Capture
        pinMode(outputs[1], OUTPUT); // Relay 1
        pinMode(outputs[2], OUTPUT); // Relay 2
        pinMode(outputs[3], OUTPUT); // Garage
        pinMode(outputs[4], OUTPUT); // IR LED
      // Debounce Relay
        digitalWrite(outputs[1], !OFF);
        digitalWrite(outputs[2], !OFF);
        digitalWrite(outputs[3], OFF);
    }
    void setupLast(){
      eventBtn(1, outStates[1], dName, 0);
      eventBtn(2, outStates[2], dName, 0);
      eventRGB(outStates[0], dName, 0);
    }
    void setupCloud(){
      Particle.function("cloudFunc", eventCloud);
    }

// Events
  // Assignment
    void loop(){
      if( firstRun ){
        newStates[3] = digitalRead(inputs[3]);
        firstRun = 0;
      }
      for (int i=1; i <= 3; i++){
        oldStates[i] = newStates[i];
        newStates[i] = digitalRead(inputs[i]);
      }
      if( newStates[1]>oldStates[1] ){
        eventBtn(1, 2, dName, 0);
      }
      if( newStates[2]>oldStates[2] ){
        eventBtn(2, 2, dName, 0);
      }
      if( newStates[3]!=oldStates[3] ){
        eventDoor(dName, 0);
      }
    }
    int eventCloud(String cloudData){ // must return Int and take a 63 character max String argument
      // Convert Data
        cloudData = cloudData.replace("%20", " ");
        int index1a = cloudData.indexOf(' ');
        String command = cloudData.substring(0, index1a);
        String user = cloudData.substring(index1a+1);
      // Parse Command
        int index1 = command.indexOf('.');
        int index2 = command.indexOf('.', index1+1);
        int cmd1 = htko.cmdStr( command.substring(0, index1) );
        int cmd2 = htko.cmdStr( command.substring(index1+1, index2) );
        int cmd3 = htko.cmdStr( command.substring(index2+1) );
      // Parse Command
        if( cmd1==1 ){ // Lights
          if( cmd2<=2 ){ // device
            if( (cmd3==0) ){ // Light Selection  RGB
              eventRGB(cmd2, user, 1);
            }else if( (cmd3==1)||(cmd3==2) ){ // Light Selection  1 or 2
              eventBtn(cmd3, cmd2, user, 1);
            }else if( (cmd3==3)&&(cmd2!=2) ){ // all & not toggle
              eventBtn(1, cmd2, user, 1);
              eventBtn(2, cmd2, user, 1);
            }
          }
        }else if( cmd1==2 ){ // Temp
          if( (cmd2==1) ){
            eventDHT(user, 1);
          }
        }else if( cmd1==4 ){ // IR
          if( (cmd2==1) ){
            eventIR(user, 1);
          }
        }else if( cmd1==5 ){ // Garage
          if( (cmd2==1) ){
            eventGarage(user, 1);
          }
        }else if( cmd1==3 ){ // Reset
          if( (cmd2==1) ){
            eventReset(user, 1);
          }
        }
      // Return
        return 1;
    }
  // Main
    void eventBtn(int light, int command, String user, int verbose){
      int result = actionLight(light,command);
      if( result ){
        actionTimerLight();
      }
      actionRGBFlash("white",25);
      if( verbose ){
        Particle.publish(dName, "Trigger: "+user+", Light: "+String(light)+", Command: "+String(command)+", State: "+String(result), 60, PRIVATE);
      }

    }
    void eventDoor(String user, int verbose){
      int timer;
      int result = newStates[3];
      if( result ){ // is door open?
        timer = shortTimer;
        actionRGBFlash("green",25);
      }else{
        timer = longTimer;
        actionRGBFlash("red",25);
      }
      lightTimer.changePeriod(timer);
      if( verbose ){
        Particle.publish(dName, "Trigger: "+user+", Door Open: "+String(result)+", Timer: "+String(timer), 60, PRIVATE);
      }
    }
    void eventRGB(int command, String user, int verbose){
      int result = actionRGB(command);
      actionRGBFlash("blue",25);
      if( verbose ){
        Particle.publish(dName, "Trigger: "+user+", RGB Command: "+String(command)+", State: "+String(result), 60, PRIVATE);
      }
    }
    void eventDHT(String user, int verbose){
      int result = actionDHT();
      actionRGBFlash("white",25);
      if( verbose ){
        if( result==999 ){
          Particle.publish(dName, "Trigger: "+user+", Temp State: Error Retrieving Temp", 60, PRIVATE);
        }else if( result>=maxTemp ){
          Particle.publish(dName, "Trigger: "+user+", Temp State: "+String(result)+"°C Overheating!", 60, PRIVATE);
        }else{
          Particle.publish(dName, "Trigger: "+user+", Temp State: "+String(result)+"°C", 60, PRIVATE);
        }
      }
    }
    void eventIR(String user, int verbose){
      int result = actionIR();
      actionRGBFlash("white",25);
      if( verbose ){
        Particle.publish(dName, "Trigger: "+user+", IR Flash, State: "+String(result), 60, PRIVATE);
      }
    }
    void eventGarage(String user, int verbose){
      int result = actionGarage();
      actionRGBFlash("white",25);
      if( verbose ){
        Particle.publish(dName, "Trigger: "+user+", Garage Button, State: "+String(result), 60, PRIVATE);
      }
    }
    void eventReset(String user, int verbose){ // Toggle pins 1
      uint32_t result = System.freeMemory();
      actionRGBFlash("white",25);
      if( verbose ){
        Particle.publish(dName, "Trigger: "+user+", System Reset, Memory: "+String(result)+"Bytes", 60, PRIVATE);
      }
      Particle.process();
      System.reset();
    }

// Actions
  // Lighting
    int actionLight(int light, int command){
      if( command<=1 ){
        outStates[light] = command;
      }else if( command==2 ){
        outStates[light] = !outStates[light];
      }
      digitalWrite(outputs[light], !outStates[light]);
      return outStates[light];
    }
    int actionRGB(int cmd){
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
      return outStates[0];
    }
    void actionRGBFlash(String color, int duration){
      // Colors
        if( color=="white"){
          RGB.color(255, 255, 255);
        }else if( color=="red"){
          RGB.color(255, 0, 0);
        }else if( color=="green"){
          RGB.color(0, 255, 0);
        }else if( color=="blue"){
          RGB.color(2, 157, 215);
        }
      // Initial
        if( outStates[0]==ON ){
          RGB.control(true);
        }
        RGB.brightness(0);
      // Main
        RGB.brightness(0);
        delay(duration/2);
        RGB.brightness(255);
        delay(duration);
        RGB.brightness(0);
      // End
        if( outStates[0]==ON ){
          RGB.brightness(150);
          RGB.color(2, 157, 215);
          RGB.control(false);
        }
    }
  // Devices
    int actionDHT(){
      int result = DHT.acquireAndWait(0);
      String status = DHT.resultStr(result);
      if( status=="OK" ){
        int temp = DHT.getCelsius();
        int humid = DHT.getHumidity();
        return temp;
      }else{
        return 999;
      }
    }
    int actionIR(){
      unsigned int data[67] = {4500, 4550, 500, 1750, 500, 1750, 500, 1700, 550, 600, 500, 600, 550, 600, 500, 600, 500, 650,
                                500, 1750, 500, 1700, 550, 1700, 550, 600, 500, 600, 500, 650, 500, 600, 500, 650, 500, 600,
                                500, 1750, 500, 600, 550, 600, 500, 600, 500, 650, 500, 600, 500, 650, 500, 1750, 500, 600,
                                500, 1750, 500, 1750, 500, 1750, 500, 1750, 500, 1700, 500, 1750, 500};
      IRsend.sendRaw(data, sizeof(data)/sizeof(data[0]), 36);
      return 1;
    }
    int actionGarage(){
      digitalWrite(outputs[3], OFF);
      digitalWrite(outputs[3], ON);
      delay(500);
      digitalWrite(outputs[3], OFF);
      return 1;
    }

// Timers
  // Light
    void actionTimerLight(){
      lightTimer.start();
    }
    void timerLight(){
      eventBtn(1, 0, "Light Timer", 1);
      eventBtn(2, 0, "Light Timer", 1);
    }
