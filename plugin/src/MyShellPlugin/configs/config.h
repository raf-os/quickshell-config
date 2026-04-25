#pragma once

#include <qjsengine.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace myqmlplugin {
namespace configs {
class Config : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  static Config *instance() {
    static Config *s_instance = new Config();
    return s_instance;
  }

  static Config *create(QQmlEngine *, QJSEngine *) { return instance(); }

private:
  explicit Config(QObject *parent = nullptr);
};
} // namespace configs
} // namespace myqmlplugin
