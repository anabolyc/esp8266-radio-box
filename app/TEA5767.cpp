/// \brief Implementation for the radio library to control the TEA5767 radio chip.
///
/// Based on work by
/// \author Matthias Hertel, http://www.mathertel.de

#include <Arduino.h>
#include <Wire.h> // The chip is controlled via the standard Arduiino Wire library and the IIC/I2C bus.

#include <radio.h> // Include the common radio library interface
#include <TEA5767.h>

// ----- Definitions for the Wire communication

#define TEA5767_ADR 0x60 // I2C address of TEA5767

// ----- Radio chip specific definitions including the registers

#define QUARTZ 32768
#define FILTER 225000

// Define the registers

#define REG_1 0x00
#define REG_1_MUTE 0x80
#define REG_1_SM 0x40
#define REG_1_PLL 0x3F

#define REG_2 0x01
#define REG_2_PLL 0xFF

#define REG_3 0x02
#define REG_3_MS 0x08
#define REG_3_SSL_HIGH 0x60
#define REG_3_SSL_MID 0x40
#define REG_3_SSL_LOW 0x20
#define REG_3_SUD 0x80
#define REG_3_HLSI 0x10

#define REG_4 0x03
#define REG_4_SMUTE 0x08
#define REG_4_XTAL 0x10
#define REG_4_BL 0x20
#define REG_4_STBY 0x40

#define REG_5 0x04
#define REG_5_PLLREF 0x80
#define REG_5_DTC 0x40

#define STAT_3 0x02
#define STAT_3_STEREO 0x80

#define STAT_4 0x03
#define STAT_4_ADC 0xF0

// // Use this define to setup European FM specific settings in the chip.
#define IN_EUROPE

// ----- implement

TEA5767::TEA5767(void) {}

// initialize all internals.
void TEA5767::init()
{
  DEBUG_FUNC0("init");

  registers[REG_1] = 0x00;
  registers[REG_2] = 0x00;
  registers[REG_3] = 0xB0;
  //registers[REG_3] = REG_3_HLSI;
  registers[REG_4] = REG_4_XTAL & ~REG_4_SMUTE;

#ifdef IN_EUROPE
  registers[REG_5] = 0; // 50 ms Europe setup
#else
  registers[REG_5] = REG_5_DTC; // 75 ms Europe setup
#endif
  Wire.begin();
}

void TEA5767::setPower(bool value)
{
  DEBUG_FUNC0("setPower");
  RADIO::setPower(value);

  if (value)
    registers[REG_4] &= ~REG_4_STBY;
  else
    registers[REG_4] |= REG_4_STBY;

  _saveRegisters();
}

// ----- Volume control -----
/// setVolume is a non-existing function in TEA5767. It will always me MAXVOLUME.
void TEA5767::setVolume(uint8_t newVolume)
{
  DEBUG_FUNC0("setVolume");
  RADIO::setVolume(MAXVOLUME);
}

/// setBassBoost is a non-existing function in TEA5767. It will never be acivated.
void TEA5767::setBassBoost(bool switchOn)
{
  DEBUG_FUNC0("setBassBoost");
  RADIO::setBassBoost(false);
}

/// force mono receiving mode.
void TEA5767::setMono(bool switchOn)
{
  DEBUG_FUNC0("setMono");
  RADIO::setMono(switchOn);

  if (switchOn)
    registers[REG_3] |= REG_3_MS;
  else
    registers[REG_3] &= ~REG_3_MS;
  _saveRegisters();
}

void TEA5767::setMute(bool switchOn)
{
  DEBUG_FUNC0("setMute");
  RADIO::setMute(switchOn);

  if (switchOn)
  {
    registers[REG_1] |= REG_1_MUTE;
  }
  else
  {
    registers[REG_1] &= ~REG_1_MUTE;
  }
  _saveRegisters();
}

