#pragma once

#include <qobjectdefs.h>
#include <qtmetamacros.h>

namespace ns::dbusmenu {
namespace ItemDisposition {
Q_NAMESPACE

enum Enum : quint8 { Normal = 0, Informative = 1, Warning = 2, Alert = 3 };

Q_ENUM_NS(Enum)

ItemDisposition::Enum fromString(const QString &str);
} // namespace ItemDisposition

namespace ItemToggleType {
Q_NAMESPACE

enum Enum : quint8 {
  None      = 0,
  Checkmark = 1,
  Radio     = 2,
};

Q_ENUM_NS(Enum)

ItemToggleType::Enum fromString(const QString &str);
} // namespace ItemToggleType

namespace ItemCheckState {
Q_NAMESPACE

enum Enum : quint8 { Unchecked = 0, Checked = 1, PartiallyChecked = 2 };

Q_ENUM_NS(Enum)
} // namespace ItemCheckState
} // namespace ns::dbusmenu
