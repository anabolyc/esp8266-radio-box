#pragma once

// Creates the Sendspin artwork slot's JPEG decoder at boot.
//
// The decoder (~20 KB, mostly the JPEGDEC state) is normally created when the first artwork
// arrives, and kept for every later image. Without PSRAM the heap is fragmented by then, a
// block that size may no longer exist, and the failed allocation aborts. Creating it first
// thing at boot, while the heap is still in one piece, avoids that.

#include "esphome/components/runtime_image/runtime_image.h"
#include "esphome/components/sendspin/image/sendspin_image.h"

#include <esp_heap_caps.h>

#include <memory>

namespace artwork_prealloc {

using esphome::runtime_image::ImageDecoder;
using esphome::runtime_image::ImageFormat;
using esphome::runtime_image::RuntimeImage;
using esphome::sendspin_::ArtworkDecodeSink;
using esphome::sendspin_::SendspinImageSlot;

// Explicit template instantiation may name non-public members; these hand out pointers to them.
template<typename Tag, typename Tag::type Member> struct MemberAccess {
  friend typename Tag::type get(Tag) { return Member; }
};

struct DecodeSinkTag {
  using type = ArtworkDecodeSink SendspinImageSlot::*;
  friend type get(DecodeSinkTag);
};
struct DecoderTag {
  using type = std::unique_ptr<ImageDecoder> RuntimeImage::*;
  friend type get(DecoderTag);
};
struct CreateDecoderTag {
  using type = std::unique_ptr<ImageDecoder> (RuntimeImage::*)(ImageFormat);
  friend type get(CreateDecoderTag);
};

template struct MemberAccess<DecodeSinkTag, &SendspinImageSlot::decode_sink_>;
template struct MemberAccess<DecoderTag, &RuntimeImage::decoder_>;
template struct MemberAccess<CreateDecoderTag, &RuntimeImage::create_decoder_>;

/// Call from an early on_boot (before the artwork decode thread gets any work).
inline void create_decoder(SendspinImageSlot *slot, ImageFormat format) {
  RuntimeImage &sink = slot->*get(DecodeSinkTag{});
  std::unique_ptr<ImageDecoder> &decoder = sink.*get(DecoderTag{});
  if (!decoder)
    decoder = (sink.*get(CreateDecoderTag{}))(format);
  ESP_LOGI("artwork_prealloc", "Artwork decoder %s; free heap %u, largest block %u",
           decoder ? "created" : "could not be created", (unsigned) heap_caps_get_free_size(MALLOC_CAP_8BIT),
           (unsigned) heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
}

}  // namespace artwork_prealloc