// ----- Band and frequency control methods -----
/// Tune to new a band.
void TEA5767::setBand(RADIO_BAND newBand)
{
  if (newBand == RADIO_BAND_FM)
  {

    RADIO::setBand(newBand);

#ifdef IN_EUROPE
    //Freq(MHz) = 0.100(in Europe) * Channel + 87.5MHz
    registers[REG_4] &= ~REG_4_BL;

#else
    //Freq(MHz) = 0.200(in USA) * Channel + 87.5MHz
    registers[REG_4] |= REG_4_BL;
#endif
  }
  // FM mixer for conversion to IF of the US/Europe (87.5 MHz to 108 MHz) and Japanese
  // (76 MHz to 91 MHz) FM band
}

RADIO_CAP TEA5767::getCapabilities()
{
  return TEA5767_CAPS;
}

/**
* @brief Retrieve the real frequency from the chip after automatic tuning.
* @return RADIO_FREQ the current frequency.
*/
RADIO_FREQ TEA5767::getFrequency()
{
  _readRegisters();
  unsigned long frequencyW = ((status[REG_1] & REG_1_PLL) << 8) | status[REG_2];
  frequencyW = ((frequencyW * QUARTZ / 4) - FILTER) / 10000;
  return ((RADIO_FREQ)frequencyW);
}

/**
* @brief Change the frequency in the chip.
* @param newF
* @return void
*/
void TEA5767::setFrequency(RADIO_FREQ newF)
{
  DEBUG_FUNC1("setFrequency", newF);
  _freq = newF;

  unsigned int frequencyB = 4 * (newF * 10000L + FILTER) / QUARTZ;

  registers[0] = frequencyB >> 8;
  registers[1] = frequencyB & 0XFF;
  _saveRegisters();
  delay(100);
}

/// Start seek mode upwards.
void TEA5767::seekUp()
{
  DEBUG_FUNC0("seekUp");
  _seek(true);
}

/// Start seek mode downwards.
void TEA5767::seekDown()
{
  DEBUG_FUNC0("seekDown");
  _seek(false);
}

/// Load all status registers from to the chip
void TEA5767::_readRegisters()
{
  Wire.requestFrom(TEA5767_ADR, 5); // We want to read all the 5 registers.

  if (Wire.available())
  {
    for (uint8_t n = 0; n < 5; n++)
    {
      status[n] = Wire.read();
    } // for
  }   // if
}

// Save writable registers back to the chip
// using the sequential write access mode.
void TEA5767::_saveRegisters()
{
  Wire.beginTransmission(TEA5767_ADR);
  for (uint8_t n = 0; n < sizeof(registers); n++)
  {
    Wire.write(registers[n]);
  } // for

  byte ack = Wire.endTransmission();
  if (ack != 0)
  {                              //We have a problem!
    Serial.print("Write Fail:"); //No ACK!
    Serial.println(ack, DEC);    //I2C error: 0 = success, 1 = data too long, 2 = rx NACK on address, 3 = rx NACK on data, 4 = other error
  }                              // if
}

void TEA5767::getRadioInfo(RADIO_INFO *info)
{
  RADIO::getRadioInfo(info);

  _readRegisters();
  if (status[STAT_3] & STAT_3_STEREO)
    info->stereo = true;
  info->rssi = (status[STAT_4] & STAT_4_ADC) >> 4;
}

void TEA5767::getAudioInfo(AUDIO_INFO *info)
{
  RADIO::getAudioInfo(info);
}

void TEA5767::checkRDS()
{
  // DEBUG_FUNC0("checkRDS");
}

// ----- Debug functions -----

/// Send the current values of all registers to the Serial port.
void TEA5767::debugStatus()
{
  RADIO::debugStatus();
}

/// Seeks out the next available station
void TEA5767::_seek(bool seekUp)
{
  DEBUG_FUNC0("_seek");

  if (seekUp)
    registers[REG_3] |= REG_3_SUD;
  else
    registers[REG_3] &= ~REG_3_SUD;
  
  registers[REG_3] |= REG_3_SSL_MID;
  registers[REG_1] |= REG_1_SM;

  _saveRegisters();
}

/// wait until the current seek and tune operation is over.
void TEA5767::_waitEnd()
{
  DEBUG_FUNC0("_waitEnd");
}
