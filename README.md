# idc-yellow-squadron
Arduino bot performing "Yellow Squadron" tasks for Duke University's Integrated Design Challenge

This project uses an Arduino Mega2560 with a Parallax Board of Education Shield (BOE-Bot) and performs the following actions:

1. Motion control using two servomotors
2. Line following code using two QTI line following sensors
2. Light detection code using a photoresistor
3. binary to integer conversion based on light detection
4. Communication between bots using Xbee

### Background:

In this Star Wars-themed integrated design challenge, we created robots that must
work together to complete a complex, multi-stage problem. 

Challenge: Stop the evil Galactic Empire and destroy the Death Star

The team was divided into five squadrons:
1. YELLOW SQUADRON - Determine which defensive turrets are activated
2. BLUE SQUADRON - Determine the location of the Death Star
3. ORANGE SQUADRON - Obtain the shuttle access code
4. RED SQUADRON - Determine the enemy’s strength by counting TIE fighters and nonthreatening craft
5. GREEN SQUARDON - Determine the distance available for maneuvering

The goal of the "Yellow Sqaudron" was to follow a line and reconnoiter the light bulbs and detect the glowing lights. The overall pattern of activation can be converted from a binary number encoded in the on-off pattern of the three lights (the first light corresponds to the least significant bit, the final light corresponds to the most significant bit) to an integer (between 1 and 5). This integer code must be
clearly displayed at the end of the mission. The next and final task is to await the arrival of your fellow Rebel squadrons, communicate your presence together, and line up the bots in the correct order.

### Equipment:
* BOE-Bot plastic wheel x2
*	BOE - Bot rubber band tire x2
*	BOE - Bot 1” tail ball wheel
*	BOE - Bot Aluminum Chassis
*	BOE - Bot Li Ion Power Pack with cable and barrel plug
*	Li Ion cell x2
*	Continuous rotation servomotor x2
*	Arduino ATMEGA 2560
*	Board of Education Shield for Arduino
*	XBee Module
*	4-40 pan head screws x4
*	1” round stainless steel spacers x4
*	QTI Line Follower AppKit for the Boe-Bot x2
*	Photoresistor
*	LED Seven Segment Display
*	Jumper wires x23
*	IR LED Shield (Top + Bottom)
*	220 ohm Resistor
*	Arduino Software


