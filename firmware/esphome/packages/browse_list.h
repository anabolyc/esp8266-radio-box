#pragma once

// Lists shown on the browse page, and other screens the side button cycles through.
// Each content package adds one list (e.g. somafm-stations.yaml, ambient-scenes.yaml) or
// one screen (e.g. lights.yaml) at boot. The side button visits the lists first, then the
// screens, then goes back to the player.

#include "esphome/components/font/font.h"
#include "esphome/components/image/image.h"

#include <functional>
#include <vector>

namespace browse {

/// What the middle button does with the selected item's target
enum class Action {
  PLAY_MEDIA,          // play target (a stream URL) on the speaker
  TRIGGER_AUTOMATION,  // trigger target (an automation entity ID)
};

struct Item {
  const char *title;
  const char *target;
  // Row icon: an image, or a glyph from a font
  esphome::image::Image *icon{nullptr};
  const char *glyph{nullptr};
  esphome::font::Font *glyph_font{nullptr};
};

struct List {
  const char *title;
  Action action;
  std::vector<Item> items;
  int selected{0};
};

inline std::vector<List> &lists() {
  static std::vector<List> all;
  return all;
}

/// Index into lists() of the open list, or -1 while the browse page is closed
inline int &current() {
  static int index = -1;
  return index;
}

/// A screen other than a list, with its own page and button handling
struct Screen {
  std::function<void()> show;
  std::function<void()> hide;
};

inline std::vector<Screen> &screens() {
  static std::vector<Screen> all;
  return all;
}

/// Index into screens() of the open screen, or -1
inline int &current_screen() {
  static int index = -1;
  return index;
}

inline List *current_list() {
  int i = current();
  return i >= 0 && i < (int) lists().size() ? &lists()[i] : nullptr;
}

inline Item *selected_item() {
  List *list = current_list();
  if (list == nullptr || list->items.empty())
    return nullptr;
  return &list->items[list->selected];
}

}  // namespace browse
