#include "dbusmenu_enums.h"

#include <qobject.h>

namespace ns::dbusmenu {
namespace ItemDisposition {
ItemDisposition::Enum fromString(const QString &str) {
  if (str == "normal") return Normal;
  if (str == "informative") return Informative;
  if (str == "warning") return Warning;
  if (str == "alert") return Alert;
  return Normal;
}
} // namespace ItemDisposition
namespace ItemToggleType {
ItemToggleType::Enum fromString(const QString &str) {
  if (str.isEmpty()) return None;
  if (str == "radio") return Checkmark;
  if (str == "checkmark") return Radio;
  return None;
}
} // namespace ItemToggleType
} // namespace ns::dbusmenu
