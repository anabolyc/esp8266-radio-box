# esp8266-radio-box

Use TEA5767 FM radio module in combination with ESP-8266 self hosted web interface

## Motivation

* Want to create web-interface for FM radio module which is
    * nice looking
    * mobile friendly
    * self-hosted

## General structure

Radio module code based on good work of [Matthias Hertel](http://www.mathertel.de). 
I completed implementation of TEA5767 module 
* it seems to have better perception than RDA5807
* it have no volume control, nor RDS, no bass boost, so those options are disabled in UI
* i didn't sync my implemetation changes in RDA5807 implementation, you might want to do it yourself

## How to use
1. Have [Sming Framwork](https://github.com/SmingHub/Sming) installed
2. Open project in [vscode](https://github.com/Microsoft/vscode) and use following build recepies
* make - build project
* attach serial - attach serial to configured port
* make flash - build, flash to device via Serial and attach terminal to it
* make flashinit - run once when using new esp8266 mcu
3. Adjust settings in Makefile-user.mk if necessary

To flash device using my pcb
1. Press FLASH and RESET
2. Release RESET, then release FLASH - device ready to flash

To debug web-interface directly in IDE (no need to upload each change to mcu)
1. run ./debug-httpd.sh (need to have [docker](https://www.docker.com/) installed)
2. Find this line 
```
var NODE_IP = "192.168.1.96";
```
    and change to your device IP
3. Go to http://localhost:8080,
 when started from localhost it will connect to ws://NODE_IP (device itself should be online providing backend)


## Links
1. [Radio library by Matthias Hertel](https://github.com/mathertel/Radio)
1. [Sming framework](https://github.com/SmingHub/Sming)
1. [EasyEDA - create and order PCB](https://easyeda.com)
1. [This project at EasyEDA](https://easyeda.com/andrey.mal/07_radiobox-dd061be66402444d946e622d28fd211e)

## Look
![Home page](https://raw.githubusercontent.com/anabolyc/esp8266-radio-box/master/images/screen-01.png)
![Device look](https://raw.githubusercontent.com/anabolyc/esp8266-radio-box/master/images/screen-02.jpg)
![Schematics](https://raw.githubusercontent.com/anabolyc/esp8266-radio-box/master/images/screen-03.png)
![PCB](https://raw.githubusercontent.com/anabolyc/esp8266-radio-box/master/images/screen-04.png)
