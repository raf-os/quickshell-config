#pragma once

#include <qfilesystemwatcher.h>
#include <qobject.h>
#include <qtimer.h>
#include <qtmetamacros.h>

#include "ns_desktopentries_shared_global.h"

namespace ns::desktop::entries {
class NS_DESKTOPENTRIES_EXPORT EntryMonitor : public QObject {
  Q_OBJECT

public:
  explicit EntryMonitor(QObject *parent = nullptr);

private slots:
  void onDirectoryChanged(const QString &path);
  void processDebounced();

signals:
  void entriesChanged();

private:
  QFileSystemWatcher m_watcher;
  QTimer             m_debouncer;

  void attachWatcher();
};
} // namespace ns::desktop::entries
