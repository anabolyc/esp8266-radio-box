#include <user_config.h>
#include <SmingCore.h>
#include <Wire.h>

int PT2323_ADDRESS = 74;
int PT2258_ADDRESS = 68;
int DEVICE_REG_MODE1 = 0x00;
int DEVICE_REG_LEDOUT0 = 0x1d;

int MIN_ATTENUATION = 0;
int MAX_ATTENUATION = 79;

int CHAN_ALL = 0;
int CHAN_FL = 1;
int CHAN_FR = 2;
int CHAN_RL = 3;
int CHAN_RR = 4;
int CHAN_CEN = 5;
int CHAN_SW = 6;

int PT2258_FL_1DB = 0x90;
int PT2258_FL_10DB = 0x80;
int PT2258_FR_1DB = 0x50;
int PT2258_FR_10DB = 0x40;
int PT2258_RL_1DB = 0x70;
int PT2258_RL_10DB = 0x60;
int PT2258_RR_1DB = 0xB0;
int PT2258_RR_10DB = 0xA0;
int PT2258_CEN_1DB = 0x10;
int PT2258_CEN_10DB = 0x00;
int PT2258_SW_1DB = 0x30;
int PT2258_SW_10DB = 0x20;
int PT2258_ALLCH_1DB = 0xE0;
int PT2258_ALLCH_10DB = 0xD0;

int volumeFR = 74;
int volumeFL = 74;
int volumeRR = 74;
int volumeRL = 74;
int volumeCEN = 74;
int volumeSW = 74;
int volumeALLCH = 74;
