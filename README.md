---
type: project
draft: false
date: '2016-06-01'
title: Proton Light Switch
topics:
  - code
tools:
  - C++
  - IoT
  - photon
  - arduino
  - infrared
link: 'https://www.youtube.com/watch?v=7FFnYyeRKBw'
code: 'https://github.com/htko89/proton'
version: 0.2.0
photos:
  - wallplate.jpg
  - fixture.jpg
  - fixture-back.jpg
path: code/personal/proton-light-switch
---
# Personal Project
This is a custom built wifi enabled smart switch I’ve built for personal use. It also is connected to my garage door opener, and has a built in IR blaster. DHT 22 temperature sensor is also present to check for overheating.

## Project Criteria:
* Microcontroller: Particle.io (Particle Photon)
* Garage Door Remote: Craftsman 315mhz Remote
* Light Switch: 2CH 3.3-5V Relay Kit
* Temp Sensor: DHT 22
* IR Emitter: TSAL6100
* IR Recorder: TSOP34438
* Pseudo MVC approach: Event Assignment > Events > Action
