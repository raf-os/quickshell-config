#include "appearanceconfig.h"

#include <qobject.h>
#include <qproperty.h>
#include <qlist.h>
#include <qtypes.h>
#include <qstring.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ AnimCurves ]]
AnimCurves::AnimCurves(QObject *parent): QObject(parent) {}

const QList<qreal> &AnimCurves::linear() const { return m_linear; }

const QList<qreal> &AnimCurves::defaultEase() const { return m_defaultEase; }

const QList<qreal> &AnimCurves::easeInOut() const { return m_easeInOut; }

const QList<qreal> &AnimCurves::accelerateOverCorrect() const { return m_accelerateOverCorrect; }
// END CLASS [[ AnimCurves ]]

// BEGIN CLASS [[ Padding ]]
Padding::Padding(QObject *parent): QObject(parent) {
	m_xxs.setBinding([this]() -> int { return 4 * m_scale.value(); });
	m_xs.setBinding([this]() -> int { return 5 * m_scale.value(); });
	m_sm.setBinding([this]() -> int { return 7 * m_scale.value(); });
	m_md.setBinding([this]() -> int { return 10 * m_scale.value(); });
	m_lg.setBinding([this]() -> int { return 12 * m_scale.value(); });
	m_xl.setBinding([this]() -> int { return 15 * m_scale.value(); });
	m_xxl.setBinding([this]() -> int { return 20 * m_scale.value(); });
}

qreal Padding::scale() const { return m_scale; }

void Padding::setScale(qreal value) {
	m_scale = value;
}
QBindable<qreal> Padding::bindableScale() { return &m_scale; }

int Padding::xxs() const { return m_xxs; }

void Padding::setXxs(int value) {
	m_xxs = value;
}
QBindable<int> Padding::bindableXxs() { return &m_xxs; }

int Padding::xs() const { return m_xs; }

void Padding::setXs(int value) {
	m_xs = value;
}
QBindable<int> Padding::bindableXs() { return &m_xs; }

int Padding::sm() const { return m_sm; }

void Padding::setSm(int value) {
	m_sm = value;
}
QBindable<int> Padding::bindableSm() { return &m_sm; }

int Padding::md() const { return m_md; }

void Padding::setMd(int value) {
	m_md = value;
}
QBindable<int> Padding::bindableMd() { return &m_md; }

int Padding::lg() const { return m_lg; }

void Padding::setLg(int value) {
	m_lg = value;
}
QBindable<int> Padding::bindableLg() { return &m_lg; }

int Padding::xl() const { return m_xl; }

void Padding::setXl(int value) {
	m_xl = value;
}
QBindable<int> Padding::bindableXl() { return &m_xl; }

int Padding::xxl() const { return m_xxl; }

void Padding::setXxl(int value) {
	m_xxl = value;
}
QBindable<int> Padding::bindableXxl() { return &m_xxl; }
// END CLASS [[ Padding ]]

// BEGIN CLASS [[ Spacing ]]
Spacing::Spacing(QObject *parent): QObject(parent) {
	m_xxs.setBinding([this]() -> int { return 4 * m_scale.value(); });
	m_xs.setBinding([this]() -> int { return 9 * m_scale.value(); });
	m_sm.setBinding([this]() -> int { return 10 * m_scale.value(); });
	m_md.setBinding([this]() -> int { return 12 * m_scale.value(); });
	m_lg.setBinding([this]() -> int { return 14 * m_scale.value(); });
	m_xl.setBinding([this]() -> int { return 17 * m_scale.value(); });
	m_xxl.setBinding([this]() -> int { return 20 * m_scale.value(); });
}

qreal Spacing::scale() const { return m_scale; }

void Spacing::setScale(qreal value) {
	m_scale = value;
}
QBindable<qreal> Spacing::bindableScale() { return &m_scale; }

int Spacing::xxs() const { return m_xxs; }

void Spacing::setXxs(int value) {
	m_xxs = value;
}
QBindable<int> Spacing::bindableXxs() { return &m_xxs; }

int Spacing::xs() const { return m_xs; }

void Spacing::setXs(int value) {
	m_xs = value;
}
QBindable<int> Spacing::bindableXs() { return &m_xs; }

int Spacing::sm() const { return m_sm; }

void Spacing::setSm(int value) {
	m_sm = value;
}
QBindable<int> Spacing::bindableSm() { return &m_sm; }

int Spacing::md() const { return m_md; }

void Spacing::setMd(int value) {
	m_md = value;
}
QBindable<int> Spacing::bindableMd() { return &m_md; }

int Spacing::lg() const { return m_lg; }

void Spacing::setLg(int value) {
	m_lg = value;
}
QBindable<int> Spacing::bindableLg() { return &m_lg; }

int Spacing::xl() const { return m_xl; }

void Spacing::setXl(int value) {
	m_xl = value;
}
QBindable<int> Spacing::bindableXl() { return &m_xl; }

int Spacing::xxl() const { return m_xxl; }

void Spacing::setXxl(int value) {
	m_xxl = value;
}
QBindable<int> Spacing::bindableXxl() { return &m_xxl; }
// END CLASS [[ Spacing ]]

