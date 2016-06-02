# Proton Smart Light Switch
[![ScreenShot](http://www.htko.ca/wp-content/uploads/2016/04/Youtube-button.png)](https://youtu.be/7FFnYyeRKBw)
- "Proton" is a smart in wall light switch / temp sensor / door sensor / IR blaster / garage door opener. Based on the Particle Photon (cloud arduino device).
- The project is split into three sections:
- Event handlers, Command Parser, and Action functions.
- All pins are defined in inputs[5] and outputs[5].

# Cloud Handler
- "cloudFunc" is a "Particle Cloud" function handler that takes a string in the format of "command.action.device user". 
- The user designation is used to tell what the Proton should report (in event logs) as the trigger user of the command. 
- EX: using the particle cloud API: send a string "light.toggle.1 mobile-phone" to "cloudFunc" will toggle light on output[1].
- "cloudFunc" event handler passes the string to "cmdParse" which does the real work.
- see Particle Cloud documentation: https://docs.particle.io/reference/firmware/photon/#particle-publish-

# Event Handlers
- Events include timer events, door opened events, temp events, etc. all of which passes the string to "cmdParse", much like "cloudFunc" above.
- EX: "garage.door.1 garage-timer"

# Command Parser
- "cmdParse" is what parses the string commands from the handler functions and then assigns actions out into the action functions.
- It will publish an event stating what the command is, who used it, and whether it was successful.
- All updates are posted to the particle cloud event log (dashboard.particle.io). A service like "IFTTT" can use this event log to push command feedback to "PushBullet" or any notification service. It can also trigger "cloudFunc".

# Action functions
- DHT library is used to in a timer every 5 minutes to check for overheating, and as its own command "dht.report.1 any-user" to report current temp / humidity. 
- IR library is used for turning on the room TV when the door opens. The door sensor handler are currently incomplete.
- Last states are saved, so flashing a updates and power losses won't make your lights flash on or off randomly.

# To-do
- Move this information into the wiki.
- Provide fritzing / wiring diagrams.
- Provide pictures of finished product.

# Credit
- Website: http://www.htko.ca/portfolio_page/proton-smart-light-switch/
- Uses Spark-Core-IRremote: https://github.com/qwertzguy/Spark-Core-IRremote.git
- Which is modified from Arduino-IRremote: https://github.com/z3t0/Arduino-IRremote.git
- Uses PietteTech_DHT: https://github.com/piettetech/PietteTech_DHT.git
