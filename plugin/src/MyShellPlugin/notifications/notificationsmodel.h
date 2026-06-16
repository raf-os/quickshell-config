#pragma once

#include "notification.h"
#include <qabstractitemmodel.h>
#include <qhash.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qvariant.h>

namespace ns::notifications {
class NotificationsModel : public QAbstractListModel {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QList<ns::notifications::Notification *> values READ values NOTIFY
                 valuesChanged)

public:
  explicit NotificationsModel(QObject *parent = nullptr);

  enum Roles {
    ModelDataRole = Qt::UserRole + 1,
  };

  qint32 rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, qint32 role) const override;
  QHash<int, QByteArray> roleNames() const override {
    return {{Roles::ModelDataRole, "modelData"}};
  }

  void removeAt(qsizetype index);
  bool removeNotification(const Notification *notif);
  void insertNotification(Notification *notif, qsizetype at = -1);

  void resetState();

  [[nodiscard]] QList<Notification *> values() const {
    return m_notifications;
  };

signals:
  void valuesChanged();

private:
  QList<Notification *> m_notifications;
};
} // namespace ns::notifications
