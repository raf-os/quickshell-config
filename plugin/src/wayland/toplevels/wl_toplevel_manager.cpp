#include "wl_toplevel_manager.h"

#include <qjsengine.h>
#include <qlist.h>
#include <qlogging.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qwaylandclientextension.h>

#include "wayland-ext-foreign-toplevel-list-v1-client-protocol.h"
#include "wl_toplevel_handle.h"

namespace ns::wayland::toplevels {
WLToplevelManager::WLToplevelManager() : QWaylandClientExtensionTemplate(1) {
  this->initialize();
}

WLToplevelManager *WLToplevelManager::instance() {
  static auto s_instance = new WLToplevelManager();
  return s_instance;
}

WLToplevelManager *WLToplevelManager::create(QQmlEngine *qmlEngine,
                                             QJSEngine * /*unused*/) {
  auto inst = instance();
  if (qmlEngine) qmlEngine->setObjectOwnership(inst, QQmlEngine::CppOwnership);
  return inst;
}

bool WLToplevelManager::available() { return this->isActive(); }

QList<WLToplevelHandle *> WLToplevelManager::toplevels() {
  return m_readyToplevels;
}

QList<WLToplevelHandle *> WLToplevelManager::allToplevels() {
  return m_toplevels;
}

void WLToplevelManager::ext_foreign_toplevel_list_v1_toplevel(
    ::ext_foreign_toplevel_handle_v1 *toplevel) {
  auto *handle = new WLToplevelHandle();

  QObject::connect(handle,
                   &WLToplevelHandle::closed,
                   this,
                   &WLToplevelManager::onToplevelClosed);
  QObject::connect(handle,
                   &WLToplevelHandle::ready,
                   this,
                   &WLToplevelManager::onToplevelReady);

  m_toplevels.append(handle);
  handle->init(toplevel);
}

void WLToplevelManager::onToplevelClosed() {
  auto *handle = qobject_cast<WLToplevelHandle *>(this->sender());
  m_toplevels.removeOne(handle);
  m_readyToplevels.removeOne(handle);
  emit toplevelsChanged();
  emit toplevelClosed(handle);
}

void WLToplevelManager::onToplevelReady() {
  auto *handle = qobject_cast<WLToplevelHandle *>(this->sender());
  m_readyToplevels.append(handle);
  emit toplevelsChanged();
  emit toplevelReady(handle);
}
} // namespace ns::wayland::toplevels
