#pragma once

#include <qqmlintegration.h>
#include <qquickitem.h>
#include <qtmetamacros.h>
namespace ns::wayland {
class EagerBufferInitializer : public QQuickItem {
  Q_OBJECT
  QML_ELEMENT

public:
  explicit EagerBufferInitializer(QQuickItem *parent = nullptr);

  void componentComplete() override;
};
} // namespace ns::wayland
