#include "eager_buffer_initializer.h"

#include <qlogging.h>
#include <qobject.h>
#include <qquickitem.h>
#include <qquickwindow.h>

#include "manager.h"

namespace ns::wayland {
EagerBufferInitializer::EagerBufferInitializer(QQuickItem *parent)
    : QQuickItem(parent) {
  QObject::connect(
      this, &QQuickItem::windowChanged, this, [this](QQuickWindow *newwindow) {
        if (!newwindow) return;

        buffer::WlBufferManager::instance()->initWindow(this->window());
      });
}

void EagerBufferInitializer::componentComplete() {
  QQuickItem::componentComplete();

  // buffer::WlBufferManager::instance()->initWindow(this->window());
}
} // namespace ns::wayland
