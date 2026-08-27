#pragma once

#include <qobject.h>
#include <qpixmap.h>
#include <qqmlengine.h>
#include <qqmlextensionplugin.h>
#include <qquickimageprovider.h>
#include <qsize.h>
#include <qtmetamacros.h>

#include "iconprovider_shared_global.h"

namespace ns::iconprovider {
class NS_ICONPROVIDER_EXPORT IconImageProvider : public QQuickImageProvider {
public:
  explicit IconImageProvider();

  enum class IconType { Shell, Qt };

  QPixmap requestPixmap(const QString &id,
                        QSize         *size,
                        const QSize   &requestedSize) override;

  static QString getSystemIconRequestString(const QString &icon,
                                            const QString &path,
                                            const QString &fallback);

private:
  QPixmap
  handleQtIcon(const QString &name, QSize *size, const QSize &resolvedSize);
  QPixmap
  handleShellIcon(const QString &name, QSize *size, const QSize &resolvedSize);
  QPixmap placeholderIcon(const QSize &resolvedSize);

  const QString m_shellIconPath =
      QString::fromUtf8(ICON_DEFAULT_PATH) + "/icons";
};

class IconImageProviderExtensionPlugin : public QQmlEngineExtensionPlugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID QQmlEngineExtensionInterface_iid)

public:
  void initializeEngine(QQmlEngine *engine, const char *uri) override;
};
} // namespace ns::iconprovider
