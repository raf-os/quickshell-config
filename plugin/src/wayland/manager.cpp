#include "manager.h"

#include <qjsengine.h>
#include <qloggingcategory.h>
#include <qqmlengine.h>
#include <qtenvironmentvariables.h>

#include "dmabuf.h"
#include "manager_p.h"
#include "shm.h"
#include "wlbuffer.h"
#include "wlbufferrequest.h"

namespace ns::wayland::buffer {
Q_DECLARE_LOGGING_CATEGORY(logNSDmabuf)

WlBufferManager::WlBufferManager() : p(new WlBufferManagerPrivate(this)) {}

WlBufferManager::~WlBufferManager() { delete this->p; }

WlBufferManager *WlBufferManager::instance() {
  static auto *s_instance = new WlBufferManager();
  return s_instance;
}

bool WlBufferManager::isReady() const {
  return this->p->dmabufFormatsReady && this->p->renderFormatsReady;
}

WlBuffer *WlBufferManager::createBuffer(const WlBufferRequest &request) {
  qCDebug(logNSDmabuf).nospace()
      << "Creating buffer from request: (" << request.width << ","
      << request.height << ")";
  qCDebug(logNSDmabuf).nospace()
      << "\tDmabuf requests on device " << request.dmabuf.device << ":";

  for (const auto &format : request.dmabuf.formats) {
    qCDebug(logNSDmabuf).nospace()
        << "\tFormat " << dmabuf::FourCCStr(format.format)
        << (format.modifiers.length() == 0 ? "(no modifiers)" : "");

    if (format.implicit) {
      qCDebug(logNSDmabuf) << "\t\tImplicit Modifier";
    }

    for (const auto &modifier : format.modifiers) {
      qCDebug(logNSDmabuf) << "\t\tExplicit Modifier"
                           << dmabuf::FourCCModStr(modifier);
    }
  }

  qCDebug(logNSDmabuf) << "\tShm requests:";

  for (const auto &format : request.shm.formats) {
    qCDebug(logNSDmabuf) << "\t\tFormat" << format;
  }

  if (request.width == 0 || request.height == 0) {
    qCWarning(logNSDmabuf) << "Attempted to create zero-sized buffer.";
    return nullptr;
  }

  if (!this->p->renderFormatsFailed) {
    if (auto *buf = this->p->dmabuf.createDmabuf(request)) return buf;
    qCWarning(logNSDmabuf)
        << "DMA buffer creation failed, falling back to SHM.";
  }

  // Fallback path, not ideal at all
  return shm::ShmbufManager::createShmbuf(request);
}

WlBufferManagerPrivate::WlBufferManagerPrivate(WlBufferManager *manager)
    : manager(manager),
      dmabuf(this) {}

void WlBufferManagerPrivate::dmabufReady() {
  this->dmabufFormatsReady = true;
  if (this->renderFormatsReady) emit this->manager->ready();
}
} // namespace ns::wayland::buffer
