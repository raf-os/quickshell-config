#pragma once

#include <qjsengine.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace myqmlplugin {
namespace configs {
class ConfigHandler : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  static ConfigHandler *instance() {
    static ConfigHandler *s_instance = new ConfigHandler();
    return s_instance;
  }

  static ConfigHandler *create(QQmlEngine *, QJSEngine *) { return instance(); }

private:
  explicit ConfigHandler(QObject *parent = nullptr);
};
} // namespace configs
} // namespace myqmlplugin
