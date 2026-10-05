#pragma once

#include <qcontainerfwd.h>
#include <qobject.h>
namespace misc {
bool parse_launcher_command(QStringList &args);
bool parse_misc_command(const QString &cmd, QStringList &args);
} // namespace misc
