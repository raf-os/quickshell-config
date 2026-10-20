#include "pw_bindable.h"

#include <pipewire/proxy.h>
#include <qdebug.h>
#include <qhashfunctions.h>
#include <qloggingcategory.h>
#include <qtypes.h>

#include "ns_services_pipewire_debug.h"
#include "pwregistry.h"

namespace ns::services::pipewire {
PwBindableBase::PwBindableBase(PwBindableProps props)
    : QObject(props.registry), m_id(props.id), m_perms(props.perms) {}

PwBindableBase::~PwBindableBase() {
  if (m_id != 0 || m_proxy) {
    qCFatal(NS_LOG_PIPEWIRE)
        << "Destroyed object" << this << "without calling kill() first.";
  }
}

void PwBindableBase::bindRegistry(const char *interface, quint32 version) {
  auto *object =
      pw_registry_bind(m_registry->registry(), m_id, interface, version, 0);
  m_proxy = static_cast<pw_proxy *>(object);
}

pw_proxy *PwBindableBase::proxy() { return m_proxy; }

void PwBindableBase::bind() { bindHooks(); }

void PwBindableBase::unbind() {
  if (!m_proxy) return;
  unbindHooks();
  pw_proxy_destroy(m_proxy);
  m_proxy = nullptr;
}

void PwBindableBase::kill() {
  unbind();
  m_id = 0;
  delete this;
}

QDebug operator<<(QDebug debug, const PwBindableBase *object) {
  if (object == nullptr) {
    debug << "PwBindableBase(0x0)";
  } else {
    auto saver = QDebugStateSaver(debug);
    auto idx = QString(object->metaObject()->className()).lastIndexOf(':') + 1;
    debug.nospace() << (object->metaObject()->className() + idx) << '('
                    << static_cast<const void *>(object)
                    << ", id=" << object->m_id << ")";
  }
  return debug;
}
} // namespace ns::services::pipewire
