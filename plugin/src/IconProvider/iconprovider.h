#pragma once

#include <qobject.h>
#include <qpixmap.h>
#include <qqmlengine.h>
#include <qqmlextensionplugin.h>
#include <qquickimageprovider.h>
#include <qsize.h>
#include <qtmetamacros.h>
#include <qtpreprocessorsupport.h>

namespace ns {
namespace iconprovider {
class IconImageProvider : public QQuickImageProvider {
public:
  explicit IconImageProvider();

  QPixmap requestPixmap(const QString &id, QSize *size,
                        const QSize &requestedSize) override;
};

class IconImageProviderExtensionPlugin : public QQmlEngineExtensionPlugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID QQmlEngineExtensionInterface_iid)

public:
  void initializeEngine(QQmlEngine *engine, const char *uri) override;
};
} // namespace iconprovider
} // namespace ns
