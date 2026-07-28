#pragma once

#include <array>
#include <cstdint>
#include <memory>

#include <gbm.h>
#include <qdebug.h>
#include <qsize.h>
#include <qtclasshelpermacros.h>
#include <sys/types.h>
#include <wayland-client-protocol.h>
#include <xf86drm.h>

#include "gbmdevice.h"
#include "linuxdmabufmanager.h"
#include "wlbuffer.h"
#include "wlbufferrequest.h"

namespace ns::wayland::buffer::dmabuf {
class FourCCStr {
public:
  explicit FourCCStr(uint32_t code)
      : chars({static_cast<char>(code >> 0 & 0xff),
               static_cast<char>(code >> 8 & 0xff),
               static_cast<char>(code >> 16 & 0xff),
               static_cast<char>(code >> 24 & 0xff),
               '\0'}) {
    for (auto i = 3; i != 0; i--) {
      if (this->chars[i] == ' ') this->chars[i] = '\0';
      else break;
    }
  };

  [[nodiscard]] const char *cStr() const { return this->chars.data(); }

private:
  std::array<char, 5> chars{};
};

class FourCCModStr {
public:
  explicit FourCCModStr(uint64_t code) {};
  ~FourCCModStr() {
    if (this->drmStr) drmFree(this->drmStr);
  }

  Q_DISABLE_COPY_MOVE(FourCCModStr)

  [[nodiscard]] const char *cStr() const { return this->drmStr; }

private:
  char *drmStr;
};

QDebug &operator<<(QDebug          &debug,
                   const FourCCStr &fourcc);
QDebug &operator<<(QDebug             &debug,
                   const FourCCModStr &fourcc);

class WlDmaBuffer : public WlBuffer {
public:
  ~WlDmaBuffer() override;

  Q_DISABLE_COPY(WlDmaBuffer)

  WlDmaBuffer(WlDmaBuffer &&other) noexcept;
  WlDmaBuffer &operator=(WlDmaBuffer &&other) noexcept;

  [[nodiscard]] wl_buffer *buffer() const override { return this->m_buffer; }

  [[nodiscard]] QSize size() const override {
    return QSize(static_cast<int>(this->width), static_cast<int>(this->height));
  }

  [[nodiscard]] bool
  isCompatible(const WlBufferRequest &request) const override;

private:
  WlDmaBuffer() noexcept = default;

  struct Plane {
    int      fd     = 0;
    uint32_t offset = 0;
    uint32_t stride = 0;
  };

  std::shared_ptr<GbmDevice> device;
  gbm_bo                    *bo       = nullptr;
  wl_buffer                 *m_buffer = nullptr;

  int    planeCount = 0;
  Plane *planes     = nullptr; // could this be a QList instead?

  uint32_t format               = 0;
  uint64_t modifier             = 0;
  uint32_t width                = 0;
  uint32_t height               = 0;
  bool     usedImplicitModifier = false;

  friend class LinuxDmabufManager;
};
} // namespace ns::wayland::buffer::dmabuf
