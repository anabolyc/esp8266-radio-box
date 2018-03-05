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


## Links
1. [Radio library by Matthias Hertel](https://github.com/mathertel/Radio)
1. [Sming framework](https://github.com/SmingHub/Sming)

## Look
![Home page](https://raw.githubusercontent.com/anabolyc/esp8266-radio-box/master/images/screen-01.png)
![Device look](https://raw.githubusercontent.com/anabolyc/esp8266-radio-box/master/images/screen-02.jpg)
![Schematics](https://raw.githubusercontent.com/anabolyc/esp8266-radio-box/master/images/screen-03.png)
![PCB](https://raw.githubusercontent.com/anabolyc/esp8266-radio-box/master/images/screen-04.png)
