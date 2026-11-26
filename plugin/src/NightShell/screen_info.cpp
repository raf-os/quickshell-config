#include "screen_info.h"

namespace ns {
ScreenInfo::ScreenInfo(QScreen *screen) : QObject(screen), m_screen(screen) {}

QString ScreenInfo::name() const { return m_screen->name(); }
QString ScreenInfo::model() const { return m_screen->model(); }
QString ScreenInfo::serial() const { return m_screen->serialNumber(); }

QScreen *ScreenInfo::screen() { return m_screen; }

bool ScreenInfo::hasData() const { return m_data.get() != nullptr; }

IScreenData *ScreenInfo::data() { return m_data.get(); }
} // namespace ns
