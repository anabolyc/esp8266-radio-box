## Local build configuration
## Parameters configured here will override default and ENV values.
## Uncomment and change examples:

## MacOS / Linux:
 ESP_HOME = /media/storage-unprotected/opt/esp-open-sdk

## MacOS / Linux
 SMING_HOME = /media/storage-unprotected/opt/Sming/Sming

## MacOS / Linux:
 COM_PORT = /dev/ttyUSB0

## Com port speed
 COM_SPEED	= 115200

## Add your source directories here separated by space
# MODULES = app
#EXTRA_INCDIR = include $SMING_HOME/third-party/esp-open-lwip/

## Configure flash parameters (for ESP12-E and other new boards):
# SPI_MODE: qio, qout, dio, dout  
# SPI_MODE = dio

# SPI_SIZE: 512K, 256K, 1M, 2M, 4M
SPI_SIZE = 4M

## SPIFFS options
#DISABLE_SPIFFS = 1

SPIFF_FILES = files

