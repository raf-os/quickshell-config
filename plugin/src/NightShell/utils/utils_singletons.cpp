#include "utils_singletons.h"

#include <qjsengine.h>
#include <qobject.h>
#include <qqmlengine.h>

#include "keyboard_layout_serializer.h"

namespace ns::utils {
UtilsSingleton::UtilsSingleton(QObject *parent) : QObject(parent) {
  m_kbdLayoutSerializer = new KeyboardLayoutXMLSerializer(nullptr, this);
}

UtilsSingleton::~UtilsSingleton() = default;

UtilsSingleton *UtilsSingleton::instance() {
  static UtilsSingleton *s_instance = nullptr;
  if (!s_instance) s_instance = new UtilsSingleton();
  return s_instance;
}

UtilsSingleton *UtilsSingleton::create(
    QQmlEngine *engine, QJSEngine * /*unused*/) {
  auto inst = instance();
  if (engine) {
    engine->setObjectOwnership(inst, QQmlEngine::CppOwnership);
  }
  return inst;
}

KeyboardLayoutXMLSerializer *UtilsSingleton::keyboardLayoutSerializer() {
  auto inst = instance();
  return instance()->m_kbdLayoutSerializer;
}
} // namespace ns::utils
