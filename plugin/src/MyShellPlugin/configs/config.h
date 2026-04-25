#pragma once

#include <qjsengine.h>
#include <qlist.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include "generated/gen_includes.h"

namespace myqmlplugin {
namespace configs {
class Config : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

#define X(Type, Name)                                                          \
  Q_PROPERTY(Type *Name READ Name)                                             \
public:                                                                        \
  [[nodiscard]] Type *Name() const;                                            \
                                                                               \
private:                                                                       \
  Type *m_##Name = nullptr;
#include "generated/gen_types.def"
#undef X

public:
  static Config *instance() {
    static Config *s_instance = new Config();
    return s_instance;
  }

  static Config *create(QQmlEngine *, QJSEngine *) { return instance(); }

private:
  explicit Config(QObject *parent = nullptr);

  QList<QObject *> m_propertyList;
};
} // namespace configs
} // namespace myqmlplugin
