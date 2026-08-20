#include "dbusimage.h"

#include <QtDBus/qdbusargument.h>
#include <qimage.h>
#include <qloggingcategory.h>
#include <qmap.h>
#include <qmutex.h>
#include <qobject.h>
#include <qquickimageprovider.h>
#include <qsize.h>
#include <qtypes.h>

namespace ns::dbusprovider {
Q_LOGGING_CATEGORY(logNSDBusProvider,
                   "nightshell.dbus.imageprovider")

namespace {
QMap<QString, DBusImageHandler *> activeHandlers;
quint32                           handleIndex = 0;

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

  auto handler = activeHandlers.value(target);
  if (handler != nullptr) {
    // Handler is valid
    auto         img = handler->imageData();
    QMutexLocker locker(&img->mutex);

    auto format =
        img->hasAlpha ? QImage::Format_RGBA8888 : QImage::Format_RGB888;
    auto image = QImage(reinterpret_cast<const uchar *>(img->data.data()),
                        img->width,
                        img->height,
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

DBusImageHandler::DBusImageHandler() : m_id(QString::number(++handleIndex)) {
  activeHandlers.insert(m_id, this);
}

DBusImageHandler::~DBusImageHandler() { activeHandlers.remove(m_id); }

QString DBusImageHandler::urlFor() const {
  QString url =
      "image://dbusimg/" + m_id + "/" + QString::number(m_changeIndex);
  return url;
}

void DBusImageHandler::imageChanged() { ++m_changeIndex; }
} // namespace ns::dbusprovider
