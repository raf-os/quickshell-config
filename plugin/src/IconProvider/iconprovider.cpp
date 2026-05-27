#include "iconprovider.h"

#include <qbuffer.h>
#include <qdir.h>
#include <qicon.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qqmlengine.h>
#include <qquickimageprovider.h>
#include <qsize.h>
#include <qstringview.h>
#include <qsvgrenderer.h>

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

  if (idx < 0 || idx + 1 > id.size())
    return handleQtIcon(id, size, resolvedSize);
  else {
    if (id.first(idx) == "qt") {
      return handleQtIcon(id.sliced(idx + 1), size, resolvedSize);
    } else if (id.first(idx) == "shell") {
      return handleShellIcon(id.sliced(idx + 1), size, resolvedSize);
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
  QFile iconFile(m_shellIconPath + "/" + name + ".svg");

  if (!iconFile.exists()) {
    qWarning() << "ns::iconprovider::IconImageProvider: Icon '" << name
               << "' does not exist.";
    return placeholderIcon(resolvedSize);
  }

  if (!iconFile.open(QIODevice::ReadOnly)) {
    qWarning() << "ns::iconprovider::IconImageProvider: Unable to open "
                  "requested icon file 'icons/"
               << name << ".svg'.";
    return placeholderIcon(resolvedSize);
  }

  QByteArray buffer(iconFile.size(), Qt::Uninitialized);
  iconFile.read(buffer.data(), buffer.size());

  iconFile.close();

  // TODO: manipulate SVG file data to edit stroke, fill, etc., maybe?

  QSvgRenderer renderer(buffer);
  QPixmap pixmap(resolvedSize);
  pixmap.fill(Qt::transparent);

  if (!renderer.isValid()) {
    qWarning() << "ns::iconprovider::IconImageProvider: Invalid svg data from "
                  "file 'icons/"
               << name << ".svg'.";
    return placeholderIcon(resolvedSize);
  }

  QPainter painter(&pixmap);
  renderer.render(&painter);
  return pixmap;
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
