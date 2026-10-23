#include "qml_pipewire.h"

#include <qobject.h>

#include "helpermacros.h"

namespace ns::services::pipewire::qml {
AUTO_MEYERS_SINGLETON_QML_IMPL(Pipewire)

Pipewire::Pipewire(QObject *parent) : QObject(parent) {}
} // namespace ns::services::pipewire::qml
