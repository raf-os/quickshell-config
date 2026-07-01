#include "entrymonitor.h"
#include "entryutils.h"
#include <qfilesystemwatcher.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qtimer.h>

namespace ns::desktop::entries {
Q_DECLARE_LOGGING_CATEGORY(logNSDesktopEntries) // from entrymanager.cpp

EntryMonitor::EntryMonitor(QObject *parent) : QObject(parent) {
  QObject::connect(&m_watcher,
                   &QFileSystemWatcher::directoryChanged,
                   this,
                   &EntryMonitor::onDirectoryChanged);
  QObject::connect(
      &m_debouncer, &QTimer::timeout, this, &EntryMonitor::processDebounced);

  m_debouncer.setInterval(1000);
  m_debouncer.setSingleShot(true);

  this->attachWatcher();
}

void EntryMonitor::processDebounced() {
  emit this->entriesChanged();
  this->attachWatcher();
}

void EntryMonitor::onDirectoryChanged(const QString &path) {
  m_debouncer.start();
}

void EntryMonitor::attachWatcher() {
  const auto pathList = EntryUtils::desktopPaths();
  if (pathList == m_watcher.directories())
    return;
  const auto failedPaths = m_watcher.addPaths(pathList);
  if (!failedPaths.isEmpty()) {
    qCWarning(logNSDesktopEntries)
        << "EntryMonitor: failed watching directories:" << failedPaths;
  }
}
} // namespace ns::desktop::entries
