#pragma once

#include <cstdint>

#include <qvarlengtharray.h>
#include <sys/types.h>

namespace ns::wayland::buffer {
struct WlBufferRequest {
  uint32_t width  = 0;
  uint32_t height = 0;

  struct DmaFormat {
    DmaFormat() = default;
    DmaFormat(uint32_t format) : format(format) {}

    void               pushMod(uint64_t mod);
    [[nodiscard]] bool isModsDefined() const;

    uint32_t                      format = 0;
    QVarLengthArray<uint64_t, 10> modifiers;
    bool                          implicit = false;
  };

  struct {
    QVarLengthArray<uint32_t, 1> formats;
  } shm;

  struct {
    dev_t                         device = 0;
    QVarLengthArray<DmaFormat, 1> formats;
  } dmabuf;

  void reset();
};
} // namespace ns::wayland::buffer
