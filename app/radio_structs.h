#ifndef __RADIO_STRUCTS_H__
#define __RADIO_STRUCTS_H__

#include <Arduino.h>

/// callback function for passing RDS data.
extern "C" {
  typedef void (*receiveRDSFunction)(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4);
}

/// Band datatype.
/// The BANDs a receiver probably can implement.
enum RADIO_BAND
{
  RADIO_BAND_NONE = 0, ///< No band selected.

  RADIO_BAND_FM = 1,      ///< FM band 87.5 � 108 MHz (USA, Europe) selected.
  RADIO_BAND_FMWORLD = 2, ///< FM band 76 � 108 MHz (Japan, Worldwide) selected.
  RADIO_BAND_AM = 3,      ///< AM band selected.
  RADIO_BAND_KW = 4,      ///< KW band selected.

  RADIO_BAND_MAX = 4 ///< Maximal band enumeration value.
};

/// Frequency data type.
/// Only 16 bits are used for any frequency value (not the real one)
typedef uint16_t RADIO_FREQ;

/// A structure that contains information about the radio features from the chip.
struct RADIO_INFO
{
  bool active;  ///< receiving is active.
  uint8_t rssi; ///< Radio Station Strength Information.
  uint8_t snr;  ///< Signal Noise Ratio.
  bool rds;     ///< RDS information is available.
  bool tuned;   ///< A stable frequency is tuned.
  bool mono;    ///< Mono mode is on.
  bool stereo;  ///< Stereo audio is available
};

/// a structure that contains information about the audio features
struct AUDIO_INFO
{
  uint8_t volume;
  bool power;
  bool mute;
  bool softmute;
  bool bassBoost;
};

struct RADIO_CAP
{
  bool volume;
  bool bassBoost;
  bool rds;
};

#endif