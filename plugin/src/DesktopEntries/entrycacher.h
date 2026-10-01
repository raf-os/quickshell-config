#pragma once

#include <optional>

#include <qlist.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include "entryscanner.h"
#include "ns_desktopentries_shared_global.h"

namespace ns::desktop::entries {
class NS_DESKTOPENTRIES_EXPORT EntryCacher : public QObject {
  Q_OBJECT

public:
  explicit EntryCacher(QObject *parent = nullptr);

  std::optional<QList<EntryData>> readFromCache();

public slots:
  bool isCacheValid();
  void recordDirectoryModificationDates();
  void saveToCache(const QList<EntryData> &data);

private:
  const QString m_dateCacheFilename = "DesktopEntryDirectoryCache.json";
  const QString m_caheFilename      = "DesktopEntries.json";
};
} // namespace ns::desktop::entries
