#pragma once

#include <QtDBus/qdbusargument.h>
#include <qimage.h>
#include <qmutex.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qqmlextensioninterface.h>
#include <qqmlextensionplugin.h>
#include <qquickimageprovider.h>
#include <qrunnable.h>
#include <qsize.h>
#include <qstringview.h>
#include <qtclasshelpermacros.h>
#include <qthreadpool.h>
#include <qtmetamacros.h>
#include <qtpreprocessorsupport.h>
#include <qtypes.h>
#include <qurl.h>

namespace ns::dbusprovider {

struct DBusNotificationImage {
  qint32 width = 0;
  qint32 height = 0;
  bool hasAlpha = false;
  QByteArray data;
  QMutex mutex;
};

const QDBusArgument &operator>>(const QDBusArgument &argument,
                                DBusNotificationImage &pixmap);
const QDBusArgument &operator<<(QDBusArgument &argument,
                                const DBusNotificationImage &pixmap);

class DBusAsyncImageRunnable : public QObject, public QRunnable {
  Q_OBJECT

signals:
  void done(QImage image);

public:
  explicit DBusAsyncImageRunnable(const QString &id, const QSize &requestedSize)
      : m_id(id), m_requestedSize(requestedSize) {}

  void run() override;

private:
  QString m_id;
  QSize m_requestedSize;
};

class DBusImageResponse : public QQuickImageResponse {
public:
  DBusImageResponse(const QString &id, const QSize &requestedSize,
                    QThreadPool *pool) {
    auto runnable = new DBusAsyncImageRunnable(id, requestedSize);
    connect(runnable, &DBusAsyncImageRunnable::done, this,
            &DBusImageResponse::handleDone);
    pool->start(runnable);
  }

  void handleDone(QImage image) {
    m_image = image;
    emit finished();
  }

  QQuickTextureFactory *textureFactory() const override {
    return QQuickTextureFactory::textureFactoryForImage(m_image);
  }

  QImage m_image;
};

class DBusImageProvider : public QQuickAsyncImageProvider {
public:
  QQuickImageResponse *
  requestImageResponse(const QString &id, const QSize &requestedSize) override;

private:
  QThreadPool pool;
};

class DBusImageHandler {
public:
  explicit DBusImageHandler();
  virtual ~DBusImageHandler();
  Q_DISABLE_COPY_MOVE(DBusImageHandler)

  DBusNotificationImage *imageData() { return &image; }

  [[nodiscard]] QString urlFor() const;
  void imageChanged();

private:
  QString m_id;
  quint32 m_changeIndex = 0;
  DBusNotificationImage image;
};

class DBusImageProviderPlugin : public QQmlEngineExtensionPlugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID QQmlEngineExtensionInterface_iid)

public:
  void initializeEngine(QQmlEngine *engine, const char *uri) override {
    Q_UNUSED(uri)
    engine->addImageProvider("dbusimg", new DBusImageProvider);
  }
};
} // namespace ns::dbusprovider
