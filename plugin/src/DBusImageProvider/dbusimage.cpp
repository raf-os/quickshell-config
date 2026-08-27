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
QMap<QString, BaseAsyncImageHandle *> activeAsyncImageHandles;
QMap<QString, BaseImageHandle *>      activeImageHandles;
quint32                               asyncHandleIndex = 0;
quint32                               imageHandleIndex = 0;

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

  auto handler = activeAsyncImageHandles.value(target);
  if (handler != nullptr) {
    // Handler exists
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
  QString target;
  QString param;
  parseRequest(id, target, param);
  QSize resolvedSize = requestedSize.isValid() ? requestedSize : QSize(24, 24);

  auto handler = activeImageHandles.value(target);
  if (handler) {
    return handler->requestPixmap(param, size, requestedSize);
  } else {
    return this->placeholderPixmap(size, resolvedSize);
  }
}

QPixmap DBusPixmapImageProvider::placeholderPixmap(const QSize &resolvedSize) {
  return placeholderPixmap(nullptr, resolvedSize);
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

BaseAsyncImageHandle::BaseAsyncImageHandle()
    : m_id(QString::number(++asyncHandleIndex)) {
  activeAsyncImageHandles.insert(m_id, this);
}

BaseAsyncImageHandle::~BaseAsyncImageHandle() {
  activeAsyncImageHandles.remove(m_id);
}

BaseImageHandle::BaseImageHandle() : m_id(QString::number(++imageHandleIndex)) {
  activeImageHandles.insert(m_id, this);
}

BaseImageHandle::~BaseImageHandle() { activeImageHandles.remove(m_id); }

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

QString BaseImageHandle::urlFor() const {
  return QString("image://dbuspix/" + m_id);
}

DBusImageHandler::DBusImageHandler() : BaseAsyncImageHandle() {}

ImageHandleAdapter DBusImageHandler::imageData() {
  ImageHandleAdapter adapter;
  adapter.data   = &this->image.data;
  adapter.width  = this->image.width;
  adapter.height = this->image.height;
  adapter.mutex  = &this->image.mutex;

  return std::move(adapter);
}

QString BaseAsyncImageHandle::urlFor() const {
  return QString("image://dbusimgasync/" + m_id);
}

QString DBusImageHandler::urlFor() const {
  QString url =
      "image://dbusimgasync/" + m_id + "/" + QString::number(m_changeIndex);
  return url;
}

void DBusImageHandler::imageChanged() { ++m_changeIndex; }
} // namespace ns::dbusprovider
