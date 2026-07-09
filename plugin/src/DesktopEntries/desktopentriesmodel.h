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
  Q_PROPERTY(bool hideTerminalOnly READ hideTerminalOnly WRITE
                 setHideTerminalOnly NOTIFY hideTerminalOnlyChanged)

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

  [[nodiscard]] bool hideTerminalOnly() const;
  void               setHideTerminalOnly(bool value);

  [[nodiscard]] QQmlListProperty<DesktopEntry> entryList();

  static bool sortCompare(DesktopEntry *a,
                          DesktopEntry *b);

  void sortEntries(QList<DesktopEntry *> &list);
  void applyFilters(QList<DesktopEntry *> &list);
  bool isEntryFavorite(DesktopEntry *entry);
  bool isEntryFiltered(const DesktopEntry *entry);

public slots:
  void reSortEntries();
  void resetAllFilters();

private slots:
  void onEntriesChanged();
  void onFavoriteEntriesChanged();
  void onDebounceTimeout();

signals:
  void filtersChanged();
  void entryListChanged();
  void queryStringChanged();
  void hideTerminalOnlyChanged();

private:
  EntryManager         *m_manager;
  QList<DesktopEntry *> m_entries;
  QList<DesktopEntry *> m_filteredEntries;
  QStringList           m_favoriteEntries;
  quint32               m_filters = 0;

  QString m_queryString;
  QTimer  m_debouncer;
  bool    m_hideTerminalOnly = false;

  struct {
    bool    hideTerminalOnly = false;
    QString query            = "";
  } m_previousState;

  static DesktopEntry *entryListAt(QQmlListProperty<DesktopEntry> *property,
                                   qsizetype                       index);
  static qsizetype     entryListCount(QQmlListProperty<DesktopEntry> *property);
};
} // namespace ns::desktop::entries
