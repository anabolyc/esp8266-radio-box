/// \brief Library header file for the radio library to control the TEA5767 radio chip.
///
/// Based on work by
/// \author Matthias Hertel, http://www.mathertel.de

#ifndef TEA5767_h
#define TEA5767_h

#include <Arduino.h>
#include <Wire.h>

#include "radio.h"

const RADIO_CAP TEA5767_CAPS = { false, false, false } ;

// ----- library definition -----
/// Library to control the TEA5767 radio chip.
class TEA5767 : public RADIO {
  public:
  const uint8_t MAXVOLUME = 15;   ///< max volume level for radio implementations.
  TEA5767();
  
  void   init();  // initialize library and the chip.
    
  // Control of the audio features
  RADIO_CAP getCapabilities();

  // control internal STAND BY mode of chip
  void setPower(bool value); 

  /// setVolume is a non-existing function in TEA5767. It will always be MAXVOLUME.
  void   setVolume(uint8_t newVolume);

  // Control the bass boost function of the radio chip
  void   setBassBoost(bool switchOn);

  // Control mono/stereo mode of the radio chip
  void   setMono(bool switchOn); // Switch to mono mode.

  // Control the mute function of the radio chip
  void   setMute(bool switchOn); // Switch to mute mode.

  // Control of the core receiver

  // Control the frequency
  void setBand(RADIO_BAND newBand);

  void    setFrequency(RADIO_FREQ newF);
  RADIO_FREQ getFrequency(void);

  void seekUp();   // start seek mode upwards
  void seekDown(); // start seek mode downwards
  
  void checkRDS(); // read RDS data from the current station and process when data available.
  
  void getRadioInfo(RADIO_INFO *info);
  void getAudioInfo(AUDIO_INFO *info);

  // ----- debug Helpers send information to Serial port
  
  void  debugScan();               // Scan all frequencies and report a status
  void  debugStatus();             // Report Info about actual Station

  // ----- read/write registers of the chip

  void  _readRegisters();  // read all status & data registers
  void  _saveRegisters();  // Save writable registers back to the chip

  private:
  // ----- local variables

  // store the current values of the 5 chip internal 8-bit registers
  uint8_t registers[5]; ///< registers for controlling the radio chip.
  uint8_t status[5];    ///< registers with the current status of the radio chip.


  // ----- low level communication to the chip using I2C bus

  void     _write16(uint16_t val);        // Write 16 Bit Value on I2C-Bus
  uint16_t _read16(void);
  
  void _seek(bool seekUp = true);
  void _waitEnd();
};

#endif
