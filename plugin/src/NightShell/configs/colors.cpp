#include "colors.h"

#include <qjsengine.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlengine.h>

#include "helpermacros.h"

namespace ns::configs {
Colors::Colors(QObject *parent) : QObject(parent) {}

AUTO_MEYERS_SINGLETON_IMPL(Colors)

ColorThemeMetadata *Colors::metadata() { return &m_metadata; }
ColorData          *Colors::current() {
  if (b_isPreviewing.value() && m_themePreview) return m_themePreview;
  return &m_theme;
}
} // namespace ns::configs
