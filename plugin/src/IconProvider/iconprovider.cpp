#include "iconprovider.h"

#include <qicon.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qqmlengine.h>
#include <qquickimageprovider.h>
#include <qsize.h>

namespace ns {
namespace iconprovider {
IconImageProvider::IconImageProvider()
    : QQuickImageProvider(
          QQuickImageProvider::Pixmap,
          QQmlImageProviderBase::ForceAsynchronousImageLoading) {}

QPixmap IconImageProvider::requestPixmap(const QString &id, QSize *size,
                                         const QSize &requestedSize) {
  QIcon::setThemeName("breeze-dark");
  auto requestedIcon = QIcon::fromTheme(id);

  if (requestedIcon.isNull()) {
    QPixmap placeholder(requestedSize.isValid() ? requestedSize
                                                : QSize(24, 24));
    placeholder.fill(Qt::transparent);
    return placeholder;
  }

  QSize resolvedSize = requestedSize.isValid() ? requestedSize : QSize(24, 24);

  if (size) {
    *size = resolvedSize;
  }

  auto pixmap = requestedIcon.pixmap(resolvedSize, QIcon::Normal, QIcon::On);
  return pixmap;
}

void IconImageProviderExtensionPlugin::initializeEngine(QQmlEngine *engine,
                                                        const char *uri) {
  Q_UNUSED(uri);
  engine->addImageProvider("qicons", new IconImageProvider);
}
} // namespace iconprovider
} // namespace ns
