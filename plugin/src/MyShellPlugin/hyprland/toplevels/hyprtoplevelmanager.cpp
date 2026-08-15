#include "hyprtoplevelmanager.h"

#include <qjsengine.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qtypes.h>
#include <qwaylandclientextension.h>

#include "hyprtoplevelhandle.h"
#include "toplevelhandle.h"
#include "toplevelmanager.h"
#include "wl_toplevel_handle.h"
#include "wl_toplevel_manager.h"

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

  QObject::connect(wayland::toplevels::WLToplevelManager::instance(),
                   &wayland::toplevels::WLToplevelManager::toplevelReady,
                   this,
                   &HyprlandToplevelMappingManager::onWlExtToplevelReady);

  for (auto *toplevel :
       wayland::wlr::toplevels::ToplevelManager::instance()->readyToplevels()) {
    this->onToplevelReady(toplevel);
  }

  for (auto *wlToplevel :
       wayland::toplevels::WLToplevelManager::instance()->toplevels()) {
    this->onWlExtToplevelReady(wlToplevel);
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
      [this](void *handle, quint64 address) {
        auto h = static_cast<wayland::wlr::toplevels::ToplevelHandle *>(handle);
        this->assignAddressWlr(h, address);
      },
      handle,
      this->get_window_for_toplevel_wlr(handle->object()));
}

void HyprlandToplevelMappingManager::onWlExtToplevelReady(
    wayland::toplevels::WLToplevelHandle *handle) {
  QObject::connect(handle, &QObject::destroyed, this, [this, handle] {
    auto existing = m_pendingMappings.key(handle, 0);
    if (existing != 0) {
      m_pendingMappings.remove(existing);
    }
  });

  auto hyprHandle = new HyprlandToplevelMappingHandle(
      [this](void *handle, quint64 address) {
        auto h = static_cast<wayland::toplevels::WLToplevelHandle *>(handle);
        this->assignAddressWlExt(h, address);
      },
      handle,
      this->get_window_for_toplevel(handle->object()));
}

void HyprlandToplevelMappingManager::onToplevelDestroyed(QObject *object) {
  m_addresses.remove(
      static_cast<wayland::wlr::toplevels::ToplevelHandle *>(object));
}

void HyprlandToplevelMappingManager::assignAddressWlr(
    wayland::wlr::toplevels::ToplevelHandle *handle,
    quint64                                  address) {
  if (m_pendingMappings.contains(address)) {
    auto p = m_pendingMappings.take(address);
    handle->mapWaylandExtHandle(p);
  }
  m_addresses.insert(handle, address);
  emit toplevelAddressed(handle, address);
}

void HyprlandToplevelMappingManager::assignAddressWlExt(
    wayland::toplevels::WLToplevelHandle *handle,
    quint64                               address) {
  auto existingKey = m_addresses.key(address, nullptr);
  if (existingKey) {
    existingKey->mapWaylandExtHandle(handle);
  } else {
    m_pendingMappings.insert(address, handle);
  }
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
