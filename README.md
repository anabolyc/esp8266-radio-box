# ESP8266 Radio Box

![Open Source Hardware](/doc/images/open-source-hardware-logo.png)
![Open Source Software](/doc/images/open-source-software-logo.png)

Use TEA5767/RDA5807M FM radio module in combination with ESP8266 self hosted web interface. Project includes both hardware and software components and shared via GPLv3 permissive license.

![image](https://user-images.githubusercontent.com/5459747/176870489-4d6b2714-57a4-485f-a7dd-00002df2779f.png)

## Key features

- WifiMnager with built-in captive portal for the first time Wifi configuration
- Built-in web server for mobile app like site hosting
- Websockets server for client-server communication
- Onboard RGB LED for status indication
- Onmoard 240x240 px TFT screen to use offline
- Built-in CH340 Serial-bridge for one-click firmware upload

## Motivation

* Want to create web-interface for FM radio module which is
    * good looking
    * mobile friendly
    * self-hosted
* Later on planned also offline use, thus TFT screen

## General structure

Radio module code based on good work of [Matthias Hertel](https://github.com/mathertel/Radio). 
I added support for both RDA5807 and TEA5767 modules, however RDA5807 seems to be better choice
* it has higher output power, so you can connect headphones directly
* it has hardware volume control, RDS, bass boost, mute and few more, TEA5767 has none of those
* TEA5767 seems to has better perception than RDA5807, but with a good signal doesn't make a big difference

## How to build and flash

1. Install [Platformio[(https://platformio.org/) IDE (vscode with plugin at the moment of writing)
1. Open project in the [vscode](https://github.com/Microsoft/vscode) and build using `Build` command. Use `esp8266-tea5767` or `esp8266-rda5807` environment depending on the radio chip installed.
1. Use `Upload Filesystem Image` task to build and upload app files used by webserver
1. Use `Upload` or `Upload and Monitor` command to flash firmware onto device.
1. Firmware uses [WiFiManager](https://github.com/tzapu/WiFiManager) for wifi configuration. First time device will not find known WiFi nework and will start its own named ESP-XXXXXX, with ChipId suffix. Connect to that network, you will be redirected to captive portal and prompted for Wifi credentials. From this moment forward device will connect to known network automatically.
1. Now enter http://device-ip or http://ESP-XXXXXX.lan adress to control Radio via app.
![Home screen](doc/screen-01.png)

## How to debug web app

To debug web-interface directly in IDE (no need to upload each change to ESP)
1. run `start debug web-server` task in vscode. It assumes you have [docker](https://www.docker.com/) installed. It will start light http server on 8080 port.
2. Find this line 
```
var NODE_IP = "192.168.0.36";
```
and change to your device IP. So app is hosted on your PC now, but will talk to actual ESP via WebSockets as a backend.
3. Acces ap at http://localhost:8080,
 
## Hardware

Design files can be found in [hardware](/hardware) section.

## Look

![image](https://user-images.githubusercontent.com/5459747/176870306-7f4cc492-df85-44ec-9982-0e9591fa8682.png)
