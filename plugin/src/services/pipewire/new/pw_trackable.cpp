#include "pw_trackable.h"

#include <pipewire/core.h>
#include <pipewire/proxy.h>
#include <qhashfunctions.h>
#include <qmap.h>
#include <qobject.h>
#include <qtypes.h>

#include "pw_globals.h"
#include "pw_registry.h"

namespace ns::pipewire {
TrackedObjectBase::TrackedObjectBase(quint32 id, quint32 perms,
    const char *interface, quint32 version,
    const QMap<QString, QString> &initProps, QObject *parent)
    : QObject(parent), m_id(id), m_perms(perms) {
  auto reg = Globals::instance()->registry();
  setParent(reg);

  auto object = pw_registry_bind(reg->object(), id, interface, version, 0);
  m_object    = static_cast<pw_proxy *>(object);

  bindHooks();
  this->initProps(initProps);
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

TrackableObjectBase::TrackableObjectBase(quint32 id, quint32 perms)
    : m_id(id), m_perms(perms) {}
} // namespace ns::pipewire
