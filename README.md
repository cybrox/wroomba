# WROOMBA!
iRobot Roombas are great for cleaning. - But pretty damn expensive. Especially the higher-tier models that actually support scheduling or remote control are not exactly cheap. The main goal of this project is to modify my Roomba 560 and add scheduled cleaning myself.    
Maybe this will go further...

### Hardware
The project hardware is dead simple. Roomba exposes an open interface at the top (Roomba Open Interface) which is a 7-pin Mini-DIN pug. (If you're like me and don't have those lying around, you can also use a 6-PS2 plug, since the roomba interface has 2 Vpp and 2 GND lines one of which is conveniently placed where the PS2 plug has a missing pin).

Other than that, you will need:
- An Espressif ESP32 development board
- A little circuit to get 3.3V from roomba's ~20V battery

### Software
The software is a standard ESP32 project. Check out [their docs](http://esp-idf.readthedocs.io/en/v1.0/) for more information.
