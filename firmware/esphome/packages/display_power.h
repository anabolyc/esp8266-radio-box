#pragma once

// Turns the MIPI SPI display off and back on at runtime.
//
// On this board the display reset and backlight share one pin, so "off" holds the panel in
// reset (backlight off) and "on" has to bring it out of reset and send the whole init sequence
// again. The mipi_spi component only initialises the panel once, in setup(), and frees its init
// sequence afterwards; calling setup() again would also register the SPI device a second time.
// So the sequence is copied before setup() runs, and replayed here the same way setup() does.

#include "esphome/core/hal.h"
#include "esphome/core/log.h"

#include <cstdint>
#include <vector>

namespace display_power {

static const char *const TAG = "display_power";

// Same encoding as mipi_spi: {cmd, len | 0x80?, args...} or {ms, DELAY_FLAG}
static constexpr uint8_t DELAY_FLAG = 0xFF;

inline std::vector<uint8_t> &saved_init_sequence() {
  static std::vector<uint8_t> sequence;
  return sequence;
}

// Grants access to the display's protected members. The subclass adds no members of its own,
// and is only used to reach them through a pointer to the real display object.
template<typename Display> struct Access : Display {
  static std::vector<uint8_t> &init_sequence(Display *d) { return static_cast<Access *>(d)->init_sequence_; }
  static esphome::GPIOPin *reset_pin(Display *d) { return static_cast<Access *>(d)->reset_pin_; }
  static void write_command(Display *d, uint8_t cmd, const uint8_t *bytes, size_t len) {
    static_cast<Access *>(d)->write_command_(cmd, bytes, len);
  }
  static void reset_params(Display *d) { static_cast<Access *>(d)->reset_params_(); }
};

/// Call before the display's setup() (on_boot priority above the display's), while the
/// init sequence is still there.
template<typename Display> void save_init_sequence(Display *display) {
  saved_init_sequence() = Access<Display>::init_sequence(display);
}

/// Hold the panel in reset, which also turns the backlight off.
template<typename Display> void turn_off(Display *display) {
  esphome::GPIOPin *reset = Access<Display>::reset_pin(display);
  if (reset != nullptr)
    reset->digital_write(false);
}

/// Bring the panel out of reset and initialise it again, like mipi_spi's setup() does.
template<typename Display> void turn_on(Display *display) {
  esphome::GPIOPin *reset = Access<Display>::reset_pin(display);
  if (reset != nullptr) {
    reset->digital_write(true);
    esphome::delay(5);
    reset->digital_write(false);
    esphome::delay(5);
    reset->digital_write(true);
  }

  // The panel accepts SLPOUT 120ms after reset; a delay entry of 0 means "until then"
  const uint32_t ready_at = esphome::millis() + 120;
  const auto &seq = saved_init_sequence();
  size_t index = 0;
  while (index + 2 <= seq.size()) {
    uint8_t cmd = seq[index++];
    uint8_t x = seq[index++];
    if (x == DELAY_FLAG) {
      uint32_t ms = cmd;
      if (ms == 0) {
        int32_t remaining = (int32_t) (ready_at - esphome::millis());
        ms = remaining > 0 ? remaining : 0;
      }
      esphome::delay(ms);
      continue;
    }
    uint8_t num_args = x & 0x7F;
    if (index + num_args > seq.size()) {
      ESP_LOGE(TAG, "Malformed init sequence");
      return;
    }
    Access<Display>::write_command(display, cmd, seq.data() + index, num_args);
    index += num_args;
  }
  // Inversion, rotation (MADCTL) and brightness are sent separately from the sequence
  Access<Display>::reset_params(display);
}

}  // namespace display_power
