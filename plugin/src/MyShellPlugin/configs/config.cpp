#include "config.h"
#include <qobject.h>

namespace myqmlplugin {
namespace configs {
Config::Config(QObject *parent) : QObject(parent) {
#define X(Type, Name)                                                          \
  m_##Name = new Type(this);                                                   \
  m_propertyList.append(m_##Name);
#include "generated/gen_types.def"
#undef X
}

#define X(Type, Name)                                                          \
  Type *Config::Name() const { return m_##Name; }
#include "generated/gen_types.def"
#undef X
} // namespace configs
} // namespace myqmlplugin
