#include "entryscanner.h"
#include "entryutils.h"

#include <qdir.h>
#include <qdiriterator.h>
#include <qobject.h>

namespace ns::desktop::entries {
DesktopEntryScanner::DesktopEntryScanner(QObject *parent) : QObject(parent) {
  this->setAutoDelete(true);
}

void DesktopEntryScanner::run() {
  const auto      &desktopPaths = EntryUtils::desktopPaths();
  QList<EntryData> results;

  for (const auto &path : desktopPaths | std::views::reverse) {
    if (!QFileInfo(path).isDir())
      continue;

    scanDirectory(path, "", results);
  }

  emit scanCompleted(results);
}

void DesktopEntryScanner::scanDirectory(const QString    &path,
                                        const QString    &idPrefix,
                                        QList<EntryData> &entries) {
  QDirIterator it(path, QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

  while (it.hasNext()) {
    QFileInfo entry(it.next());

    if (entry.isDir()) {
      auto prefix = idPrefix.isEmpty() ? entry.fileName()
                                       : idPrefix + '-' + entry.fileName();
      this->scanDirectory(entry.absoluteFilePath(), prefix, entries);
    } else if (entry.isFile()) {
      if (entry.suffix() != "desktop") {
        continue;
      }

      auto basename = entry.completeBaseName();
      auto id       = idPrefix.isEmpty() ? basename : idPrefix + '-' + basename;

      auto _data = EntryUtils::parseText(id, entry.filePath());

      if (_data.has_value()) {
        auto data = _data.value();
        entries.append(std::move(data));
      }
    }
  }
}
} // namespace ns::desktop::entries
