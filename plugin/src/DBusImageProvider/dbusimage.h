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

namespace ProviderType {
Q_NAMESPACE

enum Enum : quint8 { Auto = 0, Async = 1 };

Q_ENUM_NS(Enum)
}; // namespace ProviderType

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

private:
  QPixmap placeholderPixmap(const QSize &resolvedSize);
  QPixmap placeholderPixmap(QSize *size, const QSize &resolvedSize);
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
  explicit BaseImageHandle(ProviderType::Enum type = ProviderType::Auto);
  virtual ~BaseImageHandle();
  Q_DISABLE_COPY_MOVE(BaseImageHandle)

  [[nodiscard]] virtual QString            urlFor() const;
  [[nodiscard]] virtual ImageHandleAdapter imageData()     = 0;
  [[nodiscard]] virtual bool               hasData() const = 0;

  virtual QPixmap
  requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
  virtual QImage
  requestImage(const QString &id, QSize *size, const QSize &requestedSize);

private:
  const ProviderType::Enum m_type;
  QString                  m_id;
};

class DBusImageHandler : public BaseImageHandle {
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
    engine->addImageProvider("dbusimg", new DBusImageProvider);
    engine->addImageProvider("dbuspix", new DBusPixmapImageProvider);
  }
};
} // namespace ns::dbusprovider
