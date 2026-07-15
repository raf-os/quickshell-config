#pragma once

#include <qimage.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qqmlengine.h>
#include <qqmlextensionplugin.h>
#include <qquickimageprovider.h>
#include <qrunnable.h>
#include <qsize.h>
#include <qthreadpool.h>
#include <qtmetamacros.h>
#include <qtpreprocessorsupport.h>

namespace ns::iconprovider {
class AsyncIconResponseRunnable : public QObject, public QRunnable {
  Q_OBJECT

public:
  AsyncIconResponseRunnable(const QString &id,
                            const QSize   &requestedSize);

  void run() override;

signals:
  void done(QImage image);

private:
  QString m_id;
  QSize   m_requestedSize;

  QImage handleQtIcon(const QString &id,
                      const QSize   &resolvedSize);
  QImage handleShellIcon(const QString &id,
                         const QSize   &resolvedSize);
};

class AsyncIconImageResponse : public QQuickImageResponse {
public:
  AsyncIconImageResponse(const QString &id,
                         const QSize   &requestedSize,
                         QThreadPool   *pool);
  void                  handleDone(QImage image);
  QQuickTextureFactory *textureFactory() const override {
    return QQuickTextureFactory::textureFactoryForImage(m_image);
  }

private:
  QImage m_image;
};

class AsyncIconImageProvider : public QQuickAsyncImageProvider {
public:
  QQuickImageResponse *
  requestImageResponse(const QString &id,
                       const QSize   &requestedSize) override {
    auto *response = new AsyncIconImageResponse(id, requestedSize, &pool);
    return response;
  }

private:
  QThreadPool pool;
};

class IconImageProvider : public QQuickImageProvider {
public:
  explicit IconImageProvider();

  enum class IconType { Shell, Qt };

  QPixmap requestPixmap(const QString &id,
                        QSize         *size,
                        const QSize   &requestedSize) override;

private:
  QPixmap handleQtIcon(const QString &name,
                       QSize         *size,
                       const QSize   &resolvedSize);
  QPixmap handleShellIcon(const QString &name,
                          QSize         *size,
                          const QSize   &resolvedSize);
  QPixmap placeholderIcon(const QSize &resolvedSize);

  const QString m_shellIconPath =
      QString::fromUtf8(ICON_DEFAULT_PATH) + "/icons";
};

class IconImageProviderExtensionPlugin : public QQmlEngineExtensionPlugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID QQmlEngineExtensionInterface_iid)

public:
  void initializeEngine(QQmlEngine *engine,
                        const char *uri) override;
};
} // namespace ns::iconprovider
