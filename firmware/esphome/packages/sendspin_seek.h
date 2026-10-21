#pragma once

// ESPHome's Sendspin hub has no seek action yet, and its send_client_command() cannot carry
// seek offsets. This reaches the hub's (protected) controller role to send SEEK_RELATIVE directly.
// Remove once ESPHome exposes seeking for Sendspin.

#include "esphome/components/sendspin/sendspin_hub.h"

#include <sendspin/controller_role.h>

namespace sendspin_seek {

// Explicit template instantiation may name non-public members, so this yields a pointer to
// SendspinHub::controller_role_ without modifying the (final) class.
struct HubControllerTag {
  using type = sendspin::ControllerRole *esphome::sendspin_::SendspinHub::*;
  friend type get(HubControllerTag);
};

template<typename Tag, typename Tag::type Member> struct MemberAccess {
  friend typename Tag::type get(Tag) { return Member; }
};

template struct MemberAccess<HubControllerTag, &esphome::sendspin_::SendspinHub::controller_role_>;

/// Seek the current group by a signed offset. The server ignores it when not supported.
inline void seek_relative(esphome::sendspin_::SendspinHub *hub, int32_t offset_ms) {
  sendspin::ControllerRole *controller = hub->*get(HubControllerTag{});
  if (controller == nullptr)
    return;
  controller->send_command({
      .command = sendspin::SendspinControllerCommand::SEEK_RELATIVE,
      .offset_ms = offset_ms,
  });
}

}  // namespace sendspin_seek
