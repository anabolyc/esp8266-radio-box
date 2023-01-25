#include <Arduino.h>
#include <EEPROM.h>
#include "savedstate.h"
#include "logger.h"

SavedState::SavedState()
{
#if defined(ESP8266) || defined(ESP32)
    EEPROM.begin(1 + sizeof(RadioState_t));
#endif
}

bool SavedState::isValid()
{
    return EEPROM.read(0) == EEPROM_MAGIC_NUMBER;
}

void SavedState::load(RadioState_t* state)
{
    logger->println("Loading state");
    EEPROM.get(1, *state);
}

void SavedState::erase() {
    logger->println("Erasing saved state");
    for (uint32_t i = 0; i < EEPROM.length(); i++)
        EEPROM.write(i, 0);
#if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
#endif
}

void SavedState::save(RadioState_t *state)
{
    logger->println("Saving state");
    uint32_t i = 0;
    if (EEPROM.read(0) != EEPROM_MAGIC_NUMBER) {
        logger->printf("EEPROM[0] reads %d\n", EEPROM.read(0));
        erase();
    }

#if defined(ESP8266) || defined(ESP32)
    EEPROM.write(0, EEPROM_MAGIC_NUMBER);
#else
    EEPROM.update(0, EEPROM_MAGIC_NUMBER);
#endif

    EEPROM.put(1, *state);

#if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
#endif
}