#pragma once

#include <qobject.h>

#include "screencopy_context.h"

namespace ns::wayland::screencopy {
class ScreencopyManager {
public:
  static ScreencopyContext *createContext(QObject *object);

  // TODO: Dynamically query the wayland compositor for supported protocols so
  // the correct context is created
};
} // namespace ns::wayland::screencopy
