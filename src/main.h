#pragma once

#if (RADIO_MODULE==0)
#include "TEA5767.h"
#define Radio TEA5767
#define RadioCaps {false, false, false}
#endif

#if (RADIO_MODULE==1)
#include <RDA5807M.h>
#define Radio RDA5807M
#define RadioCaps {true, true, true}
#endif