#pragma once

#include "entryscanner.h"
#include <optional>
#include <qcontainerfwd.h>
#include <qobject.h>

namespace ns::desktop::entries {
class EntryUtils {
public:
  static const QStringList       &desktopPaths();
  static std::optional<EntryData> parseText(const QString &id,
                                            const QString &filePath);
  static QStringList              parseExecString(const QString &execString);
};

} // namespace ns::desktop::entries
