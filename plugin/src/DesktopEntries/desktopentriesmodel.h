#pragma once

#include "desktopentry.h"
#include "entrymanager.h"
#include <qabstractitemmodel.h>
#include <qcontainerfwd.h>
#include <qhash.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qstringview.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qvariant.h>

namespace ns::desktop::entries {
class DesktopEntriesModel : public QAbstractListModel {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QQmlListProperty<ns::desktop::entries::DesktopEntry> entryList READ
                 entryList NOTIFY entryListChanged)
  Q_PROPERTY(QString queryString READ queryString WRITE setQueryString NOTIFY
                 queryStringChanged)
  Q_PROPERTY(bool showTerminalOnly READ showTerminalOnly WRITE
                 setShowTerminalOnly NOTIFY showTerminalOnlyChanged)

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

  [[nodiscard]] QString queryString() const;
  void                  setQueryString(const QString &value);

  [[nodiscard]] bool showTerminalOnly() const;
  void               setShowTerminalOnly(bool value);

  [[nodiscard]] QQmlListProperty<DesktopEntry> entryList();

  static bool sortCompare(DesktopEntry *a,
                          DesktopEntry *b);

  void sortEntries(QList<DesktopEntry *> &list);

public slots:
  void resetAllFilters();

private slots:
  void onEntriesChanged();
  void onFavoriteEntriesChanged();
  void onDebounceTimeout();

signals:
  void entryListChanged();
  void queryStringChanged();
  void showTerminalOnlyChanged();

private:
  EntryManager         *m_manager;
  QList<DesktopEntry *> m_entries;
  QStringList           m_favoriteEntries;

  QString m_previousQueryString;
  QString m_queryString;
  QTimer  m_debouncer;
  bool    m_showTerminalOnly = false;

  static DesktopEntry *entryListAt(QQmlListProperty<DesktopEntry> *property,
                                   qsizetype                       index);
  static qsizetype     entryListCount(QQmlListProperty<DesktopEntry> *property);
};
} // namespace ns::desktop::entries
