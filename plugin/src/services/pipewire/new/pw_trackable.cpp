#include "pw_trackable.h"

#include <pipewire/core.h>
#include <pipewire/proxy.h>
#include <qdebug.h>
#include <qhashfunctions.h>
#include <qmap.h>
#include <qobject.h>
#include <qtypes.h>

#include "pw_globals.h"
#include "pw_registry.h"

namespace ns::pipewire {
TrackedObjectBase::TrackedObjectBase(
    const TrackedObjectProps &props, QObject *parent)
    : QObject(parent), m_id(props.id), m_perms(props.perms) {
  auto reg = Globals::instance()->registry();
  setParent(reg);

  auto object =
      pw_registry_bind(reg->object(), m_id, props.interface, props.version, 0);
  m_object = static_cast<pw_proxy *>(object);

  bindHooks();
  this->initProps(props.initProps);
}

TrackedObjectBase::~TrackedObjectBase() {
  if (!m_object) return;
  unbindHooks();
  pw_proxy_destroy(m_object);
}

void TrackedObjectBase::kill() {
  onBeforeDestroy();
  delete this;
}

QDebug operator<<(QDebug debug, const TrackedObjectBase *object) {
  if (object == nullptr) {
    debug << "TrackableObjectBase(0x0)";
  } else {
    auto saver = QDebugStateSaver(debug);
    auto idx = QString(object->metaObject()->className()).lastIndexOf(':') + 1;
    debug.nospace() << (object->metaObject()->className() + idx) << '('
                    << static_cast<const void *>(object)
                    << ", id=" << object->m_id << ")";
  }
  return debug;
}

TrackableObjectBase::TrackableObjectBase(quint32 id, quint32 perms)
    : m_id(id), m_perms(perms) {}
} // namespace ns::pipewire
