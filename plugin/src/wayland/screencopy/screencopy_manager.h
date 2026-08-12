#pragma once

#include <qobject.h>

#include "screencopy_context.h"

namespace ns::wayland::screencopy {
class ScreencopyManager {
  static ScreencopyContext *createContext(QObject *object);
};
} // namespace ns::wayland::screencopy
