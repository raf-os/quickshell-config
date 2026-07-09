#pragma once

#include "desktopentry.h"
#include "entryaction.h"
#include "entrycacher.h"
#include "entrymonitor.h"
#include "entryscanner.h"

#include <qcontainerfwd.h>
#include <qdir.h>
#include <qhash.h>
#include <qjsengine.h>
#include <qlist.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qsqlerror.h>
#include <qtmetamacros.h>
#include <qtypes.h>

namespace ns::desktop::entries {
class EntryManager : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  static EntryManager *instance() {
    static EntryManager *s_instance = new EntryManager();
    return s_instance;
  }

  static EntryManager *create(QQmlEngine *qmlEngine,
                              QJSEngine * /* unused */) {
    auto inst = instance();
    if (qmlEngine)
      qmlEngine->setObjectOwnership(inst, QQmlEngine::CppOwnership);
    return inst;
  }

  QHash<QString,
        DesktopEntry *>
  getEntries() const;

  Q_INVOKABLE DesktopEntry *findEntryById(const QString &id);
  Q_INVOKABLE DesktopEntry *findEntry(const QString &name);
  Q_INVOKABLE void          toggleFavorite(DesktopEntry *target);

  Q_INVOKABLE void executeGeneric(const QStringList &cmd,
                                  const QString     &workingDirectory,
                                  DesktopEntry      *reference = nullptr);

public slots:
  quint32 getFrequencyForApp(const QString &id) const;
  void    incrementFrequencyFor(DesktopEntry *target);
  void    scanDesktopEntries();
  void    init();

private slots:
  void onScanCompleted(const QList<EntryData> &results);
  void processEntryList(const QList<EntryData> &results);

signals:
  void applicationsChanged();
  void applicationsFrequencyChanged();

private:
  explicit EntryManager(QObject *parent = nullptr);

  const QString                  m_uuid;
  QHash<QString, DesktopEntry *> m_desktopEntries;

  bool          m_dbSuccess      = false;
  bool          m_scanInProgress = false;
  bool          m_scanQueued     = false;
  EntryMonitor *m_monitor        = nullptr;
  EntryCacher  *m_entryCacher    = nullptr;

  QSqlError initDb();
};
} // namespace ns::desktop::entries
