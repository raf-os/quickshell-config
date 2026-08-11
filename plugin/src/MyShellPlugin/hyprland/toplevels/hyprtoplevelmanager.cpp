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

  QObject::connect(wayland::wlr::toplevels::ToplevelManager::instance(),
                   &wayland::wlr::toplevels::ToplevelManager::toplevelReady,
                   this,
                   &HyprlandToplevelMappingManager::onToplevelReady);

  for (auto *toplevel :
       wayland::wlr::toplevels::ToplevelManager::instance()->readyToplevels()) {
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

void HyprlandToplevelMappingManager::onToplevelReady(
    wayland::wlr::toplevels::ToplevelHandle *handle) {
  QObject::connect(handle,
                   &QObject::destroyed,
                   this,
                   &HyprlandToplevelMappingManager::onToplevelDestroyed);

  auto hyprHandle = new HyprlandToplevelMappingHandle(
      handle, this->get_window_for_toplevel_wlr(handle->object()));
}

void HyprlandToplevelMappingManager::onToplevelDestroyed(QObject *object) {
  m_addresses.remove(
      static_cast<wayland::wlr::toplevels::ToplevelHandle *>(object));
}

void HyprlandToplevelMappingManager::assignAddress(
    wayland::wlr::toplevels::ToplevelHandle *handle,
    quint64                                  address) {
  m_addresses.insert(handle, address);
  emit toplevelAddressed(handle, address);
}

bool HyprlandToplevelMappingManager::hasAddress(
    wayland::wlr::toplevels::ToplevelHandle *handle) const {
  return m_addresses.contains(handle);
}

quint64 HyprlandToplevelMappingManager::getToplevelAddress(
    wayland::wlr::toplevels::ToplevelHandle *handle) const {
  return m_addresses.value(handle);
}

wayland::wlr::toplevels::ToplevelHandle *
HyprlandToplevelMappingManager::getHandleForAddress(
    const quint64 &address) const {
  return m_addresses.key(address, nullptr);
}
} // namespace ns::hyprland::toplevels
