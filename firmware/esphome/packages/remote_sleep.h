#pragma once

// Pin handling around deep sleep that ESPHome's deep_sleep component does not cover.

#include <driver/gpio.h>
#include <driver/rtc_io.h>
#include <esp_sleep.h>

#include <initializer_list>

namespace remote_sleep {

/// Called right before deep sleep.
///  - off_pin is driven low and held through sleep (display reset + backlight share it).
///  - pulldown_pins are active-high wake buttons: ESPHome does not keep their pull-downs
///    during sleep for ext1 wakeup, so a floating pin could wake the chip at random.
inline void prepare(gpio_num_t off_pin, std::initializer_list<gpio_num_t> pulldown_pins) {
  gpio_set_direction(off_pin, GPIO_MODE_OUTPUT);
  gpio_set_level(off_pin, 0);
  gpio_hold_en(off_pin);
  gpio_deep_sleep_hold_en();

  for (gpio_num_t pin : pulldown_pins) {
    rtc_gpio_pullup_dis(pin);
    rtc_gpio_pulldown_en(pin);
  }
  // RTC peripherals must stay powered for the RTC pull resistors to work in deep sleep
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
}

/// Called early at boot. Pin holds survive the wake-up, and a held reset pin would keep the
/// display in reset, so release every pin that may have been held for sleep.
inline void release(std::initializer_list<gpio_num_t> pins) {
  for (gpio_num_t pin : pins)
    gpio_hold_dis(pin);
  gpio_deep_sleep_hold_dis();
}

}  // namespace remote_sleep
