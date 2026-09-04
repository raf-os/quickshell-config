#include "dbusmenupngimage.h"

#include <qpixmap.h>
#include <qsize.h>

#include "dbusmenumodel.h"

namespace ns::dbusmenu {
QPixmap DBusMenuPngImage::requestPixmap(
    const QString & /*unused*/, QSize *size, const QSize & /*unused*/) {
  auto pixmap = QPixmap();
  if (!pixmap.loadFromData(this->data, "PNG")) {
    qCWarning(logNSDBusMenu) << "Unable to load dbusmenu item png";
  }

  if (size != nullptr) *size = pixmap.size();
  return pixmap;
}
} // namespace ns::dbusmenu
