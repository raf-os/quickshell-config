#include "screen_info.h"

namespace ns {
ScreenInfo::ScreenInfo(QScreen *screen) : QObject(screen), m_screen(screen) {}

[[nodiscard]] QString ScreenInfo::name() const { return m_screen->name(); }
[[nodiscard]] QString ScreenInfo::model() const { return m_screen->model(); }
[[nodiscard]] QString ScreenInfo::serial() const {
  return m_screen->serialNumber();
}

[[nodiscard]] QScreen *ScreenInfo::screen() { return m_screen; }

bool ScreenInfo::hasData() const { return m_data.get() != nullptr; }
} // namespace ns
