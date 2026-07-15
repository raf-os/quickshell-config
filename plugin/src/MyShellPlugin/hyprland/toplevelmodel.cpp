#include "toplevelmodel.h"

#include <algorithm>
#include <iterator>

#include <qabstractitemmodel.h>
#include <qhash.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qstringview.h>
#include <qtypes.h>
#include <qvariant.h>

#include "hyprtoplevelmanager.h"
#include "toplevelhandle.h"
#include "toplevelmanager.h"

namespace ns::hyprland {
ToplevelInstance::ToplevelInstance(toplevels::ToplevelHandle *handle,
                                   QObject                   *parent)
    : QObject(parent),
      m_waylandHandle(handle) {
  QObject::connect(
      m_waylandHandle, &toplevels::ToplevelHandle::closed, this, [this] {
        if (m_waylandHandle) {
          QObject::disconnect(m_waylandHandle, nullptr, this, nullptr);
        }
        m_waylandHandle.clear();
      });
  QObject::connect(m_waylandHandle,
                   &toplevels::ToplevelHandle::appIdChanged,
                   this,
                   &ToplevelInstance::appIdChanged);
  QObject::connect(m_waylandHandle,
                   &toplevels::ToplevelHandle::titleChanged,
                   this,
                   &ToplevelInstance::titleChanged);

  const auto hyprMapManager =
      toplevels::HyprlandToplevelMappingManager::instance();
  if (hyprMapManager->hasAddress(m_waylandHandle)) {
    this->onHyprAddress(m_waylandHandle,
                        hyprMapManager->getToplevelAddress(m_waylandHandle));
  } else {
    QObject::connect(
        hyprMapManager,
        &toplevels::HyprlandToplevelMappingManager::toplevelAddressed,
        this,
        &ToplevelInstance::onHyprAddress);
  }
}

bool ToplevelInstance::isValid() const { return m_isValid; }

QString ToplevelInstance::appId() const {
  if (m_waylandHandle) return m_waylandHandle->appId();
  else return QString();
}
QString ToplevelInstance::title() const {
  if (m_waylandHandle) return m_waylandHandle->title();
  else return QString();
}
quint64 ToplevelInstance::address() const { return m_address; }
toplevels::ToplevelHandle *ToplevelInstance::handle() const {
  return m_waylandHandle;
}

void ToplevelInstance::activate() {
  if (!m_waylandHandle) return;
  m_waylandHandle->activate();
}

void ToplevelInstance::onHyprAddress(toplevels::ToplevelHandle *handle,
                                     quint64                    address) {
  if (address == m_address || address == 0) return;
  if (!m_waylandHandle || m_waylandHandle != handle) return;

  m_address = address;
  emit addressChanged();

  QObject::disconnect(toplevels::HyprlandToplevelMappingManager::instance(),
                      nullptr,
                      this,
                      nullptr);

  if (m_waylandHandle) {
    m_isValid = true;
    emit ready();
  }
}

ToplevelModel::ToplevelModel(QObject *parent) : QAbstractListModel(parent) {
  QObject::connect(toplevels::ToplevelManager::instance(),
                   &toplevels::ToplevelManager::destroyed,
                   this,
                   [this] { this->deleteLater(); });

  for (auto toplevel :
       toplevels::ToplevelManager::instance()->readyToplevels()) {
    this->createNewInstance(toplevel, true);
  }

  QObject::connect(toplevels::ToplevelManager::instance(),
                   &toplevels::ToplevelManager::toplevelReady,
                   this,
                   &ToplevelModel::onWaylandToplevelCreated);

  QObject::connect(toplevels::ToplevelManager::instance(),
                   &toplevels::ToplevelManager::toplevelClosed,
                   this,
                   &ToplevelModel::onWaylandToplevelDestroyed);
}

QHash<int,
      QByteArray>
ToplevelModel::roleNames() const {
  return {
      {Roles::ModelDataRole, "modelData"},
  };
}

qint32 ToplevelModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return static_cast<qint32>(m_readyToplevels.size());
}

QVariant ToplevelModel::data(const QModelIndex &index,
                             qint32             role) const {
  if (!index.isValid()) return {};

  switch (role) {
  case Roles::ModelDataRole:
    return QVariant::fromValue(m_readyToplevels.at(index.row()));
  default: return {};
  }
}

void ToplevelModel::onWaylandToplevelCreated(
    toplevels::ToplevelHandle *toplevel) {
  this->createNewInstance(toplevel);
}

void ToplevelModel::onWaylandToplevelDestroyed(
    toplevels::ToplevelHandle *toplevel) {
  auto it = std::ranges::find_if(m_readyToplevels.begin(),
                                 m_readyToplevels.end(),
                                 [toplevel](ToplevelInstance *instance) {
                                   return instance->handle() == toplevel;
                                 });

  if (it != m_readyToplevels.end()) {
    auto toplevelInstance = *it;
    auto idx              = std::distance(m_readyToplevels.begin(), it);
    this->removeAtIndex(idx);
    toplevelInstance->deleteLater();
    return;
  }
}

void ToplevelModel::insertAtIndex(ToplevelInstance *instance,
                                  int               index,
                                  bool              noModelUpdate) {
  if (noModelUpdate) {
    m_readyToplevels.append(instance);
  } else {
    beginInsertRows({}, index, index);
    m_readyToplevels.append(instance);
    endInsertRows();
  }
}
void ToplevelModel::insertAtEnd(ToplevelInstance *instance,
                                bool              noModelUpdate) {
  return this->insertAtIndex(
      instance, m_readyToplevels.size() - 1, noModelUpdate);
}

void ToplevelModel::removeAtIndex(int index) {
  beginRemoveRows({}, index, index);
  m_readyToplevels.removeAt(index);
  endRemoveRows();
}

void ToplevelModel::removeInstance(ToplevelInstance *instance) {
  auto idx = m_readyToplevels.indexOf(instance);
  if (idx != -1) {
    this->removeAtIndex(idx);
  }
  instance->deleteLater();
}

void ToplevelModel::createNewInstance(toplevels::ToplevelHandle *handle,
                                      bool noModelUpdate) {
  auto inst = new ToplevelInstance(handle, this);

  if (inst->isValid()) {
    insertAtEnd(inst, noModelUpdate);
    return;
  } else {
    QObject::connect(
        inst,
        &ToplevelInstance::ready,
        this,
        [this, handle, inst] {
          auto idx = m_pendingToplevels.indexOf(inst);
          if (idx == -1) {
            // if we're here, something unexpected happened, so aggressively
            // clean up to avoid memory leaks
            // TODO: logging
            this->removeInstance(inst);
          } else {
            m_pendingToplevels.removeAt(idx);
            this->insertAtEnd(inst);
          }
        },
        Qt::SingleShotConnection);
  }
}

void ToplevelModel::onAddressActivated(quint64 address) {
  auto it = std::ranges::find_if(m_readyToplevels.begin(),
                                 m_readyToplevels.end(),
                                 [address](ToplevelInstance *instance) {
                                   return instance->address() == address;
                                 });

  if (it != m_readyToplevels.end()) {
    auto idx = std::distance(m_readyToplevels.begin(), it);
    this->beginMoveRows({}, idx, idx, {}, 0);
    m_readyToplevels.move(idx, 0);
    this->endMoveRows();
    return;
  }
}
} // namespace ns::hyprland
