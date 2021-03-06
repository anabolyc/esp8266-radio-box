/// \brief Implementation for the radio library to control the RDA5807M radio chip.
///
/// Based on work by 
/// \author Matthias Hertel, http://www.mathertel.de
#include <RDA5807M.h>

// initialize the extra variables in RDA5807M
RDA5807M::RDA5807M()
{
  // t.b.d. ???
}

// initialize all internals.
void RDA5807M::init()
{
  bool result = false; // no chip found yet.
  DEBUG_FUNC0("init");

  Wire.begin();
  Wire.beginTransmission(I2C_INDX);
  result = Wire.endTransmission();
  if (result == 0)
  {
    DEBUG_STR("radio found.");
    result = true;

    // initialize all registers
    registers[RADIO_REG_CHIPID] = 0x5804; // 00 id
    registers[1] = 0x0000;                // 01 not used
    registers[RADIO_REG_CTRL] = (RADIO_REG_CTRL_RESET | RADIO_REG_CTRL_ENABLE);
    setBand(RADIO_BAND_FM);
    registers[RADIO_REG_R4] = RADIO_REG_R4_EM50; //  0x1800;  // 04 DE ? SOFTMUTE
    registers[RADIO_REG_VOL] = 0x9081;           // 0x81D1;  // 0x82D1 / INT_MODE, SEEKTH=0110,????, Volume=1
    registers[6] = 0x0000;
    registers[7] = 0x0000;
    registers[8] = 0x0000;
    registers[9] = 0x0000;

    // reset the chip
    _saveRegisters();

    registers[RADIO_REG_CTRL] = RADIO_REG_CTRL_ENABLE;
    _saveRegister(RADIO_REG_CTRL);
  } // if
}

// switch the power off
void RDA5807M::term()
{
  DEBUG_FUNC0("term");
  setVolume(0);
  registers[RADIO_REG_CTRL] = 0x0000; // all bits off
  _saveRegisters();
} // term

// ----- Volume control -----

bool RDA5807M::volumeEnabled() {
  return true;
}

void RDA5807M::setVolume(uint8_t newVolume)
{
  RADIO::setVolume(newVolume);
  newVolume &= RADIO_REG_VOL_VOL;
  registers[RADIO_REG_VOL] &= (~RADIO_REG_VOL_VOL);
  registers[RADIO_REG_VOL] |= newVolume;
  _saveRegister(RADIO_REG_VOL);
} // setVolume()

void RDA5807M::setBassBoost(bool switchOn)
{
  RADIO::setBassBoost(switchOn);
  uint16_t regCtrl = registers[RADIO_REG_CTRL];
  if (switchOn)
    regCtrl |= RADIO_REG_CTRL_BASS;
  else
    regCtrl &= (~RADIO_REG_CTRL_BASS);
  registers[RADIO_REG_CTRL] = regCtrl;
  _saveRegister(RADIO_REG_CTRL);
} // setBassBoost()

// Mono / Stereo
void RDA5807M::setMono(bool switchOn)
{
  RADIO::setMono(switchOn);

  registers[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_SEEK);
  if (switchOn)
  {
    registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_MONO;
  }
  else
  {
    registers[RADIO_REG_CTRL] &= ~RADIO_REG_CTRL_MONO;
  }
  _saveRegister(RADIO_REG_CTRL);
} // setMono

// Switch mute mode.
void RDA5807M::setMute(bool switchOn)
{
  RADIO::setMute(switchOn);

  if (switchOn)
  {
    // now don't unmute
    registers[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_UNMUTE);
  }
  else
  {
    // now unmute
    registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_UNMUTE;
  } // if
  _saveRegister(RADIO_REG_CTRL);
} // setMute()

// Switch softmute mode.
void RDA5807M::setSoftMute(bool switchOn)
{
  RADIO::setSoftMute(switchOn);

  if (switchOn)
  {
    registers[RADIO_REG_R4] |= (RADIO_REG_R4_SOFTMUTE);
  }
  else
  {
    registers[RADIO_REG_R4] &= (~RADIO_REG_R4_SOFTMUTE);
  } // if
  _saveRegister(RADIO_REG_R4);
} // setSoftMute()

// ----- Band and frequency control methods -----

// tune to new band.
void RDA5807M::setBand(RADIO_BAND newBand)
{
  uint16_t r;
  RADIO::setBand(newBand);

  if (newBand == RADIO_BAND_FM)
    r = RADIO_REG_CHAN_BAND_FM;
  else if (newBand == RADIO_BAND_FMWORLD)
    r = RADIO_REG_CHAN_BAND_FMWORLD;
  registers[RADIO_REG_CHAN] = (r | RADIO_REG_CHAN_SPACE_100);
} // setBand()

