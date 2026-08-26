#include "dbusimage.h"

#include <QtDBus/qdbusargument.h>
#include <qimage.h>
#include <qloggingcategory.h>
#include <qmap.h>
#include <qmutex.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qquickimageprovider.h>
#include <qsize.h>
#include <qtypes.h>

namespace ns::dbusprovider {
Q_LOGGING_CATEGORY(logNSDBusProvider, "nightshell.dbus.imageprovider")

namespace {
QMap<QString, BaseImageHandle *> activeHandles;
quint32                          handleIndex = 0;

void parseRequest(const QString &request,
                  QString       &outTarget,
                  QString       &outParam) {
  auto idx = request.indexOf("/");
  if (idx != -1) {
    outTarget = request.sliced(0, idx);
    outParam  = request.sliced(idx + 1);
  } else {
    outTarget = request;
  }
}
} // namespace

void DBusAsyncImageRunnable::run() {
  QString target;
  QString param;
  parseRequest(m_id, target, param);

  auto handler = activeHandles.value(target);
  if (handler != nullptr) {
    // Handler is valid
    auto         img = handler->imageData();
    QMutexLocker locker(img.mutex);

    auto format =
        img.hasAlpha ? QImage::Format_RGBA8888 : QImage::Format_RGB888;
    auto image = QImage(reinterpret_cast<const uchar *>(img.data->constData()),
                        img.width,
                        img.height,
                        format);

    if (m_requestedSize.isValid()) {
      image = image.scaled(m_requestedSize);
    }

    emit done(image);
    return;
  } else {
    // Handler is invalid
    auto image = QImage();
    emit done(image);
    return;
  }
}

const QDBusArgument &operator>>(const QDBusArgument   &argument,
                                DBusNotificationImage &pixmap) {
  QMutexLocker locker(&pixmap.mutex);
  argument.beginStructure();
  argument >> pixmap.width;
  argument >> pixmap.height;
  auto rowstride = qdbus_cast<qint32>(argument);
  argument >> pixmap.hasAlpha;
  auto sampleBits = qdbus_cast<qint32>(argument);
  auto channels   = qdbus_cast<qint32>(argument);
  argument >> pixmap.data;
  argument.endStructure();

  return argument;
}

const QDBusArgument &operator<<(QDBusArgument               &argument,
                                const DBusNotificationImage &pixmap) {
  argument.beginStructure();
  argument << pixmap.width;
  argument << pixmap.height;
  argument << pixmap.width * (pixmap.hasAlpha ? 4 : 3);
  argument << pixmap.hasAlpha;
  argument << 8;
  argument << (pixmap.hasAlpha ? 4 : 3);
  argument << pixmap.data;
  argument.endStructure();
  return argument;
}

QQuickImageResponse *
DBusImageProvider::requestImageResponse(const QString &id,
                                        const QSize   &requestedSize) {
  auto response = new DBusImageResponse(id, requestedSize, &pool);
  return response;
}

QPixmap DBusPixmapImageProvider::requestPixmap(const QString &id,
                                               QSize         *size,
                                               const QSize   &requestedSize) {
  QSize resolvedSize = requestedSize.isValid() ? requestedSize : QSize(24, 24);
  QPixmap pmap;

  auto handler = activeHandles.value(id);
  if (handler) {
  } else {
    return this->placeholderPixmap(size, resolvedSize);
  }
  return pmap;
}

QPixmap DBusPixmapImageProvider::placeholderPixmap(const QSize &resolvedSize) {
  return this->placeholderPixmap(nullptr, resolvedSize);
}

QPixmap DBusPixmapImageProvider::placeholderPixmap(QSize       *size,
                                                   const QSize &resolvedSize) {
  QPixmap pmap(resolvedSize);
  pmap.fill(Qt::magenta);
  if (size) {
    *size = resolvedSize;
  }
  return pmap;
}

BaseImageHandle::BaseImageHandle(ProviderType::Enum type)
    : m_id(QString::number(++handleIndex)), m_type(type) {
  activeHandles.insert(m_id, this);
}

BaseImageHandle::~BaseImageHandle() { activeHandles.remove(m_id); }

QPixmap BaseImageHandle::requestPixmap(const QString & /*unused*/,
                                       QSize * /*unused*/,
                                       const QSize & /*unused*/) {
  qCWarning(logNSDBusProvider) << "Received request for a pixmap, but" << this
                               << "has not implemented this functionality.";
  return QPixmap();
}

QImage BaseImageHandle::requestImage(const QString & /*unused*/,
                                     QSize * /*unused*/,
                                     const QSize & /*unused*/) {
  qCWarning(logNSDBusProvider) << "Received request for an image, but" << this
                               << "has not implemented this functionality.";
  return QImage();
}

DBusImageHandler::DBusImageHandler() : BaseImageHandle(ProviderType::Async) {}

ImageHandleAdapter DBusImageHandler::imageData() {
  ImageHandleAdapter adapter;
  adapter.data   = &this->image.data;
  adapter.width  = this->image.width;
  adapter.height = this->image.height;
  adapter.mutex  = &this->image.mutex;

  return std::move(adapter);
}

QString DBusImageHandler::urlFor() const {
  QString url =
      "image://dbusimg/" + m_id + "/" + QString::number(m_changeIndex);
  return url;
}

void DBusImageHandler::imageChanged() { ++m_changeIndex; }
} // namespace ns::dbusprovider
