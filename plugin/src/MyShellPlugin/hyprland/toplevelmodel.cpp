#include "toplevelmodel.h"

#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>

#include <qhash.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonparseerror.h>
#include <qlist.h>
#include <qlogging.h>
#include <qloggingcategory.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqmllist.h>
#include <qstringview.h>
#include <qtypes.h>
#include <qvariant.h>

#include "hyprtoplevelmanager.h"
#include "qlisthelpers.h"
#include "rapidfuzz/fuzz.hpp"
#include "toplevelhandle.h"
#include "toplevelmanager.h"

namespace ns::hyprland {
Q_DECLARE_LOGGING_CATEGORY(logNSHyprland) // from hyprland.cpp

ToplevelInstance::ToplevelInstance(toplevels::ToplevelHandle *handle,
                                   QObject                   *parent)
    : QObject(parent),
      m_waylandHandle(handle) {
  this->setupToplevelConnections();

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

ToplevelInstance::ToplevelInstance(quint64  address,
                                   QObject *parent)
    : QObject(parent),
      m_address(address) {}

void ToplevelInstance::setupToplevelConnections() {
  if (!m_waylandHandle) return;

  QObject::connect(
      m_waylandHandle, &toplevels::ToplevelHandle::closed, this, [this] {
        if (m_waylandHandle) {
          QObject::disconnect(m_waylandHandle, nullptr, this, nullptr);
        }
      });
  QObject::connect(m_waylandHandle,
                   &toplevels::ToplevelHandle::appIdChanged,
                   this,
                   &ToplevelInstance::appIdChanged);
  QObject::connect(m_waylandHandle,
                   &toplevels::ToplevelHandle::titleChanged,
                   this,
                   &ToplevelInstance::titleChanged);
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
void ToplevelInstance::setAddress(const quint64 &address) {
  if (m_address == address) return;

  QObject::disconnect(toplevels::HyprlandToplevelMappingManager::instance(),
                      nullptr,
                      this,
                      nullptr);

  m_address = address;
  emit addressChanged();

  if (m_waylandHandle) {
    m_isValid = true;
    emit ready();
  }
}
int  ToplevelInstance::workspaceId() const { return m_workspaceId; }
void ToplevelInstance::setWorkspaceId(int value) {
  if (m_workspaceId == value) return;

  m_workspaceId = value;
  emit workspaceIdChanged();
}

void ToplevelInstance::activate() {
  if (!m_waylandHandle) return;
  m_waylandHandle->activate();
}

void ToplevelInstance::onHyprAddress(toplevels::ToplevelHandle *handle,
                                     quint64                    address) {
  if (address == m_address || address == 0) return;
  if (!m_waylandHandle || m_waylandHandle != handle) return;

  QObject::disconnect(toplevels::HyprlandToplevelMappingManager::instance(),
                      nullptr,
                      this,
                      nullptr);

  m_address = address;
  emit addressChanged();
  m_isValid = true;

  emit ready();
}

void ToplevelInstance::onToplevelMap(toplevels::ToplevelHandle *handle) {
  if (m_address == 0 || m_waylandHandle) return;

  QObject::disconnect(toplevels::HyprlandToplevelMappingManager::instance(),
                      nullptr,
                      this,
                      nullptr);

  m_waylandHandle = handle;

  this->setupToplevelConnections();

  m_isValid = true;
  emit ready();
}

ToplevelModel::ToplevelModel(QObject *parent) : QObject(parent) {
  QObject::connect(toplevels::ToplevelManager::instance(),
                   &toplevels::ToplevelManager::destroyed,
                   this,
                   [this] { this->deleteLater(); });

  for (auto toplevel :
       toplevels::ToplevelManager::instance()->readyToplevels()) {
    this->createNewInstance(toplevel);
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

QQmlListProperty<ToplevelInstance> ToplevelModel::items() {
  return readonlyQmlList<ToplevelInstance>(this, &m_filteredToplevels);
}
QList<ToplevelInstance *> ToplevelModel::toplevelList() const {
  return m_readyToplevels;
}

QString ToplevelModel::searchQuery() const { return m_searchQuery; }
void    ToplevelModel::setSearchQuery(const QString &value) {
  if (value == m_searchQuery) return;

  const auto lval = value.toLower();
  m_searchQuery   = lval;
  emit searchQueryChanged();

  auto filtered = m_readyToplevels;
  this->applyFilters(&filtered);

  if (filtered != m_filteredToplevels) {
    m_filteredToplevels = filtered;
    emit itemsChanged();
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
    this->removeInstance(toplevelInstance);
    m_allTopLevels.removeOne(toplevelInstance);
    toplevelInstance->deleteLater();
    return;
  }
}

void ToplevelModel::insertAtEnd(ToplevelInstance *instance) {
  m_readyToplevels.append(instance);
  emit readyToplevelsChanged(m_readyToplevels);

  auto filtered = m_readyToplevels;
  this->applyFilters(&filtered);

  if (filtered != m_filteredToplevels) {
    m_filteredToplevels = filtered;
    emit itemsChanged();
  }
}

void ToplevelModel::removeAtIndex(int index) {
  m_readyToplevels.removeAt(index);
  emit readyToplevelsChanged(m_readyToplevels);
}

void ToplevelModel::removeInstance(ToplevelInstance *instance) {
  m_allTopLevels.removeOne(instance);
  auto idx = m_readyToplevels.indexOf(instance);
  if (idx != -1) {
    this->removeAtIndex(idx);
  }
  if (m_filteredToplevels.removeOne(instance)) {
    auto filtered = m_readyToplevels;
    this->applyFilters(&filtered);

    if (filtered != m_filteredToplevels) {
      m_filteredToplevels = filtered;
      emit itemsChanged();
    }
  }
  instance->deleteLater();
}

/* This will create memory leaks as of now
 */
ToplevelInstance *ToplevelModel::createNewInstance(const quint64 &address) {
  auto inst = new ToplevelInstance(address, this);

  if (inst->isValid()) {
    insertAtEnd(inst);
  } else {
    m_allTopLevels.append(inst);
    QObject::connect(
        inst,
        &ToplevelInstance::ready,
        this,
        [this, inst] {
          auto idx = m_allTopLevels.indexOf(inst);
          if (idx == -1) {
            this->removeInstance(inst);
          } else {
            this->insertAtEnd(inst);
          }
        },
        Qt::SingleShotConnection);
  }

  return inst;
}

ToplevelInstance *
ToplevelModel::createNewInstance(toplevels::ToplevelHandle *handle) {
  auto inst = new ToplevelInstance(handle, this);

  if (inst->isValid()) {
    insertAtEnd(inst);
  } else {
    m_allTopLevels.append(inst);
    QObject::connect(
        inst,
        &ToplevelInstance::ready,
        this,
        [this, inst] {
          auto idx = m_allTopLevels.indexOf(inst);
          if (idx == -1) {
            this->removeInstance(inst);
          } else {
            this->insertAtEnd(inst);
          }
        },
        Qt::SingleShotConnection);
  }

  return inst;
}

void ToplevelModel::onAddressActivated(quint64 address) {
  auto it = std::ranges::find_if(m_readyToplevels.begin(),
                                 m_readyToplevels.end(),
                                 [address](ToplevelInstance *instance) {
                                   return instance->address() == address;
                                 });

  if (it != m_readyToplevels.end()) {
    auto idx = std::distance(m_readyToplevels.begin(), it);
    if (idx == 0 || idx > m_readyToplevels.size()) return;
    m_readyToplevels.move(idx, 0);
    auto fit = std::ranges::find_if(m_filteredToplevels.begin(),
                                    m_filteredToplevels.end(),
                                    [address](ToplevelInstance *inst) {
                                      return inst->address() == address;
                                    });
    if (fit != m_filteredToplevels.end()) {
      auto fidx = std::distance(m_filteredToplevels.begin(), fit);
      if (fidx != 0) {
        m_filteredToplevels.move(fidx, 0);
      }
    }
  }
}

void ToplevelModel::handleHyprClientsPayload(const QByteArray &data) {
  if (data.isEmpty()) return;

  QJsonParseError parseError;
  auto            jDoc = QJsonDocument::fromJson(data, &parseError);

  if (parseError.error != QJsonParseError::NoError) {
    qCWarning(logNSHyprland)
        << "Error parsing 'clients' payload from hyprland as a json document.";
    return;
  }

  const auto &tlist = m_allTopLevels;

  auto jArray = jDoc.array();
  for (auto entry : jArray) {
    auto jObj = entry.toObject();

    bool isOk    = false;
    auto address = jObj.value("address").toString().toULongLong(&isOk, 16);
    if (!isOk) {
      qCWarning(logNSHyprland)
          << "Invalid address received from hyprland's j/clients!";
      continue;
    }

    auto it = std::ranges::find_if(
        tlist.begin(), tlist.end(), [address](ToplevelInstance *inst) {
          return inst->address() == address;
        });

    ToplevelInstance *toplevel = it == tlist.end() ? nullptr : *it;
    bool              exists   = toplevel != nullptr;

    // if (!exists) toplevel = createNewInstance(address);

    if (!exists) {
      auto addr = toplevels::HyprlandToplevelMappingManager::instance()
                      ->getHandleForAddress(address);

      if (addr == nullptr) {
        qCWarning(logNSHyprland) << "Could not match address" << address
                                 << "to any wayland toplevels.";
        continue;
      }

      auto iit = std::ranges::find_if(
          tlist.begin(), tlist.end(), [addr](ToplevelInstance *inst) {
            return inst->handle() == addr;
          });

      if (iit == tlist.end()) continue;

      toplevel = *iit;
      toplevel->setAddress(address);
    }

    auto workspaceObj = jObj.value("workspace").toObject();
    if (!workspaceObj.isEmpty()) {
      toplevel->setWorkspaceId(workspaceObj.value("id").toInt(0));
    }
  }

  emit readyToplevelsChanged(m_readyToplevels);
}

void ToplevelModel::onWindowMoveWorkspace(const quint64 &address,
                                          int            workspaceId) {
  auto it = std::ranges::find_if(
      m_readyToplevels.begin(),
      m_readyToplevels.end(),
      [address](ToplevelInstance *inst) { return inst->address() == address; });

  if (it != m_readyToplevels.end()) {
    (*it)->setWorkspaceId(workspaceId);
    emit windowMoved(*it);
  }
}

void ToplevelModel::applyFilters(QList<ToplevelInstance *> *target) {
  if (m_searchQuery.isEmpty()) {
    return;
  }

  const auto                                qstr = m_searchQuery.toStdString();
  rapidfuzz::fuzz::CachedPartialRatio<char> scorer(qstr);

  target->removeIf([qstr, scorer](ToplevelInstance *item) {
    const auto name = item->title().toLower().toStdString();

    if (name.contains(qstr)) return false;

    auto score = scorer.similarity(std::string_view(name), 80.0);

    if (score > 80.0) {
      return false;
    }

    return true;
  });
}
} // namespace ns::hyprland
