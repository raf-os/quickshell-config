#include "manager.h"

#include <qjsengine.h>
#include <qloggingcategory.h>
#include <qqmlengine.h>
#include <qtenvironmentvariables.h>

#include "manager_p.h"
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

WlBufferManager *WlBufferManager::create(QQmlEngine *qmlEngine,
                                         QJSEngine * /*unused*/) {
  auto inst = instance();
  if (qmlEngine) qmlEngine->setObjectOwnership(inst, QQmlEngine::CppOwnership);
  return inst;
}

bool WlBufferManager::isReady() const {
  return this->p->dmabufFormatsReady && this->p->renderFormatsReady;
}

void *WlBufferManager::createBuffer(const WlBufferRequest &request) {
  static const bool dmabufDisabled =
      qEnvironmentVariableIsSet("QS_DISABLE_DMABUF");

  if (request.width == 0 || request.height == 0) {
    qCWarning(logNSDmabuf) << "Attempted to create zero-sized buffer.";
    return nullptr;
  }

  if (!dmabufDisabled && !this->p->renderFormatsFailed) {
    // if (auto &buf = this->p->dmabuf.createDmabuf(request)) return buf;
    // else it failed
  }

  // return shm buffer
  return this;
}

WlBufferManagerPrivate::WlBufferManagerPrivate(WlBufferManager *manager)
    : manager(manager),
      dmabuf(this) {}

void WlBufferManagerPrivate::dmabufReady() {
  this->dmabufFormatsReady = true;
  if (this->renderFormatsReady) emit this->manager->ready();
}
} // namespace ns::wayland::buffer
