#include "hyprtoplevelmanager.h"

#include <qjsengine.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qtypes.h>
#include <qwaylandclientextension.h>

#include "hyprtoplevelhandle.h"
#include "toplevelhandle.h"
#include "toplevelmanager.h"

namespace ns::hyprland::toplevels {
HyprlandToplevelMappingManager::HyprlandToplevelMappingManager()
    : QWaylandClientExtensionTemplate(1) {
  this->initialize();

  if (!this->isInitialized()) {
    return;
  }

  QObject::connect(ToplevelManager::instance(),
                   &ToplevelManager::toplevelReady,
                   this,
                   &HyprlandToplevelMappingManager::onToplevelReady);

  for (auto *toplevel : ToplevelManager::instance()->readyToplevels()) {
    this->onToplevelReady(toplevel);
  }
}

HyprlandToplevelMappingManager *HyprlandToplevelMappingManager::instance() {
  static auto *s_instance = new HyprlandToplevelMappingManager();
  return s_instance;
}

HyprlandToplevelMappingManager *
HyprlandToplevelMappingManager::create(QQmlEngine *qmlEngine,
                                       QJSEngine * /* unused */) {
  auto inst = instance();
  if (qmlEngine) qmlEngine->setObjectOwnership(inst, QQmlEngine::CppOwnership);
  return inst;
}

void HyprlandToplevelMappingManager::onToplevelReady(ToplevelHandle *handle) {
  QObject::connect(handle,
                   &QObject::destroyed,
                   this,
                   &HyprlandToplevelMappingManager::onToplevelDestroyed);

  auto hyprHandle = new HyprlandToplevelMappingHandle(
      handle, this->get_window_for_toplevel_wlr(handle->object()));
}

void HyprlandToplevelMappingManager::onToplevelDestroyed(QObject *object) {
  m_addresses.remove(static_cast<ToplevelHandle *>(object));
}

void HyprlandToplevelMappingManager::assignAddress(ToplevelHandle *handle,
                                                   quint64         address) {
  m_addresses.insert(handle, address);
  emit toplevelAddressed(handle, address);
}

bool HyprlandToplevelMappingManager::hasAddress(ToplevelHandle *handle) const {
  return m_addresses.contains(handle);
}

quint64 HyprlandToplevelMappingManager::getToplevelAddress(
    ToplevelHandle *handle) const {
  return m_addresses.value(handle);
}
} // namespace ns::hyprland::toplevels
