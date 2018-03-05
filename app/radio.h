/// \brief Library header file for the radio libraries to control radio chips.

/// Based on work by
/// Matthias Hertel, http://www.mathertel.de

#ifndef __RADIO_h__
#define __RADIO_h__

#include <Arduino.h>
#include <Wire.h>
#include "radio_structs.h"
#include "IRadio.h"

// The DEBUG_xxx Macros enable Information to the Serial port.
// They can be enabled by setting the _debugEnabled variable to true disabled by using the debugEnable function.
// When the code has to be minimized they can be redefined without implementation like:
// #define DEBUG_STR(txt) {}

/// Used for Debugging text information.
#define DEBUG_STR(txt)   \
  if (_debugEnabled)     \
  {                      \
    Serial.print('>');   \
    Serial.println(txt); \
  }

/// Used for Debugging function entries without parameters.
#define DEBUG_VAL(label, val) \
  if (_debugEnabled)          \
  {                           \
    Serial.print('>');        \
    Serial.print(label);      \
    Serial.print(':');        \
    Serial.println(val);      \
  }
#define DEBUG_VALX(label, val) \
  if (_debugEnabled)           \
  {                            \
    Serial.print('>');         \
    Serial.print(label);       \
    Serial.print(':');         \
    Serial.println(val, HEX);  \
  }

/// Used for Debugging function entries without parameters.
#define DEBUG_FUNC0(fn)   \
  if (_debugEnabled)      \
  {                       \
    Serial.print('>');    \
    Serial.print(fn);     \
    Serial.println("()"); \
  }

/// Used for Debugging function entries with 1 parameter.
#define DEBUG_FUNC1(fn, p1) \
  if (_debugEnabled)        \
  {                         \
    Serial.print('>');      \
    Serial.print(fn);       \
    Serial.print('(');      \
    Serial.print(p1);       \
    Serial.println(')');    \
  }

/// Used for Debugging function entries with 1 parameters as hex Value.
#define DEBUG_FUNC1X(fn, p1) \
  if (_debugEnabled)         \
  {                          \
    Serial.print('>');       \
    Serial.print(fn);        \
    Serial.print("(0x");     \
    Serial.print(p1, HEX);   \
    Serial.println(')');     \
  }

/// Used for Debugging function entries with 2 parameters.
#define DEBUG_FUNC2(fn, p1, p2) \
  if (_debugEnabled)            \
  {                             \
    Serial.print('>');          \
    Serial.print(fn);           \
    Serial.print('(');          \
    Serial.print(p1);           \
    Serial.print(", ");         \
    Serial.print(p2);           \
    Serial.println(')');        \
  }

/// Used for Debugging function entries with 2 parameters and Hex Value.
#define DEBUG_FUNC2X(fn, p1, p2) \
  if (_debugEnabled)             \
  {                              \
    Serial.print('>');           \
    Serial.print(fn);            \
    Serial.print("(0x");         \
    Serial.print(p1, HEX);       \
    Serial.print(", 0x");        \
    Serial.print(p2, HEX);       \
    Serial.println(')');         \
  }

// ----- common RADIO class definition -----
/// Library to control radio chips in general. This library acts as a base library for the chip specific implementations.
class RADIO : public IRadio
{

public:
  const uint8_t MAXVOLUME = 15; ///< max volume level for all radio implementations.

  RADIO(); ///< create a new object from this class.

  virtual void init(); ///< initialize library and the chip.
  virtual void term(); ///< terminate all radio functions.

  virtual RADIO_CAP getCapabilities() = 0;

  virtual bool getPower();
  virtual void setPower(bool value);

  // ----- Audio features -----

  virtual void setVolume(uint8_t newVolume); ///< Control the volume output of the radio chip in the range 0..15.
  virtual uint8_t getVolume();               ///< Retrieve the current output volume in the range 0..15.

  virtual void setMute(bool switchOn); ///< Control the mute mode of the radio chip.
  virtual bool getMute();              ///< Retrieve the current mute mode setting.

