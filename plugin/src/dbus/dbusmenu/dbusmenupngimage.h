#pragma once

#include <qstringview.h>

#include "dbusimage.h"

namespace ns::dbusmenu {
class DBusMenuPngImage : public dbusprovider::BaseIndexedImageHandle {
public:
  [[nodiscard]] bool hasData() const { return !this->data.isEmpty(); }

  QPixmap requestPixmap(
      const QString &id, QSize *size, const QSize &requestedSize) override;

  QByteArray data;
};
} // namespace ns::dbusmenu
