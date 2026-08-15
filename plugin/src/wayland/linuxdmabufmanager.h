#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <qcontainerfwd.h>
#include <qlist.h>
#include <qquickwindow.h>
#include <qtclasshelpermacros.h>
#include <qvarlengtharray.h>
#include <qwayland-linux-dmabuf-v1.h>
#include <qwaylandclientextension.h>
#include <wayland-client-protocol.h>
#include <wayland-linux-dmabuf-v1-client-protocol.h>
#include <wayland-util.h>

#include "gbmdevice.h"
#include "wlbuffer.h"
#include "wlbufferrequest.h"

namespace ns::wayland::buffer {
class WlBufferManagerPrivate;
}

namespace ns::wayland::buffer::dmabuf {
struct LinuxDmabufModifiers {
  QVarLengthArray<uint64_t, 10> modifiers;
  bool                          implicit = false;
};

struct LinuxDmabufFormatSelection {
  bool                                                      sorted = false;
  QVarLengthArray<QPair<uint32_t, LinuxDmabufModifiers>, 2> formats;
  void                                                      ensureSorted();
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

protected:
  void zwp_linux_dmabuf_feedback_v1_main_device(wl_array *device) override;
  void zwp_linux_dmabuf_feedback_v1_format_table(int32_t  fd,
                                                 uint32_t size) override;
  void
  zwp_linux_dmabuf_feedback_v1_tranche_target_device(wl_array *device) override;
  void zwp_linux_dmabuf_feedback_v1_tranche_flags(uint32_t flags) override;
  void zwp_linux_dmabuf_feedback_v1_tranche_formats(wl_array *indices) override;
  void zwp_linux_dmabuf_feedback_v1_tranche_done() override;
  void zwp_linux_dmabuf_feedback_v1_done() override;

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
  explicit LinuxDmabufManager(
      ns::wayland::buffer::WlBufferManagerPrivate *manager);

  [[nodiscard]] WlBuffer *
  createDmabufFromRequest(const WlBufferRequest &request);
  [[nodiscard]] WlBuffer *createDmabuf(const std::shared_ptr<GbmDevice> &device,
                                       uint32_t                          format,
                                       const LinuxDmabufModifiers &modifiers,
                                       uint32_t                    width,
                                       uint32_t                    height);

  bool                       initRenderFormats(QQuickWindow *window);
  static LinuxDmabufManager *getManager();

private:
  std::shared_ptr<GbmDevice> getGbmDevice(dev_t handle);

  bool initRenderFormatsVk(QQuickWindow *window);
  bool initRenderFormatsGl(QQuickWindow *window);

  void feedbackDone();

  LinuxDmabufFormatSelection                   renderFormats;
  std::string                                  renderNode;
  QList<LinuxDmabufTranche>                    tranches;
  QList<std::weak_ptr<GbmDevice>>              gbmDevices;
  ns::wayland::buffer::WlBufferManagerPrivate *manager;

  friend class LinuxDmabufFeedback;
  friend struct GbmDevice;
};
} // namespace ns::wayland::buffer::dmabuf