// retrieve the real frequency from the chip after automatic tuning.
RADIO_FREQ RDA5807M::getFrequency()
{
  // check register A

  while (true)
  {
    Wire.requestFrom(I2C_SEQ, 2);
    uint16_t readValue = _read16();
    if (readValue != 0xffff)
    {
      registers[RADIO_REG_RA] = readValue;
      Wire.endTransmission();
      break;
    }
  }

  uint16_t ch = registers[RADIO_REG_RA] & RADIO_REG_RA_NR;
  /*
  Serial.println("----FREQ----");
  Serial.print(" reg = ");
  Serial.print(registers[RADIO_REG_RA]);
  Serial.print(" 0xffff = ");
  Serial.print(0xffff);
  Serial.print(" ch = ");
  Serial.print(ch);
  Serial.print(" _freqLow = ");
  Serial.println(_freqLow);
  Serial.println("------------");
*/
  _freq = _freqLow + (ch * 10); // assume 100 kHz spacing
  return (_freq);
} // getFrequency

void RDA5807M::setFrequency(RADIO_FREQ newF)
{
  DEBUG_FUNC1("setFrequency", newF);
  uint16_t newChannel;
  uint16_t regChannel = registers[RADIO_REG_CHAN] & (RADIO_REG_CHAN_SPACE | RADIO_REG_CHAN_BAND);

  if (newF < _freqLow)
    newF = _freqLow;
  if (newF > _freqHigh)
    newF = _freqHigh;
  newChannel = (newF - _freqLow) / 10;

  regChannel += RADIO_REG_CHAN_TUNE; // enable tuning
  regChannel |= newChannel << 6;

  // enable output and unmute
  registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_OUTPUT | RADIO_REG_CTRL_UNMUTE | RADIO_REG_CTRL_RDS | RADIO_REG_CTRL_ENABLE; //  | RADIO_REG_CTRL_NEW
  _saveRegister(RADIO_REG_CTRL);

  registers[RADIO_REG_CHAN] = regChannel;
  _saveRegister(RADIO_REG_CHAN);

  // adjust Volume
  _saveRegister(RADIO_REG_VOL);
} // setFrequency()

// start seek mode upwards
void RDA5807M::seekUp()
{
  // start seek mode
  registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_SEEKUP;
  registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_SEEK;
  _saveRegister(RADIO_REG_CTRL);

  if (!toNextSender)
  {
    // stop scanning right now
    registers[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_SEEK);
    _saveRegister(RADIO_REG_CTRL);
  } // if
} // seekUp()

// start seek mode downwards
void RDA5807M::seekDown()
{
  registers[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_SEEKUP);
  registers[RADIO_REG_CTRL] |= RADIO_REG_CTRL_SEEK;
  _saveRegister(RADIO_REG_CTRL);

  if (!toNextSender)
  {
    // stop scanning right now
    registers[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_SEEK);
    _saveRegister(RADIO_REG_CTRL);
  } // if
} // seekDown()

// Load all status registers from to the chip
// registers 0A through 0F
// using the sequential read access mode.
void RDA5807M::_readRegisters()
{
  Wire.requestFrom(I2C_SEQ, (6 * 2));
  for (int i = 0; i < 6; i++)
  {
    registers[0xA + i] = _read16();
  }
  Wire.endTransmission();
}

// Save writable registers back to the chip
// The registers 02 through 06, containing the configuration
// using the sequential write access mode.
void RDA5807M::_saveRegisters()
{
  DEBUG_FUNC0("-saveRegisters");
  Wire.beginTransmission(I2C_SEQ);
  for (int i = 2; i <= 6; i++)
    _write16(registers[i]);
  Wire.endTransmission();
} // _saveRegisters

// Save one register back to the chip
void RDA5807M::_saveRegister(byte regNr)
{
  DEBUG_FUNC2X("-_saveRegister", regNr, registers[regNr]);

  Wire.beginTransmission(I2C_INDX);
  Wire.write(regNr);
  _write16(registers[regNr]);
  Wire.endTransmission();
} // _saveRegister

// write a register value using 2 bytes into the Wire.
void RDA5807M::_write16(uint16_t val)
{
  Wire.write(val >> 8);
  Wire.write(val & 0xFF);
} // _write16

// read a register value using 2 bytes in a row
uint16_t RDA5807M::_read16(void)
{
  uint8_t hiByte = Wire.read();
  uint8_t loByte = Wire.read();
  return (256 * hiByte + loByte);
} // _read16

// return current Radio Station Strength Information
// uint8_t RDA5807M::getRSSI() {
//   _readRegisters();
//   uint8_t rssi = registers[RADIO_REG_RB] >> 10;
//   return(rssi);
// } // getRSSI

