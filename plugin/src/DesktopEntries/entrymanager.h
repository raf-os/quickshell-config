#pragma once

#include "desktopentry.h"
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
#include <qrunnable.h>
#include <qtmetamacros.h>

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

public slots:
  void scanDesktopEntries();

private slots:
  void onScanCompleted(const QList<EntryData> &results);

signals:
  void applicationsChanged();

private:
  explicit EntryManager(QObject *parent = nullptr);

  QHash<QString, DesktopEntry *> m_desktopEntries;

  bool          m_scanInProgress = false;
  bool          m_scanQueued     = false;
  EntryMonitor *m_monitor        = nullptr;
};
} // namespace ns::desktop::entries
