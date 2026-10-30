#include "qml_object_interface.h"

#include <qobject.h>

#include "pw_trackable.h"

namespace ns::pipewire {
PwObjectInterface::PwObjectInterface(TrackedObjectBase *object, QObject *parent)
    : QObject(parent), m_tracked(object) {
  if (!m_tracked) return;

  QObject::connect(m_tracked, &QObject::destroyed, this,
      &PwObjectInterface::onTrackedDestroyed);
}
PwObjectInterface::~PwObjectInterface() {}

void PwObjectInterface::onTrackedDestroyed() {
  emit destroying();
  cleanup();
  m_tracked = nullptr;
  this->deleteLater();
}

TrackedObjectBase *PwObjectInterface::trackedObject() { return m_tracked; }
} // namespace ns::pipewire
