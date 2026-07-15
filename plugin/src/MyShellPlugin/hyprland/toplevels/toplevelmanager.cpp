#include "toplevelmanager.h"

#include <qjsengine.h>
#include <qobject.h>
#include <qqmlengine.h>

#include "toplevelhandle.h"
#include "wayland-wlr-foreign-toplevel-management-unstable-v1-client-protocol.h"

namespace ns::hyprland::toplevels {
ToplevelManager::ToplevelManager() : QWaylandClientExtensionTemplate(3) {
  this->initialize();
}

bool ToplevelManager::available() const { return this->isActive(); }

ToplevelManager *ToplevelManager::instance() {
  static auto s_instance = new ToplevelManager();
  return s_instance;
}

ToplevelManager *ToplevelManager::create(QQmlEngine *qmlEngine,
                                         QJSEngine * /* unused*/) {
  auto inst = instance();
  if (qmlEngine) qmlEngine->setObjectOwnership(inst, QQmlEngine::CppOwnership);
  return inst;
}

void ToplevelManager::zwlr_foreign_toplevel_manager_v1_toplevel(
    ::zwlr_foreign_toplevel_handle_v1 *toplevel) {
  auto *handle = new ToplevelHandle();
  QObject::connect(handle,
                   &ToplevelHandle::closed,
                   this,
                   &ToplevelManager::onToplevelClosed);
  QObject::connect(
      handle, &ToplevelHandle::ready, this, &ToplevelManager::onToplevelReady);

  m_toplevels.append(handle);
  handle->init(toplevel);
}

ToplevelHandle *
ToplevelManager::handleFor(::zwlr_foreign_toplevel_handle_v1 *toplevel) {
  if (toplevel == nullptr) return nullptr;

  for (auto *other : m_toplevels) {
    if (other->object() == toplevel) return other;
  }

  return nullptr;
}

void ToplevelManager::onToplevelReady() {
  auto *handle = qobject_cast<ToplevelHandle *>(this->sender());
  m_readyToplevels.append(handle);
  emit toplevelReady(handle);
}

void ToplevelManager::onToplevelClosed() {
  auto *handle = qobject_cast<ToplevelHandle *>(this->sender());
  m_readyToplevels.removeOne(handle);
  m_toplevels.removeOne(handle);
  emit toplevelClosed(handle);
}

QList<ToplevelHandle *> ToplevelManager::toplevels() { return m_toplevels; }
QList<ToplevelHandle *> ToplevelManager::readyToplevels() {
  return m_readyToplevels;
}
} // namespace ns::hyprland::toplevels
