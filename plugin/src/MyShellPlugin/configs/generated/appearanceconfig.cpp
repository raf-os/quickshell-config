#include <appearanceconfig.h>
#include <qproperty.h>
#include <qtypes.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qstring.h>
#include <qobject.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ FontSize ]]
FontSize::FontSize(QObject *parent): QObject(parent) {
	m_xxs.setBinding([this]() { return 8 * m_scale.value(); });
}

qreal FontSize::scale const { return m_scale; }

void FontSize::setScale(const qreal &value) {
	m_scale = value;
}
QBindable<qreal> FontSize::bindableScale() { return &m_scale; }

int FontSize::xxs const { return m_xxs; }

void FontSize::setXxs(const int &value) {
	m_xxs = value;
}
QBindable<int> FontSize::bindableXxs() { return &m_xxs; }
// END CLASS [[ FontSize ]]

// BEGIN CLASS [[ FontFamily ]]
FontFamily::FontFamily(QObject *parent): QObject(parent) {}

QString FontFamily::sans const { return m_sans; }

void FontFamily::setSans(const QString &value) {
	m_sans = value;
}
QBindable<QString> FontFamily::bindableSans() { return &m_sans; }

QString FontFamily::mono const { return m_mono; }

void FontFamily::setMono(const QString &value) {
	m_mono = value;
}
QBindable<QString> FontFamily::bindableMono() { return &m_mono; }

QString FontFamily::monoIcon const { return m_monoIcon; }

void FontFamily::setMonoIcon(const QString &value) {
	m_monoIcon = value;
}
QBindable<QString> FontFamily::bindableMonoIcon() { return &m_monoIcon; }

QString FontFamily::material const { return m_material; }

void FontFamily::setMaterial(const QString &value) {
	m_material = value;
}
QBindable<QString> FontFamily::bindableMaterial() { return &m_material; }
// END CLASS [[ FontFamily ]]

// BEGIN CLASS [[ AppearanceConfig ]]
AppearanceConfig::AppearanceConfig(QObject *parent): QObject(parent) {}

FontFamily *AppearanceConfig::fontFamily() const { return m_fontFamily; }

FontSize *AppearanceConfig::fontSize() const { return m_fontSize; }
// END CLASS [[ AppearanceConfig ]]
} // namespace configs
} // namespace myqmlplugin
