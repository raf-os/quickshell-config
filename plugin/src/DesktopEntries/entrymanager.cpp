#include "entrymanager.h"
#include "desktopentry.h"
#include "entrycacher.h"
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
      m_monitor(new EntryMonitor(this)),
      m_entryCacher(new EntryCacher(this)) {
  QObject::connect(m_monitor,
                   &EntryMonitor::entriesChanged,
                   this,
                   &EntryManager::scanDesktopEntries);

  auto isCacheValid = m_entryCacher->isCacheValid();
  if (isCacheValid) {
    qCDebug(logNSDesktopEntries)
        << "Desktop entry cache found: attempting to read from cache...";
    auto results = m_entryCacher->readFromCache();
    if (results.has_value()) {
      m_scanInProgress = true;
      this->processEntryList(results.value());
      qCDebug(logNSDesktopEntries)
          << "Succesfully read desktop entry list from cache!";
    } else {
      qCDebug(logNSDesktopEntries)
          << "Failed acquiring entry cache. Scanning as usual.";
    }
  } else {
    this->scanDesktopEntries();
  }
}

void EntryManager::init() {}

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

void EntryManager::processEntryList(const QList<EntryData> &results) {
  auto guard = qScopeGuard([this] {
    m_scanInProgress = false;
    if (m_scanQueued) {
      m_scanQueued = false;
      this->scanDesktopEntries();
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

void EntryManager::onScanCompleted(const QList<EntryData> &results) {
  qCDebug(logNSDesktopEntries)
      << "Desktop entry scan completed. Processing results...";
  m_entryCacher->saveToCache(results);
  this->processEntryList(results);
}
} // namespace ns::desktop::entries
