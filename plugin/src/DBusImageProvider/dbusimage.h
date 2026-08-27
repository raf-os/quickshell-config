#pragma once

#include <QtDBus/qdbusargument.h>
#include <qimage.h>
#include <qmutex.h>
#include <qobject.h>
#include <qpixmap.h>
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
struct ImageHandleAdapter {
  bool        hasAlpha = 0;
  QByteArray *data;
  QMutex     *mutex  = nullptr;
  qint32      width  = 0;
  qint32      height = 0;
};

struct DBusNotificationImage {
  qint32     width    = 0;
  qint32     height   = 0;
  bool       hasAlpha = false;
  QByteArray data;
  QMutex     mutex;
};

const QDBusArgument &operator>>(const QDBusArgument   &argument,
                                DBusNotificationImage &pixmap);
const QDBusArgument &operator<<(QDBusArgument               &argument,
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
  QSize   m_requestedSize;
};

class DBusImageResponse : public QQuickImageResponse {
public:
  DBusImageResponse(const QString &id,
                    const QSize   &requestedSize,
                    QThreadPool   *pool) {
    auto runnable = new DBusAsyncImageRunnable(id, requestedSize);
    connect(runnable,
            &DBusAsyncImageRunnable::done,
            this,
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

class DBusPixmapImageProvider : public QQuickImageProvider {
public:
  explicit DBusPixmapImageProvider()
      : QQuickImageProvider(QQuickImageProvider::Pixmap) {}

  QPixmap
  requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);

  static QPixmap placeholderPixmap(const QSize &resolvedSize);
  static QPixmap placeholderPixmap(QSize *size, const QSize &resolvedSize);
};

class DBusImageProvider : public QQuickAsyncImageProvider {
public:
  QQuickImageResponse *
  requestImageResponse(const QString &id, const QSize &requestedSize) override;

private:
  QThreadPool pool;
};

class BaseImageHandle {
public:
  explicit BaseImageHandle();
  virtual ~BaseImageHandle();
  Q_DISABLE_COPY_MOVE(BaseImageHandle)

  virtual QPixmap
  requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
  virtual QImage
  requestImage(const QString &id, QSize *size, const QSize &requestedSize);

  [[nodiscard]] QString urlFor() const;

private:
  QString m_id;
};

class BaseAsyncImageHandle {
public:
  explicit BaseAsyncImageHandle();
  virtual ~BaseAsyncImageHandle();
  Q_DISABLE_COPY_MOVE(BaseAsyncImageHandle)

  [[nodiscard]] virtual QString            urlFor() const;
  [[nodiscard]] virtual ImageHandleAdapter imageData()     = 0;
  [[nodiscard]] virtual bool               hasData() const = 0;

private:
  QString m_id;
};

class DBusImageHandler : public BaseAsyncImageHandle {
public:
  explicit DBusImageHandler();
  Q_DISABLE_COPY_MOVE(DBusImageHandler)

  ImageHandleAdapter imageData() override;
  [[nodiscard]] bool hasData() const override {
    return !this->image.data.isEmpty();
  }
  void clear() { this->image.data.clear(); }

  [[nodiscard]] QString urlFor() const override;
  void                  imageChanged();

  [[nodiscard]] DBusNotificationImage &writeImage() {
    this->imageChanged();
    return this->image;
  }

private:
  QString               m_id;
  quint32               m_changeIndex = 0;
  DBusNotificationImage image;
};

class DBusImageProviderPlugin : public QQmlEngineExtensionPlugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID QQmlEngineExtensionInterface_iid)

public:
  void initializeEngine(QQmlEngine *engine, const char *uri) override {
    Q_UNUSED(uri)
    engine->addImageProvider("dbusimgasync", new DBusImageProvider);
    engine->addImageProvider("dbuspix", new DBusPixmapImageProvider);
  }
};
} // namespace ns::dbusprovider
