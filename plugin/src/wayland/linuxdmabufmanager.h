#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <qcontainerfwd.h>
#include <qlist.h>
#include <qtclasshelpermacros.h>
#include <qwayland-linux-dmabuf-v1.h>
#include <qwaylandclientextension.h>
#include <wayland-client-protocol.h>
#include <wayland-linux-dmabuf-v1-client-protocol.h>

#include "dmabuf.h"
#include "inplacevector.h"
#include "manager_p.h"

namespace ns::wayland::buffer::dmabuf {
struct LinuxDmabufModifiers {
  InplaceVector<uint64_t, 10> modifiers;
  bool                        implicit = false;
};

struct LinuxDmabufFormatSelection {
  bool                                                    sorted = false;
  InplaceVector<QPair<uint32_t, LinuxDmabufModifiers>, 2> formats;
  void                                                    ensureSorted();
};

struct LinuxDmabufTranche {
  dev_t                      device = 0;
  uint32_t                   flags  = 0;
  LinuxDmabufFormatSelection formats;
};

class LinuxDmabufFeedback : public QtWayland::zwp_linux_dmabuf_feedback_v1 {
public:
  explicit LinuxDmabufFeedback(::zwp_linux_dmabuf_feedback_v1 *feedback);
  ~LinuxDmabufFeedback() override;
  Q_DISABLE_COPY_MOVE(LinuxDmabufFeedback)

private:
  dev_t                     mainDevice = 0;
  QList<LinuxDmabufTranche> tranches;
  void                     *formatTable     = nullptr;
  uint32_t                  formatTableSize = 0;
};

class LinuxDmabufManager
    : public QWaylandClientExtensionTemplate<LinuxDmabufManager>,
      public QtWayland::zwp_linux_dmabuf_v1 {
public:
  explicit LinuxDmabufManager(WlBufferManagerPrivate *manager);

  // [[nodiscard]] WlBuffer *createDmabuf(const WlBufferRequest &request);
  // [[nodiscard]] WlBuffer *createDmabuf(const std::shared_ptr<GbmDevice>
  // &device,
  //                                      uint32_t format, const
  //                                      LinuxDmabufModifiers &modifiers,
  //                                      uint32_t                    width,
  //                                      uint32_t                    height);

private:
  std::shared_ptr<GbmDevice> getGbmDevice(dev_t handle);

  void feedbackDone();

  LinuxDmabufFormatSelection      renderFormats;
  std::string                     renderNode;
  QList<LinuxDmabufTranche>       tranches;
  QList<std::weak_ptr<GbmDevice>> gbmDevices;
  WlBufferManagerPrivate         *manager;

  friend class LinuxDmabufFeedback;
  friend struct GbmDevice;
};
} // namespace ns::wayland::buffer::dmabuf