  virtual void setSoftMute(bool switchOn); ///< Control the softmute mode (mute on low signals) of the radio chip.
  virtual bool getSoftMute();              ///< Retrieve the current soft mute mode setting.

  virtual void setBassBoost(bool switchOn); ///< Control the bass boost mode of the radio chip.
  virtual bool getBassBoost();              ///< Retrieve the current bass boost mode setting.

  // ----- Receiver features -----

  virtual RADIO_FREQ getMinFrequency();  ///< Get the minimum frequency of the current selected band.
  virtual RADIO_FREQ getMaxFrequency();  ///< Get the maximum frequency of the current selected band.
  virtual RADIO_FREQ getFrequencyStep(); ///< Get resolution of the current selected band.

  virtual void setBand(RADIO_BAND newBand); ///< Set the current band.
  virtual RADIO_BAND getBand();             ///< Retrieve the current band setting.

  virtual void setFrequency(RADIO_FREQ newF); ///< Start using the new frequency for receiving.
  virtual RADIO_FREQ getFrequency(void);      ///< Retrieve the current tuned frequency.

  virtual void setBandFrequency(RADIO_BAND newBand, RADIO_FREQ newFreq); ///< Set Band and Frequency in one call.

  virtual void seekUp();   ///< Start a seek upwards from the current frequency.
  virtual void seekDown(); ///< Start a seek downwards from the current frequency.

  virtual void setMono(bool switchOn); ///< Control the mono mode of the radio chip.
  virtual bool getMono();              ///< Retrieve the current mono mode setting.

  // ----- combined status functions -----

  virtual void getRadioInfo(RADIO_INFO *info); ///< Retrieve some information about the current radio function of the chip.

  virtual void getAudioInfo(AUDIO_INFO *info); ///< Retrieve some information about the current audio function of the chip.

  // ----- Supporting RDS for FM bands -----

  virtual void checkRDS();                                       ///< Check if RDS Data is available and good.
  virtual void clearRDS();                                       ///< Clear RDS data in the attached RDS Receiver by sending 0,0,0,0.
  virtual void attachReceiveRDS(receiveRDSFunction newFunction); ///< Register a RDS processor function.

  // ----- Utilitys -----

  /// Format the current frequency for display and printing.
  virtual void formatFrequency(char *s, uint8_t length);

  virtual void formatFrequency(RADIO_FREQ freq, RADIO_BAND band, char *s, uint8_t length);
  // ----- debug Helpers send information to Serial port

  virtual void debugEnable(bool enable = true); ///< Enable sending debug information to the Serial port.
  virtual void debugRadioInfo();                ///< Print out all radio information.
  virtual void debugAudioInfo();                ///< Print out all audio information.
  virtual void debugStatus();                   ///< Send debug information about actual available chip functionality and other internal things.

protected:
  bool _debugEnabled; ///< Set by debugEnable() and controls debugging functionality.

  uint8_t _volume; ///< Last set volume level.
  bool _bassBoost; ///< Last set bass Boost effect.
  bool _mono;      ///< Last set mono effect.
  bool _mute;      ///< Last set mute effect.
  bool _softMute;  ///< Last set softMute effect.
  bool _power;

  RADIO_BAND _band; ///< Last set band.
  RADIO_FREQ _freq; ///< Last set frequency.

  RADIO_FREQ _freqLow;   ///< Lowest frequency of the current selected band.
  RADIO_FREQ _freqHigh;  ///< Highest frequency of the current selected band.
  RADIO_FREQ _freqSteps; ///< Resulution of the tuner.

  receiveRDSFunction _sendRDS; ///< Registered RDS Function that is called on new available data.

  void _printHex4(uint16_t val); ///> Prints a register as 4 character hexadecimal code with leading zeros.

private:
  void int16_to_s(char *s, uint16_t val); ///< Converts a int16 number to a string, similar to itoa, but using the format "00000".

}; // class RADIO

#endif

// End.