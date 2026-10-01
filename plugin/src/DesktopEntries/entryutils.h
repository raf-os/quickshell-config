#pragma once

#include <optional>

#include <qcontainerfwd.h>
#include <qobject.h>

#include "entryscanner.h"
#include "ns_desktopentries_shared_global.h"

namespace ns::desktop::entries {
class NS_DESKTOPENTRIES_EXPORT EntryUtils {
public:
  static const QStringList       &desktopPaths();
  static std::optional<EntryData> parseText(
      const QString &id, const QString &filePath);
  static QStringList parseExecString(const QString &execString);
};

} // namespace ns::desktop::entries
