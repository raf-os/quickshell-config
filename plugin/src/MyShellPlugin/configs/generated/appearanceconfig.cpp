#include <qtmetamacros.h>
#include <qobject.h>
#include <qstring.h>
#include <qqmlintegration.h>
#include <qbindable.h>

namespace myqmlplugin{
namespace configs{
AppearanceConfig::AppearanceConfig(QObject *parent): QObject(parent) {}

FontFamily::FontFamily(QObject *parent): QObject(parent) {}

QString FontFamily::sans const { return m_sans; }

void FontFamily::setSans(const QString &value) {
	m_sans = value;
}

QString FontFamily::mono const { return m_mono; }

void FontFamily::setMono(const QString &value) {
	m_mono = value;
}

QString FontFamily::monoIcon const { return m_monoIcon; }

void FontFamily::setMonoIcon(const QString &value) {
	m_monoIcon = value;
}

QString FontFamily::material const { return m_material; }

void FontFamily::setMaterial(const QString &value) {
	m_material = value;
}

QString FontFamily::test const { return m_test; }

void FontFamily::setTest(const QString &value) {
	m_test = value;
}
} // namespace configs
} // namespace myqmlplugin
