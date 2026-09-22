#include "appearance.h"
#include "serializableobject.h"

namespace ns::configs {
FontFamily::FontFamily(const QString &className, QObject *root, QObject *parent) : SerializableObject(className, root, parent) {}

QBindable<QString> FontFamily::bindableSans() const { return &b_sans; }
void FontFamily::resetSans() { b_sans = "Inter"; }
QBindable<QString> FontFamily::bindableMono() const { return &b_mono; }
void FontFamily::resetMono() { b_mono = "RobotoMono Nerd Font Propo"; }

AppearanceConfig::AppearanceConfig(const QString &className, QObject *root, QObject *parent) : SerializableObject(className, root, parent) {}

QBindable<int> AppearanceConfig::bindableScaling() const { return &b_scaling; }
void AppearanceConfig::resetScaling() { b_scaling = 1; }
FontFamily *AppearanceConfig::fontFamily() { return &m_fontFamily; }
} // namespace ns::configs