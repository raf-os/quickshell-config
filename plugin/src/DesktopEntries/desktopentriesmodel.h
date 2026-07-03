#pragma once

#include "desktopentry.h"
#include <qabstractitemmodel.h>
#include <qcontainerfwd.h>
#include <qhash.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qvariant.h>

namespace ns::desktop::entries {
class DesktopEntriesModel : public QAbstractListModel {
  Q_OBJECT
  QML_ELEMENT

public:
  explicit DesktopEntriesModel(QObject *parent = nullptr);

  enum Roles {
    ModelDataRole = Qt::UserRole + 1,
  };
  QHash<int,
        QByteArray>
  roleNames() const override {
    return {
        {Roles::ModelDataRole, "modelData"}
    };
  }

  qint32   rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index,
                qint32             role) const override;

private slots:
  void onEntriesChanged();

private:
  QList<DesktopEntry *> m_entries;
  QStringList           m_favoriteEntries;
};
} // namespace ns::desktop::entries
