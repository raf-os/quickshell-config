#include "hyprkeyboardlayout.h"

#include <qobject.h>

namespace ns::hyprland {
HyprKeyboardLayout::HyprKeyboardLayout(const QString &layout,
                                       const QString &variant,
                                       const QString &description,
                                       QObject       *parent)
    : QObject(parent),
      m_layout(layout),
      m_variant(variant),
      m_description(description) {}

QString HyprKeyboardLayout::layout() const { return m_layout; }
QString HyprKeyboardLayout::variant() const { return m_variant; }
QString HyprKeyboardLayout::description() const { return m_layout; }
} // namespace ns::hyprland
