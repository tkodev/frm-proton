# Proton
"Proton" is a smart in wall light switch / temp sensor / door sensor / IR blaster / garage door opener. Based on the Particle Photon (cloud arduino device).

The project is split into three sections:
Event handlers, Command Parser, and Action functions.
All pins are defined in inputs[5] and outputs[5]

cloudFunc is a registered cloud function handler that takes a string in the format of "command.action.device user". For example: send this string "light.toggle.1 mobile-phone" to cloudFunc using the particle cloud REST API to toggle light on output[1]. Last states are saved, so flashing a updates won't make your lights flash randomly. cloudFunc event handler passes the string to cmdParse which does the real work

cmdParse is what parses the string commands from the handler functions and then assigns actions out into the action functions. It will publish an event stating what the command is, who used it, and whether it was successful.

DHT library is used to in a timer every 5 minutes to check for overheating, and as its own command "dht.report.1 any-user" to report current temp / humidity. IR library is used for turning on the room TV when the door opens. The door sensor handler are currently incomplete.

Uses Spark-Core-IRremote: https://github.com/qwertzguy/Spark-Core-IRremote.git
which is modified from Arduino-IRremote: https://github.com/z3t0/Arduino-IRremote.git
Uses PietteTech_DHT: https://github.com/piettetech/PietteTech_DHT.git
