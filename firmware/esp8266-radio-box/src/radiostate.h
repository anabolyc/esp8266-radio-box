#pragma once

#include "radio.h"
#include "main.h"
#include "savedstate.h"
#include <Ticker.h>
#include <RDSParser.h>

#define REFRESH_STATE_PERIOD_MS 1000

const RADIO_BAND FIX_BAND = RADIO_BAND_FM;
const RADIO_FREQ FIX_STATION = 9920;

struct RadioCapabilities
{
  bool volume;
  bool bassBoost;
  bool rds;
};

class RadioState
{
public:
  RadioState(RadioCapabilities, Radio *);

  void init(void);

  int volume;
  bool mono = false;
  bool power = true;
  bool bassBoost;
  RADIO_BAND band;
  RADIO_FREQ freq;
  RadioCapabilities caps;

  Radio *radio;

  void setPower(bool);

private:
  RadioState_t stateObject;
  SavedState *_state;
  Ticker *_ticker;
  static RDSParser *rds;
};