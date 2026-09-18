#include "appearance.h"
#include "serializableobject.h"

namespace ns::configs {
FontFamily::FontFamily(const QString &className, QObject *root, QObject *parent) : SerializableObject(className, root, parent) {}

QBindable<QString> FontFamily::bindableSans() const { return &b_sans; }
void FontFamily::resetSans() { b_sans = "DejaVu Sans"; }
QBindable<QString> FontFamily::bindableMono() const { return &b_mono; }
void FontFamily::resetMono() { b_mono = "RobotoMono Nerd Font Propo"; }

Appearance::Appearance(const QString &className, QObject *root, QObject *parent) : SerializableObject(className, root, parent) {}

QBindable<int> Appearance::bindableScaling() const { return &b_scaling; }
void Appearance::resetScaling() { b_scaling = 1; }
FontFamily *Appearance::fontFamily() { return &m_fontFamily; }
} // namespace ns::configs