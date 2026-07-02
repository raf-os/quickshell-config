#pragma once

#include "entryscanner.h"

#include <optional>
#include <qlist.h>
#include <qobject.h>
#include <qtmetamacros.h>

namespace ns::desktop::entries {
class EntryCacher : public QObject {
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
