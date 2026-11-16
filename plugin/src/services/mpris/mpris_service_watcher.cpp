#include "mpris_service_watcher.h"

#include <qabstractitemmodel.h>
#include <qcontainerfwd.h>
#include <qdbusconnection.h>
#include <qdbusconnectioninterface.h>
#include <qdbusservicewatcher.h>
#include <qhash.h>
#include <qhashfunctions.h>
#include <qloggingcategory.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqmllist.h>
#include <qtypes.h>
#include <qvariant.h>

#include "mpris_player.h"
#include "ns_mpris_debug.h"

namespace ns::mpris {
MprisPlayerListModel::MprisPlayerListModel(QObject *parent)
    : QAbstractListModel(parent) {}

QQmlListProperty<MprisPlayer> MprisPlayerListModel::values() {
  return QQmlListProperty<MprisPlayer>(this, &m_players,
      &MprisPlayerListModel::qListCount, &MprisPlayerListModel::qListAt);
}

qsizetype MprisPlayerListModel::qListCount(
    QQmlListProperty<MprisPlayer> *property) {
  return static_cast<QList<MprisPlayer *> *>(property->data)->size();
}

MprisPlayer *MprisPlayerListModel::qListAt(
    QQmlListProperty<MprisPlayer> *property, qsizetype index) {
  return static_cast<QList<MprisPlayer *> *>(property->data)->at(index);
}

qsizetype MprisPlayerListModel::indexOf(MprisPlayer *player) const {
  return m_players.indexOf(player);
}

void MprisPlayerListModel::insert(MprisPlayer *player) {
  if (!m_players.contains(player)) {
    beginInsertRows({}, m_players.size(), m_players.size());
    m_players.append(player);
    endInsertRows();
    emit valuesChanged();
  }
}

bool MprisPlayerListModel::removeOne(MprisPlayer *player) {
  auto idx = m_players.indexOf(player);
  if (idx != -1) {
    beginRemoveRows({}, idx, idx);
    m_players.removeAt(idx);
    endRemoveRows();
    emit valuesChanged();
  }
  return (idx != -1);
}

QHash<int, QByteArray> MprisPlayerListModel::roleNames() const {
  return {
      {Roles::ModelData, "modelData"},
  };
}

int MprisPlayerListModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return static_cast<int>(m_players.size());
}

QVariant MprisPlayerListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) return {};

  auto player = m_players.at(index.row());

  switch (role) {
  case ModelData: return QVariant::fromValue(player);
  }

  return {};
}

// MprisServiceWatcher
MprisServiceWatcher::MprisServiceWatcher(QObject *parent)
    : QObject(parent), m_readyPlayersModel(new MprisPlayerListModel(this)) {
  auto bus = QDBusConnection::sessionBus();

  if (!bus.isConnected()) {
    qCWarning(NS_LOG_MPRIS)
        << "Unable to connect to DBus. Mpris service is unavailable.";
    return;
  }

  QObject::connect(&m_watcher, &QDBusServiceWatcher::serviceRegistered, this,
      &MprisServiceWatcher::onServiceRegistered);
  QObject::connect(&m_watcher, &QDBusServiceWatcher::serviceUnregistered, this,
      &MprisServiceWatcher::onServiceUnregistered);

  m_watcher.setWatchMode(QDBusServiceWatcher::WatchForUnregistration |
                         QDBusServiceWatcher::WatchForRegistration);

  m_watcher.addWatchedService("org.mpris.MediaPlayer2");
  m_watcher.setConnection(bus);

  const QStringList &existingList = bus.interface()->registeredServiceNames();
  for (const QString &service : existingList) {
    if (service.startsWith("org.mpris.MediaPlayer2")) {
      qCDebug(NS_LOG_MPRIS) << "Registering existing service:" << service;
      registerPlayer(service);
    }
  }

  m_isValid = true;
}

bool MprisServiceWatcher::isValid() const { return m_isValid; }

MprisPlayerListModel *MprisServiceWatcher::model() {
  return m_readyPlayersModel;
}

void MprisServiceWatcher::registerPlayer(const QString &address) {
  if (m_players.contains(address)) {
    qCDebug(NS_LOG_MPRIS) << "Skipping duplicate mpris player:" << address;
    return;
  }

  auto *player = new MprisPlayer(address, this);
  if (!player->isValid()) {
    qCWarning(NS_LOG_MPRIS)
        << "Unable to register invalid mpris player:" << address;
    delete player;
    return;
  }

  m_players.insert(address, player);

  QObject::connect(player, &QObject::destroyed, this,
      &MprisServiceWatcher::onPlayerDestroyed);
  QObject::connect(player, &MprisPlayer::ready, this,
      &MprisServiceWatcher::onPlayerReady, Qt::SingleShotConnection);
}

void MprisServiceWatcher::onPlayerDestroyed() {
  auto *player = static_cast<MprisPlayer *>(sender());
  m_readyPlayersModel->removeOne(player);
}

void MprisServiceWatcher::onPlayerReady() {
  auto *player = static_cast<MprisPlayer *>(sender());
  m_readyPlayersModel->insert(player);
}

void MprisServiceWatcher::onServiceRegistered(const QString &service) {
  if (service.startsWith("org.mpris.MediaPlayer2")) {
    registerPlayer(service);
  }
}

void MprisServiceWatcher::onServiceUnregistered(const QString &service) {
  auto it = m_players.find(service);
  if (it != m_players.end()) {
    auto player = it.value();
    QObject::disconnect(player, nullptr, this, nullptr);
    m_players.erase(it);
    player->deleteLater();
  }
}
} // namespace ns::mpris
