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
  QSize resolvedSize = requestedSize.isValid() ? requestedSize : QSize(24, 24);
  auto idx = id.indexOf("/");

  if (idx < 0 || idx > id.size())
    return handleQtIcon(id, size, resolvedSize);
  else {
    if (id.first(idx) == "qt") {
      return handleQtIcon(id.last(idx), size, resolvedSize);
    } else if (id.first(idx) == "shell") {
      return handleShellIcon(id.last(idx), size, resolvedSize);
    } else {
      return placeholderIcon(resolvedSize);
    }
  }
}

QPixmap IconImageProvider::handleQtIcon(const QString &name, QSize *size,
                                        const QSize &resolvedSize) {
  auto requestedIcon = QIcon::fromTheme(name);

  if (requestedIcon.isNull()) {
    return placeholderIcon(resolvedSize);
  }

  if (size) {
    *size = resolvedSize;
  }

  auto pixmap = requestedIcon.pixmap(resolvedSize, QIcon::Normal);
  return pixmap;
}

QPixmap IconImageProvider::handleShellIcon(const QString &name, QSize *size,
                                           const QSize &resolvedSize) {
  // TODO: This
  return placeholderIcon(resolvedSize);
}

QPixmap IconImageProvider::placeholderIcon(const QSize &resolvedSize) {
  QPixmap placeholder(resolvedSize);
  placeholder.fill(Qt::transparent);
  return placeholder;
}

void IconImageProviderExtensionPlugin::initializeEngine(QQmlEngine *engine,
                                                        const char *uri) {
  Q_UNUSED(uri);
  QIcon::setThemeName("breeze-dark");
  engine->addImageProvider("qicons", new IconImageProvider);
}
} // namespace iconprovider
} // namespace ns
