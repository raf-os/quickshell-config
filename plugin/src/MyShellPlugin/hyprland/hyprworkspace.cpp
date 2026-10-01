#include "hyprworkspace.h"

#include <algorithm>

#include <qloggingcategory.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmllist.h>
#include <qtypes.h>

#include "hyprdefs.h"
#include "hyprevents.h"
#include "hyprland.h"
#include "qlisthelpers.h"
#include "toplevelmodel.h"

namespace ns::hyprland {
Q_DECLARE_LOGGING_CATEGORY(logNSHyprland) // from hyprland.cpp

HyprWorkspace::HyprWorkspace(int id, QObject *parent)
    : QObject(parent), m_id(id) {
  QObject::connect(Hyprland::instance()->eventHandler(),
      &HyprEvents::activeWindowChanged, this,
      &HyprWorkspace::onAddressActivated);
}

void HyprWorkspace::updateData(common::HyprWorkspaceData data) {
  if (data.id != m_id) return; // received invalid data

  {
    QScopedPropertyUpdateGroup group;

    if (QString::number(m_id) == data.name) b_name = "";
    else b_name = data.name;
    b_isPersistent = data.isPersistent;
    b_isFullScreen = data.isFullScreen;
    b_monitorId    = data.monitorId;
    b_monitorName  = data.monitorName;
    m_lastAddress  = data.lastWindow;
    if (m_lastAddress > 0) {
      setActiveToplevel(m_lastAddress);
    }
  }
}

void HyprWorkspace::onAddressActivated(quint64 address) {
  setActiveToplevel(address);
}

QQmlListProperty<ToplevelInstance> HyprWorkspace::toplevels() {
  return readonlyQmlList(this, &m_childToplevels);
}

ToplevelInstance *HyprWorkspace::activeToplevel() {
  return m_activeToplevel.get();
}
void HyprWorkspace::setActiveToplevel(quint64 address) {
  auto it =
      std::ranges::find_if(m_childToplevels.begin(), m_childToplevels.end(),
          [&address](ToplevelInstance *t) { return t->address() == address; });

  if (it != m_childToplevels.end()) {
    m_lastAddress    = 0;
    m_activeToplevel = *it;
    emit activeToplevelChanged();
  }
}

void HyprWorkspace::setName(const QString &name) { b_name = name; }

void HyprWorkspace::attachToplevel(ToplevelInstance *toplevel) {
  auto idx = m_childToplevels.indexOf(toplevel);
  if (idx != -1) return;

  QObject::connect(toplevel, &ToplevelInstance::destroyed, this,
      [this, toplevel] { this->detachToplevel(toplevel); });
  QObject::connect(toplevel, &ToplevelInstance::workspaceIdChanged, this,
      [this, toplevel] { this->detachToplevel(toplevel); });

  m_childToplevels.append(toplevel);
  if (m_lastAddress > 0 && m_lastAddress == toplevel->address()) {
    setActiveToplevel(toplevel->address());
  }
  emit toplevelsChanged();
}

void HyprWorkspace::detachToplevel(ToplevelInstance *toplevel) {
  if (m_activeToplevel.get() == toplevel) {
    m_activeToplevel = nullptr;
    emit activeToplevelChanged();
  }
  auto idx = m_childToplevels.indexOf(toplevel);
  if (idx == -1) return;

  if (toplevel) QObject::disconnect(toplevel, nullptr, this, nullptr);

  if (m_childToplevels.removeOne(toplevel)) {
    emit toplevelsChanged();
  }

  if (m_activeToplevel.get() == nullptr) {
    if (m_childToplevels.size() > 0) {
      m_activeToplevel = m_childToplevels.at(0);
    }
  }
}

QBindable<bool> HyprWorkspace::bindableIsFullScreen() {
  return &b_isFullScreen;
}
} // namespace ns::hyprland
