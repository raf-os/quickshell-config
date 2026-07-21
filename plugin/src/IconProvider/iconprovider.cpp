#include "iconprovider.h"

#include <qbuffer.h>
#include <qdir.h>
#include <qicon.h>
#include <qimage.h>
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
#include <qthreadpool.h>

namespace ns::iconprovider {
namespace {
QImage PLACEHOLDER_ICON(const QSize &size) {
  QImage placeholder(size, QImage::Format_ARGB32);
  placeholder.fill(Qt::transparent);
  return placeholder;
}
} // namespace
AsyncIconResponseRunnable::AsyncIconResponseRunnable(const QString &id,
                                                     const QSize &requestedSize)
    : m_id(id),
      m_requestedSize(requestedSize) {}

void AsyncIconResponseRunnable::run() {
  QSize resolvedSize =
      m_requestedSize.isValid() ? m_requestedSize : QSize(24, 24);
  auto idx = m_id.indexOf("/");

  if (idx < 0 || idx + 1 > m_id.size()) {
    auto qm = handleQtIcon(m_id, resolvedSize);
    emit done(qm);
  } else if (m_id.first(idx) == "qt") {
    auto qm = handleQtIcon(m_id.sliced(idx + 1), resolvedSize);
    emit done(qm);
  } else if (m_id.first(idx) == "shell") {
    auto qm = handleShellIcon(m_id.sliced(idx + 1), resolvedSize);
    emit done(qm);
  } else {
    auto placeholder = PLACEHOLDER_ICON(resolvedSize);
    emit done(placeholder);
  }
}

QImage AsyncIconResponseRunnable::handleQtIcon(const QString &id,
                                               const QSize   &resolvedSize) {
  auto requestedIcon = QIcon::fromTheme(id);
  if (requestedIcon.isNull()) {
    return PLACEHOLDER_ICON(resolvedSize);
  }

  auto pmap = requestedIcon.pixmap(resolvedSize, QIcon::Normal).toImage();
  return pmap;
}

QImage AsyncIconResponseRunnable::handleShellIcon(const QString &id,
                                                  const QSize   &resolvedSize) {
  QFile iconFile(QString::fromUtf8(ICON_DEFAULT_PATH) + "/icons/" + id +
                 ".svg");

  if (!iconFile.exists()) return PLACEHOLDER_ICON(resolvedSize);

  if (!iconFile.open(QIODevice::ReadOnly)) {
    return PLACEHOLDER_ICON(resolvedSize);
  }

  QByteArray buffer(iconFile.size(), Qt::Uninitialized);
  iconFile.read(buffer.data(), buffer.size());
  iconFile.close();

  QSvgRenderer renderer(buffer);
  QPixmap      pixmap(resolvedSize);
  pixmap.fill(Qt::transparent);

  if (!renderer.isValid()) {
    return PLACEHOLDER_ICON(resolvedSize);
  }

  QPainter painter(&pixmap);
  renderer.render(&painter);
  return pixmap.toImage();
}

AsyncIconImageResponse::AsyncIconImageResponse(const QString &id,
                                               const QSize   &requestedSize,
                                               QThreadPool   *pool) {
  auto runnable = new AsyncIconResponseRunnable(id, requestedSize);
  connect(runnable,
          &AsyncIconResponseRunnable::done,
          this,
          &AsyncIconImageResponse::handleDone);
  pool->start(runnable);
}

void AsyncIconImageResponse::handleDone(QImage image) {
  m_image = image;
  emit finished();
}

IconImageProvider::IconImageProvider()
    : QQuickImageProvider(
          QQuickImageProvider::Pixmap,
          QQmlImageProviderBase::ForceAsynchronousImageLoading) {}

QPixmap IconImageProvider::requestPixmap(const QString &id,
                                         QSize         *size,
                                         const QSize   &requestedSize) {
  QSize resolvedSize = requestedSize.isValid() ? requestedSize : QSize(24, 24);

  if (id.isEmpty()) return placeholderIcon(resolvedSize);

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

QPixmap IconImageProvider::handleQtIcon(const QString &name,
                                        QSize         *size,
                                        const QSize   &resolvedSize) {
  auto    queryIdx = name.indexOf("?");
  QString iconName;
  QString fallbackIcon;

  if (queryIdx != -1) {
    const auto fullQuery = name.sliced(queryIdx + 1);
    iconName             = name.sliced(0, queryIdx);

    auto args = fullQuery.split(";");
    for (auto &arg : args) {
      if (arg.startsWith("fallback=")) {
        fallbackIcon = arg.sliced(9);
      }
    }
  } else {
    iconName = name;
  }

  auto requestedIcon = QIcon::fromTheme(iconName);

  if (requestedIcon.isNull() && !fallbackIcon.isEmpty()) {
    requestedIcon = QIcon::fromTheme(fallbackIcon);
  }

  if (requestedIcon.isNull()) {
    return placeholderIcon(resolvedSize);
  }

  if (size) {
    *size = resolvedSize;
  }

  auto pixmap = requestedIcon.pixmap(resolvedSize, QIcon::Normal);
  return pixmap;
}

QPixmap IconImageProvider::handleShellIcon(const QString &name,
                                           QSize         *size,
                                           const QSize   &resolvedSize) {
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
  QPixmap      pixmap(resolvedSize);
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
} // namespace ns::iconprovider
