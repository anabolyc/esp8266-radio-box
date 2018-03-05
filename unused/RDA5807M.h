/// \brief Library header file for the radio library to control the RDA5807M radio chip.
///
/// Based on work by 
/// \author Matthias Hertel, http://www.mathertel.de

#ifndef RDA5807M_h
#define RDA5807M_h

#include <Arduino.h>
#include <Wire.h>
#include <radio.h>

// ----- Register Definitions -----

// this chip only supports FM mode
#define FREQ_STEPS 10

#define RADIO_REG_CHIPID  0x00

#define RADIO_REG_CTRL    0x02
#define RADIO_REG_CTRL_OUTPUT 0x8000
#define RADIO_REG_CTRL_UNMUTE 0x4000
#define RADIO_REG_CTRL_MONO   0x2000
#define RADIO_REG_CTRL_BASS   0x1000
#define RADIO_REG_CTRL_SEEKUP 0x0200
#define RADIO_REG_CTRL_SEEK   0x0100
#define RADIO_REG_CTRL_RDS    0x0008
#define RADIO_REG_CTRL_NEW    0x0004
#define RADIO_REG_CTRL_RESET  0x0002
#define RADIO_REG_CTRL_ENABLE 0x0001

#define RADIO_REG_CHAN    0x03
#define RADIO_REG_CHAN_SPACE     0x0003
#define RADIO_REG_CHAN_SPACE_100 0x0000
#define RADIO_REG_CHAN_BAND      0x000C
#define RADIO_REG_CHAN_BAND_FM      0x0000
#define RADIO_REG_CHAN_BAND_FMWORLD 0x0008
#define RADIO_REG_CHAN_TUNE   0x0010
//      RADIO_REG_CHAN_TEST   0x0020
#define RADIO_REG_CHAN_NR     0x7FC0

#define RADIO_REG_R4    0x04
#define RADIO_REG_R4_EM50   0x0800
//      RADIO_REG_R4_RES   0x0400
#define RADIO_REG_R4_SOFTMUTE   0x0200
#define RADIO_REG_R4_AFC   0x0100


#define RADIO_REG_VOL     0x05
#define RADIO_REG_VOL_VOL   0x000F


#define RADIO_REG_RA      0x0A
#define RADIO_REG_RA_RDS       0x8000
#define RADIO_REG_RA_RDSBLOCK  0x0800
#define RADIO_REG_RA_STEREO    0x0400
#define RADIO_REG_RA_NR        0x03FF

#define RADIO_REG_RB          0x0B
#define RADIO_REG_RB_FMTRUE   0x0100
#define RADIO_REG_RB_FMREADY  0x0080


#define RADIO_REG_RDSA   0x0C
#define RADIO_REG_RDSB   0x0D
#define RADIO_REG_RDSC   0x0E
#define RADIO_REG_RDSD   0x0F

// I2C-Address RDA Chip for sequential  Access
#define I2C_SEQ  0x10

// I2C-Address RDA Chip for Index  Access
#define I2C_INDX  0x11

// ----- library definition -----

/// Library to control the RDA5807M radio chip.
class RDA5807M : public RADIO {
  public:
    // ----- RDA5807M specific implementations -----
    const uint8_t MAXVOLUME = 15;   ///< max volume level for radio implementations.

  RDA5807M();
  
  void   init();
  void   term();
    
  // ----- Audio features -----
  bool   volumeEnabled();
  void   setVolume(uint8_t newVolume);
  void   setBassBoost(bool switchOn);
  void   setMono(bool switchOn);
  void   setMute(bool switchOn);
  void   setSoftMute(bool switchOn);    ///< Set the soft mute mode (mute on low signals) on or off.

  // ----- Receiver features -----
  void   setBand(RADIO_BAND newBand);
  void   setFrequency(RADIO_FREQ newF);
  RADIO_FREQ getFrequency(void);

  void    seekUp(bool toNextSender = true);   // start seek mode upwards
  void    seekDown(bool toNextSender = true); // start seek mode downwards
  
  // ----- Supporting RDS for RADIO_BAND_FM and RADIO_BAND_FMWORLD

  void    checkRDS();

  // ----- combined status functions -----

  virtual void getRadioInfo(RADIO_INFO *info); ///< Retrieve some information about the current radio function of the chip.

  // ----- Supporting RDS for RADIO_BAND_FM and RADIO_BAND_FMWORLD

  // ----- debug Helpers send information to Serial port
  
  void    debugScan();               // Scan all frequencies and report a status
  void    debugStatus();             // DebugInfo about actual chip data available

  private:
  // ----- local variables
  uint16_t registers[16];  // memory representation of the registers

  // ----- low level communication to the chip using I2C bus

  void     _readRegisters(); // read all status & data registers
  void     _saveRegisters();     // Save writable registers back to the chip
  void     _saveRegister(byte regNr); // Save one register back to the chip
  
  void     _write16(uint16_t val);        // Write 16 Bit Value on I2C-Bus
  uint16_t _read16(void);
};

#endif