void RDA5807M::checkRDS()
{
  // DEBUG_FUNC0("checkRDS");

  // check RDS data if there is a listener !
  if (_sendRDS)
  {

    // check register A
    Wire.requestFrom(I2C_SEQ, 2);
    registers[RADIO_REG_RA] = _read16();
    Wire.endTransmission();

    if (registers[RADIO_REG_RA] & RADIO_REG_RA_RDSBLOCK)
    {
      DEBUG_STR("BLOCK_E found.");
    } // if

    if (registers[RADIO_REG_RA] & RADIO_REG_RA_RDS)
    {
      // check for new RDS data available
      uint16_t newData;
      bool result = false;

      Wire.beginTransmission(I2C_INDX); // Device 0x11 for random access
      Wire.write(RADIO_REG_RDSA);       // Start at Register 0x0C
      Wire.endTransmission(0);          // restart condition

      Wire.requestFrom(I2C_INDX, 8, 1); // Retransmit device address with READ, followed by 8 bytes
      newData = _read16();
      if (newData != registers[RADIO_REG_RDSA])
      {
        registers[RADIO_REG_RDSA] = newData;
        result = true;
      }

      newData = _read16();
      if (newData != registers[RADIO_REG_RDSB])
      {
        registers[RADIO_REG_RDSB] = newData;
        result = true;
      }

      newData = _read16();
      if (newData != registers[RADIO_REG_RDSC])
      {
        registers[RADIO_REG_RDSC] = newData;
        result = true;
      }

      newData = _read16();
      if (newData != registers[RADIO_REG_RDSD])
      {
        registers[RADIO_REG_RDSD] = newData;
        result = true;
      }

      Wire.endTransmission();
      // _printHex(registers[RADIO_REG_RDSA]); _printHex(registers[RADIO_REG_RDSB]);
      // _printHex(registers[RADIO_REG_RDSC]); _printHex(registers[RADIO_REG_RDSD]);
      // Serial.println();

      if (result)
      {
        // new data in the registers
        // send to RDS decoder
        _sendRDS(registers[RADIO_REG_RDSA], registers[RADIO_REG_RDSB], registers[RADIO_REG_RDSC], registers[RADIO_REG_RDSD]);
      } // if
    }   // if
  }
}

/// Retrieve all the information related to the current radio receiving situation.
void RDA5807M::getRadioInfo(RADIO_INFO *info)
{

  RADIO::getRadioInfo(info);

  // read data from registers A .. F of the chip into class memory
  _readRegisters();
  info->active = true; // ???
  if (registers[RADIO_REG_RA] & RADIO_REG_RA_STEREO)
    info->stereo = true;
  if (registers[RADIO_REG_RA] & RADIO_REG_RA_RDS)
    info->rds = true;
  info->rssi = registers[RADIO_REG_RB] >> 10;
  if (registers[RADIO_REG_RB] & RADIO_REG_RB_FMTRUE)
    info->tuned = true;
  if (registers[RADIO_REG_CTRL] & RADIO_REG_CTRL_MONO)
    info->mono = true;
} // getRadioInfo()

// ----- Debug functions -----

void RDA5807M::debugScan()
{
  DEBUG_FUNC0("debugScan");
  uint16_t regChannel = registers[RADIO_REG_CHAN] & (RADIO_REG_CHAN_SPACE | RADIO_REG_CHAN_BAND);
  RADIO_FREQ f = _freqLow;
  int channel = 0;

  while (f < _freqHigh)
  {
    registers[RADIO_REG_CHAN] = regChannel | RADIO_REG_CHAN_TUNE | (channel << 6);
    _saveRegister(RADIO_REG_CHAN);

    delay(500);
    debugStatus();

    f += _freqSteps;
    channel += 1;
  } // while
} // debugScan

// send a status report to the serial port
// dump all registers to Serial output
void RDA5807M::debugStatus()
{
  char s[12];

  // read data from registers A .. F of the chip into class memory
  _readRegisters();

  formatFrequency(s, sizeof(s));
  Serial.print("Frequency=");
  Serial.print(s);

  uint16_t pi = registers[RADIO_REG_RDSA];
  Serial.print(" PI=");
  _printHex4(pi);

  Serial.print((registers[RADIO_REG_RA] & RADIO_REG_RA_STEREO) ? " Stereo" : " Mono  ");
  Serial.print((registers[RADIO_REG_RA] & RADIO_REG_RA_RDS) ? " ---" : " RDS");

  int rssi = registers[RADIO_REG_RB] >> 10;

  Serial.print(" Sig=");
  if (rssi < 10)
    Serial.write(' ');
  Serial.print(rssi);
  Serial.print(' ');
  for (int i = 0; i < rssi - 15; i++)
  {
    Serial.write('*');
  } // Empfangspegel ab 15. Zeichen
  Serial.println();

  // ruler
  Serial.println("0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F");
  // variables
  for (int n = 0; n < 16; n++)
  {
    _printHex4(registers[n]);
  }
  Serial.println();

  // registers
  Wire.beginTransmission(I2C_INDX);  // Device 0x11 for random access
  Wire.write(0x00);                  // Start at Register 0x0C
  Wire.endTransmission(0);           // restart condition
  Wire.requestFrom(I2C_INDX, 32, 1); // Retransmit device address with READ, followed by 8 bytes
  for (int n = 0; n < 16; n++)
  {
    _printHex4(_read16());
  }
  Wire.endTransmission();
  Serial.println();

  // clear text information in Registers
  if (getBassBoost())
    Serial.print("BassBoost ");
  if (getMono())
    Serial.print("Mono ");
  int v = getVolume();
  Serial.print("Volume=");
  Serial.print(v);
  Serial.print(' ');
  Serial.println();

} // debugStatus

// ----- internal functions -----

// The End.
