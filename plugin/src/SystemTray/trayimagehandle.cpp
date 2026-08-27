#include "trayimagehandle.h"

#include <qpixmap.h>

#include "dbusimage.h"
#include "statusnotifieritem.h"

namespace ns::systemtray {
TrayImageHandle::TrayImageHandle(StatusNotifierItem *item)
    : dbusprovider::BaseImageHandle(), m_item(item) {}

QPixmap TrayImageHandle::requestPixmap(const QString & /*unused*/,
                                       QSize       *size,
                                       const QSize &requestedSize) {
  auto resolvedSize = requestedSize.isValid() ? requestedSize : QSize(24, 24);

  QPixmap pixmap = m_item->createPixmap(resolvedSize);
  if (pixmap.isNull()) {
    pixmap = dbusprovider::DBusPixmapImageProvider::placeholderPixmap(
        size, requestedSize);
  }
  return pixmap;
}
} // namespace ns::systemtray
