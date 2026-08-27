#pragma once

#include <qpixmap.h>
#include <qsize.h>

#include "dbusimage.h"
#include "statusnotifieritem.h"

namespace ns::systemtray {
class TrayImageHandle : public dbusprovider::BaseImageHandle {
public:
  explicit TrayImageHandle(StatusNotifierItem *item);

  QPixmap requestPixmap(const QString &id,
                        QSize         *size,
                        const QSize   &requestedSize) override;

private:
  StatusNotifierItem *m_item;
};
} // namespace ns::systemtray
