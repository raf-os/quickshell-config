#include "notificationsmodel.h"
#include "notification.h"

#include <qabstractitemmodel.h>
#include <qobject.h>
#include <qtypes.h>
#include <qvariant.h>

namespace ns::notifications {
NotificationsModel::NotificationsModel(QObject *parent)
    : QAbstractListModel(parent) {}

qint32 NotificationsModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return static_cast<qint32>(this->m_notifications.length());
}

QVariant NotificationsModel::data(const QModelIndex &index, qint32 role) const {
  if (!index.isValid())
    return {};

  switch (role) {
  case Roles::ModelDataRole:
    return QVariant::fromValue(m_notifications.at(index.row()));
  default:
    return {};
  }
}

bool NotificationsModel::removeNotification(const Notification *notif) {
  auto idx = m_notifications.indexOf(notif);
  if (idx == -1)
    return false;

  removeAt(idx);
  return true;
}

void NotificationsModel::removeAt(qsizetype index) {
  if (index < 0 || index > m_notifications.size())
    return;

  Notification *notification = m_notifications.at(index);
  auto intIdx = static_cast<qint32>(index);
  beginRemoveRows({}, intIdx, intIdx);
  m_notifications.removeAt(index);
  endRemoveRows();

  emit valuesChanged();
}

void NotificationsModel::insertNotification(Notification *notif, qsizetype at) {
  auto idx = at == -1 ? m_notifications.size() : at;

  auto intIdx = static_cast<qint32>(idx);
  beginInsertRows({}, intIdx, intIdx);
  m_notifications.insert(idx, notif);
  endInsertRows();

  emit valuesChanged();
}

void NotificationsModel::resetState() {
  beginResetModel();
  m_notifications.clear();
  endResetModel();

  emit valuesChanged();
}
} // namespace ns::notifications
