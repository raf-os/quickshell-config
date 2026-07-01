#include "entrymanager.h"
#include "desktopentry.h"
#include "entrymonitor.h"
#include "entryscanner.h"

#include <qbuffer.h>
#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qdir.h>
#include <qdiriterator.h>
#include <qfileinfo.h>
#include <qhash.h>
#include <qlist.h>
#include <qloggingcategory.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpair.h>
#include <qscopeguard.h>
#include <qstringview.h>
#include <qtenvironmentvariables.h>
#include <qthreadpool.h>

namespace ns::desktop::entries {
Q_LOGGING_CATEGORY(logNSDesktopEntries,
                   "nightshell.desktop.entries")

EntryManager::EntryManager(QObject *parent)
    : QObject(parent),
      m_monitor(new EntryMonitor(this)) {
  QObject::connect(m_monitor,
                   &EntryMonitor::entriesChanged,
                   this,
                   &EntryManager::scanDesktopEntries);
}

void EntryManager::scanDesktopEntries() {
  qCDebug(logNSDesktopEntries) << "Starting desktop entry scan...";

  if (m_scanInProgress) {
    qCDebug(logNSDesktopEntries)
        << "A scan is already in progress - current request queued.";
    m_scanQueued = true;
    return;
  }

  m_scanInProgress = true;
  m_scanQueued     = false;
  auto scanner     = new DesktopEntryScanner(this);
  QObject::connect(scanner,
                   &DesktopEntryScanner::scanCompleted,
                   this,
                   &EntryManager::onScanCompleted);
  QThreadPool::globalInstance()->start(scanner);
}

void EntryManager::onScanCompleted(const QList<EntryData> &results) {
  auto guard = qScopeGuard([this] {
    m_scanInProgress = false;
    if (m_scanQueued) {
      m_scanQueued = false;
      // scan entries
    }
  });

  auto                           oldEntries = m_desktopEntries;
  QHash<QString, DesktopEntry *> newEntries;

  for (const auto &data : results) {
    if (data.hidden) {
      if (auto *target = newEntries.take(data.id))
        target->deleteLater();

      if (auto it = oldEntries.find(data.id); it != oldEntries.end()) {
        it.value()->deleteLater();
        oldEntries.erase(it);
      }
    }

    DesktopEntry *entry = nullptr;

    if (auto it = oldEntries.find(data.id); it != oldEntries.end()) {
      entry = it.value();
      oldEntries.erase(it);
      entry->updateState(data);
    } else {
      entry = new DesktopEntry(data.id, this);
      entry->updateState(data);
    }

    if (!entry->isValid()) {
      if (!oldEntries.contains(data.id)) {
        entry->deleteLater();
      }
      continue;
    }

    auto conflictingId = newEntries.contains(data.id);

    if (conflictingId) {
      if (auto target = newEntries.take(data.id))
        target->deleteLater();
    }

    newEntries.insert(data.id, entry);
  }

  m_desktopEntries = newEntries;
  emit applicationsChanged();

  for (auto *e : oldEntries)
    e->deleteLater();
}
} // namespace ns::desktop::entries