// BEGIN CLASS [[ Rounding ]]
Rounding::Rounding(QObject *parent): QObject(parent) {
	m_xs.setBinding([this]() -> int { return 4 * m_scale.value(); });
	m_sm.setBinding([this]() -> int { return 8 * m_scale.value(); });
	m_md.setBinding([this]() -> int { return 16 * m_scale.value(); });
	m_lg.setBinding([this]() -> int { return 24 * m_scale.value(); });
	m_full.setBinding([this]() -> int { return 1000 * m_scale.value(); });
}

qreal Rounding::scale() const { return m_scale; }

void Rounding::setScale(qreal value) {
	m_scale = value;
}
QBindable<qreal> Rounding::bindableScale() { return &m_scale; }

int Rounding::xs() const { return m_xs; }

void Rounding::setXs(int value) {
	m_xs = value;
}
QBindable<int> Rounding::bindableXs() { return &m_xs; }

int Rounding::sm() const { return m_sm; }

void Rounding::setSm(int value) {
	m_sm = value;
}
QBindable<int> Rounding::bindableSm() { return &m_sm; }

int Rounding::md() const { return m_md; }

void Rounding::setMd(int value) {
	m_md = value;
}
QBindable<int> Rounding::bindableMd() { return &m_md; }

int Rounding::lg() const { return m_lg; }

void Rounding::setLg(int value) {
	m_lg = value;
}
QBindable<int> Rounding::bindableLg() { return &m_lg; }

int Rounding::full() const { return m_full; }

void Rounding::setFull(int value) {
	m_full = value;
}
QBindable<int> Rounding::bindableFull() { return &m_full; }
// END CLASS [[ Rounding ]]

// BEGIN CLASS [[ FontSize ]]
FontSize::FontSize(QObject *parent): QObject(parent) {
	m_xxs.setBinding([this]() -> int { return 8 * m_scale.value(); });
	m_xs.setBinding([this]() -> int { return 9 * m_scale.value(); });
	m_sm.setBinding([this]() -> int { return 10 * m_scale.value(); });
	m_md.setBinding([this]() -> int { return 12 * m_scale.value(); });
	m_lg.setBinding([this]() -> int { return 14 * m_scale.value(); });
	m_xl.setBinding([this]() -> int { return 17 * m_scale.value(); });
	m_xxl.setBinding([this]() -> int { return 20 * m_scale.value(); });
}

qreal FontSize::scale() const { return m_scale; }

void FontSize::setScale(qreal value) {
	m_scale = value;
}
QBindable<qreal> FontSize::bindableScale() { return &m_scale; }

int FontSize::xxs() const { return m_xxs; }

void FontSize::setXxs(int value) {
	m_xxs = value;
}
QBindable<int> FontSize::bindableXxs() { return &m_xxs; }

int FontSize::xs() const { return m_xs; }

void FontSize::setXs(int value) {
	m_xs = value;
}
QBindable<int> FontSize::bindableXs() { return &m_xs; }

int FontSize::sm() const { return m_sm; }

void FontSize::setSm(int value) {
	m_sm = value;
}
QBindable<int> FontSize::bindableSm() { return &m_sm; }

int FontSize::md() const { return m_md; }

void FontSize::setMd(int value) {
	m_md = value;
}
QBindable<int> FontSize::bindableMd() { return &m_md; }

int FontSize::lg() const { return m_lg; }

void FontSize::setLg(int value) {
	m_lg = value;
}
QBindable<int> FontSize::bindableLg() { return &m_lg; }

int FontSize::xl() const { return m_xl; }

void FontSize::setXl(int value) {
	m_xl = value;
}
QBindable<int> FontSize::bindableXl() { return &m_xl; }

int FontSize::xxl() const { return m_xxl; }

void FontSize::setXxl(int value) {
	m_xxl = value;
}
QBindable<int> FontSize::bindableXxl() { return &m_xxl; }
// END CLASS [[ FontSize ]]

// BEGIN CLASS [[ FontFamily ]]
FontFamily::FontFamily(QObject *parent): QObject(parent) {}

QString FontFamily::sans() const { return m_sans; }

void FontFamily::setSans(const QString &value) {
	m_sans = value;
}
QBindable<QString> FontFamily::bindableSans() { return &m_sans; }

QString FontFamily::mono() const { return m_mono; }

void FontFamily::setMono(const QString &value) {
	m_mono = value;
}
QBindable<QString> FontFamily::bindableMono() { return &m_mono; }

QString FontFamily::monoIcon() const { return m_monoIcon; }

void FontFamily::setMonoIcon(const QString &value) {
	m_monoIcon = value;
}
QBindable<QString> FontFamily::bindableMonoIcon() { return &m_monoIcon; }

QString FontFamily::material() const { return m_material; }

void FontFamily::setMaterial(const QString &value) {
	m_material = value;
}
QBindable<QString> FontFamily::bindableMaterial() { return &m_material; }
// END CLASS [[ FontFamily ]]

// BEGIN CLASS [[ AppearanceConfig ]]
AppearanceConfig::AppearanceConfig(QObject *parent): QObject(parent) {}

FontFamily *AppearanceConfig::fontFamily() const { return m_fontFamily; }

FontSize *AppearanceConfig::fontSize() const { return m_fontSize; }

Rounding *AppearanceConfig::rounding() const { return m_rounding; }

Spacing *AppearanceConfig::spacing() const { return m_spacing; }

Padding *AppearanceConfig::padding() const { return m_padding; }

AnimCurves *AppearanceConfig::animCurves() const { return m_animCurves; }
// END CLASS [[ AppearanceConfig ]]
} // namespace configs
} // namespace myqmlplugin
