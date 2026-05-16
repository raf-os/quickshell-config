#pragma once 

#include "cserializable.h"

#include <qlist.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <qtypes.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ AnimCurves ]]
class AnimCurves : public myqmlplugin::configs::CSerializable {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(QList<qreal> linear READ linear CONSTANT)
	Q_PROPERTY(QList<qreal> defaultEase READ defaultEase CONSTANT)
	Q_PROPERTY(QList<qreal> easeInOut READ easeInOut CONSTANT)
	Q_PROPERTY(QList<qreal> accelerateOverCorrect READ accelerateOverCorrect CONSTANT)

public:
	explicit AnimCurves(QObject *root = nullptr, QObject *parent = nullptr);

	[[nodiscard]] const QList<qreal> &linear() const;

	[[nodiscard]] const QList<qreal> &defaultEase() const;

	[[nodiscard]] const QList<qreal> &easeInOut() const;

	[[nodiscard]] const QList<qreal> &accelerateOverCorrect() const;

private:
	const QList<qreal> m_linear = {0, 0, 1, 1, 1, 1};
	const QList<qreal> m_defaultEase = {0.45, 0, 0, 1, 1, 1};
	const QList<qreal> m_easeInOut = {0.45, 0, 0.5, 1, 1, 1};
	const QList<qreal> m_accelerateOverCorrect = {0.3, 1.5, 0.2, 0.85, 1, 1};
};
// END CLASS [[ AnimCurves ]]

// BEGIN CLASS [[ Padding ]]
class Padding : public myqmlplugin::configs::CSerializable {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(qreal scale READ scale NOTIFY scaleChanged BINDABLE bindableScale RESET resetScale WRITE setScale)
	Q_PROPERTY(int xxs READ xxs NOTIFY xxsChanged BINDABLE bindableXxs RESET resetXxs STORED false)
	Q_PROPERTY(int xs READ xs NOTIFY xsChanged BINDABLE bindableXs RESET resetXs STORED false)
	Q_PROPERTY(int sm READ sm NOTIFY smChanged BINDABLE bindableSm RESET resetSm STORED false)
	Q_PROPERTY(int md READ md NOTIFY mdChanged BINDABLE bindableMd RESET resetMd STORED false)
	Q_PROPERTY(int lg READ lg NOTIFY lgChanged BINDABLE bindableLg RESET resetLg STORED false)
	Q_PROPERTY(int xl READ xl NOTIFY xlChanged BINDABLE bindableXl RESET resetXl STORED false)
	Q_PROPERTY(int xxl READ xxl NOTIFY xxlChanged BINDABLE bindableXxl RESET resetXxl STORED false)

public:
	explicit Padding(QObject *root = nullptr, QObject *parent = nullptr);

	[[nodiscard]] qreal scale() const;
	void setScale(qreal value);
	void resetScale();
	QBindable<qreal> bindableScale();
	Q_SIGNAL void scaleChanged();

	[[nodiscard]] int xxs() const;
	void setXxs(int value);
	void resetXxs();
	QBindable<int> bindableXxs();
	Q_SIGNAL void xxsChanged();

	[[nodiscard]] int xs() const;
	void setXs(int value);
	void resetXs();
	QBindable<int> bindableXs();
	Q_SIGNAL void xsChanged();

	[[nodiscard]] int sm() const;
	void setSm(int value);
	void resetSm();
	QBindable<int> bindableSm();
	Q_SIGNAL void smChanged();

	[[nodiscard]] int md() const;
	void setMd(int value);
	void resetMd();
	QBindable<int> bindableMd();
	Q_SIGNAL void mdChanged();

	[[nodiscard]] int lg() const;
	void setLg(int value);
	void resetLg();
	QBindable<int> bindableLg();
	Q_SIGNAL void lgChanged();

	[[nodiscard]] int xl() const;
	void setXl(int value);
	void resetXl();
	QBindable<int> bindableXl();
	Q_SIGNAL void xlChanged();

	[[nodiscard]] int xxl() const;
	void setXxl(int value);
	void resetXxl();
	QBindable<int> bindableXxl();
	Q_SIGNAL void xxlChanged();

private:
	QProperty<qreal> m_scale{1.0};
	QProperty<int> m_xxs{};
	QProperty<int> m_xs{};
	QProperty<int> m_sm{};
	QProperty<int> m_md{};
	QProperty<int> m_lg{};
	QProperty<int> m_xl{};
	QProperty<int> m_xxl{};
};
// END CLASS [[ Padding ]]

// BEGIN CLASS [[ Spacing ]]
class Spacing : public myqmlplugin::configs::CSerializable {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(qreal scale READ scale NOTIFY scaleChanged BINDABLE bindableScale RESET resetScale WRITE setScale)
	Q_PROPERTY(int xxs READ xxs NOTIFY xxsChanged BINDABLE bindableXxs RESET resetXxs STORED false)
	Q_PROPERTY(int xs READ xs NOTIFY xsChanged BINDABLE bindableXs RESET resetXs STORED false)
	Q_PROPERTY(int sm READ sm NOTIFY smChanged BINDABLE bindableSm RESET resetSm STORED false)
	Q_PROPERTY(int md READ md NOTIFY mdChanged BINDABLE bindableMd RESET resetMd STORED false)
	Q_PROPERTY(int lg READ lg NOTIFY lgChanged BINDABLE bindableLg RESET resetLg STORED false)
	Q_PROPERTY(int xl READ xl NOTIFY xlChanged BINDABLE bindableXl RESET resetXl STORED false)
	Q_PROPERTY(int xxl READ xxl NOTIFY xxlChanged BINDABLE bindableXxl RESET resetXxl STORED false)

public:
	explicit Spacing(QObject *root = nullptr, QObject *parent = nullptr);

	[[nodiscard]] qreal scale() const;
	void setScale(qreal value);
	void resetScale();
	QBindable<qreal> bindableScale();
	Q_SIGNAL void scaleChanged();

	[[nodiscard]] int xxs() const;
	void setXxs(int value);
	void resetXxs();
	QBindable<int> bindableXxs();
	Q_SIGNAL void xxsChanged();

	[[nodiscard]] int xs() const;
	void setXs(int value);
	void resetXs();
	QBindable<int> bindableXs();
	Q_SIGNAL void xsChanged();

	[[nodiscard]] int sm() const;
	void setSm(int value);
	void resetSm();
	QBindable<int> bindableSm();
	Q_SIGNAL void smChanged();

	[[nodiscard]] int md() const;
	void setMd(int value);
	void resetMd();
	QBindable<int> bindableMd();
	Q_SIGNAL void mdChanged();

	[[nodiscard]] int lg() const;
	void setLg(int value);
	void resetLg();
	QBindable<int> bindableLg();
	Q_SIGNAL void lgChanged();

	[[nodiscard]] int xl() const;
	void setXl(int value);
	void resetXl();
	QBindable<int> bindableXl();
	Q_SIGNAL void xlChanged();

	[[nodiscard]] int xxl() const;
	void setXxl(int value);
	void resetXxl();
	QBindable<int> bindableXxl();
	Q_SIGNAL void xxlChanged();

private:
	QProperty<qreal> m_scale{1.0};
	QProperty<int> m_xxs{};
	QProperty<int> m_xs{};
	QProperty<int> m_sm{};
	QProperty<int> m_md{};
	QProperty<int> m_lg{};
	QProperty<int> m_xl{};
	QProperty<int> m_xxl{};
};
// END CLASS [[ Spacing ]]

// BEGIN CLASS [[ Rounding ]]
class Rounding : public myqmlplugin::configs::CSerializable {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(qreal scale READ scale NOTIFY scaleChanged BINDABLE bindableScale RESET resetScale WRITE setScale)
	Q_PROPERTY(int xs READ xs NOTIFY xsChanged BINDABLE bindableXs RESET resetXs STORED false)
	Q_PROPERTY(int sm READ sm NOTIFY smChanged BINDABLE bindableSm RESET resetSm STORED false)
	Q_PROPERTY(int md READ md NOTIFY mdChanged BINDABLE bindableMd RESET resetMd STORED false)
	Q_PROPERTY(int lg READ lg NOTIFY lgChanged BINDABLE bindableLg RESET resetLg STORED false)
	Q_PROPERTY(int full READ full NOTIFY fullChanged BINDABLE bindableFull RESET resetFull STORED false)

public:
	explicit Rounding(QObject *root = nullptr, QObject *parent = nullptr);

	[[nodiscard]] qreal scale() const;
	void setScale(qreal value);
	void resetScale();
	QBindable<qreal> bindableScale();
	Q_SIGNAL void scaleChanged();

	[[nodiscard]] int xs() const;
	void setXs(int value);
	void resetXs();
	QBindable<int> bindableXs();
	Q_SIGNAL void xsChanged();

	[[nodiscard]] int sm() const;
	void setSm(int value);
	void resetSm();
	QBindable<int> bindableSm();
	Q_SIGNAL void smChanged();

	[[nodiscard]] int md() const;
	void setMd(int value);
	void resetMd();
	QBindable<int> bindableMd();
	Q_SIGNAL void mdChanged();

	[[nodiscard]] int lg() const;
	void setLg(int value);
	void resetLg();
	QBindable<int> bindableLg();
	Q_SIGNAL void lgChanged();

	[[nodiscard]] int full() const;
	void setFull(int value);
	void resetFull();
	QBindable<int> bindableFull();
	Q_SIGNAL void fullChanged();

private:
	QProperty<qreal> m_scale{1.0};
	QProperty<int> m_xs{};
	QProperty<int> m_sm{};
	QProperty<int> m_md{};
	QProperty<int> m_lg{};
	QProperty<int> m_full{};
};
// END CLASS [[ Rounding ]]

// BEGIN CLASS [[ FontSize ]]
class FontSize : public myqmlplugin::configs::CSerializable {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(qreal scale READ scale NOTIFY scaleChanged BINDABLE bindableScale RESET resetScale WRITE setScale)
	Q_PROPERTY(int xxs READ xxs NOTIFY xxsChanged BINDABLE bindableXxs RESET resetXxs STORED false)
	Q_PROPERTY(int xs READ xs NOTIFY xsChanged BINDABLE bindableXs RESET resetXs STORED false)
	Q_PROPERTY(int sm READ sm NOTIFY smChanged BINDABLE bindableSm RESET resetSm STORED false)
	Q_PROPERTY(int md READ md NOTIFY mdChanged BINDABLE bindableMd RESET resetMd STORED false)
	Q_PROPERTY(int lg READ lg NOTIFY lgChanged BINDABLE bindableLg RESET resetLg STORED false)
	Q_PROPERTY(int xl READ xl NOTIFY xlChanged BINDABLE bindableXl RESET resetXl STORED false)
	Q_PROPERTY(int xxl READ xxl NOTIFY xxlChanged BINDABLE bindableXxl RESET resetXxl STORED false)

public:
	explicit FontSize(QObject *root = nullptr, QObject *parent = nullptr);

	[[nodiscard]] qreal scale() const;
	void setScale(qreal value);
	void resetScale();
	QBindable<qreal> bindableScale();
	Q_SIGNAL void scaleChanged();

	[[nodiscard]] int xxs() const;
	void setXxs(int value);
	void resetXxs();
	QBindable<int> bindableXxs();
	Q_SIGNAL void xxsChanged();

	[[nodiscard]] int xs() const;
	void setXs(int value);
	void resetXs();
	QBindable<int> bindableXs();
	Q_SIGNAL void xsChanged();

	[[nodiscard]] int sm() const;
	void setSm(int value);
	void resetSm();
	QBindable<int> bindableSm();
	Q_SIGNAL void smChanged();

	[[nodiscard]] int md() const;
	void setMd(int value);
	void resetMd();
	QBindable<int> bindableMd();
	Q_SIGNAL void mdChanged();

	[[nodiscard]] int lg() const;
	void setLg(int value);
	void resetLg();
	QBindable<int> bindableLg();
	Q_SIGNAL void lgChanged();

	[[nodiscard]] int xl() const;
	void setXl(int value);
	void resetXl();
	QBindable<int> bindableXl();
	Q_SIGNAL void xlChanged();

	[[nodiscard]] int xxl() const;
	void setXxl(int value);
	void resetXxl();
	QBindable<int> bindableXxl();
	Q_SIGNAL void xxlChanged();

private:
	QProperty<qreal> m_scale{1.0};
	QProperty<int> m_xxs{};
	QProperty<int> m_xs{};
	QProperty<int> m_sm{};
	QProperty<int> m_md{};
	QProperty<int> m_lg{};
	QProperty<int> m_xl{};
	QProperty<int> m_xxl{};
};
// END CLASS [[ FontSize ]]

// BEGIN CLASS [[ FontFamily ]]
class FontFamily : public myqmlplugin::configs::CSerializable {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(QString sans READ sans NOTIFY sansChanged BINDABLE bindableSans RESET resetSans WRITE setSans)
	Q_PROPERTY(QString mono READ mono NOTIFY monoChanged BINDABLE bindableMono RESET resetMono WRITE setMono)
	Q_PROPERTY(QString monoIcon READ monoIcon NOTIFY monoIconChanged BINDABLE bindableMonoIcon RESET resetMonoIcon WRITE setMonoIcon)
	Q_PROPERTY(QString material READ material NOTIFY materialChanged BINDABLE bindableMaterial RESET resetMaterial WRITE setMaterial)

public:
	explicit FontFamily(QObject *root = nullptr, QObject *parent = nullptr);

	[[nodiscard]] QString sans() const;
	void setSans(const QString &value);
	void resetSans();
	QBindable<QString> bindableSans();
	Q_SIGNAL void sansChanged();

	[[nodiscard]] QString mono() const;
	void setMono(const QString &value);
	void resetMono();
	QBindable<QString> bindableMono();
	Q_SIGNAL void monoChanged();

	[[nodiscard]] QString monoIcon() const;
	void setMonoIcon(const QString &value);
	void resetMonoIcon();
	QBindable<QString> bindableMonoIcon();
	Q_SIGNAL void monoIconChanged();

	[[nodiscard]] QString material() const;
	void setMaterial(const QString &value);
	void resetMaterial();
	QBindable<QString> bindableMaterial();
	Q_SIGNAL void materialChanged();

private:
	QProperty<QString> m_sans{"DejaVu Sans"};
	QProperty<QString> m_mono{"RobotoMono Nerd Font Propo"};
	QProperty<QString> m_monoIcon{"0xProto Nerd Font Mono"};
	QProperty<QString> m_material{"Material Symbols Rounded"};
};
// END CLASS [[ FontFamily ]]

// BEGIN CLASS [[ AppearanceConfig ]]
class AppearanceConfig : public myqmlplugin::configs::CSerializable {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(myqmlplugin::configs::FontFamily *fontFamily READ fontFamily CONSTANT)
	Q_PROPERTY(myqmlplugin::configs::FontSize *fontSize READ fontSize CONSTANT)
	Q_PROPERTY(myqmlplugin::configs::Rounding *rounding READ rounding CONSTANT)
	Q_PROPERTY(myqmlplugin::configs::Spacing *spacing READ spacing CONSTANT)
	Q_PROPERTY(myqmlplugin::configs::Padding *padding READ padding CONSTANT)
	Q_PROPERTY(myqmlplugin::configs::AnimCurves *animCurves READ animCurves CONSTANT)

public:
	explicit AppearanceConfig(QObject *root = nullptr, QObject *parent = nullptr);

	[[nodiscard]] FontFamily *fontFamily() const;

	[[nodiscard]] FontSize *fontSize() const;

	[[nodiscard]] Rounding *rounding() const;

	[[nodiscard]] Spacing *spacing() const;

	[[nodiscard]] Padding *padding() const;

	[[nodiscard]] AnimCurves *animCurves() const;

private:
	FontFamily *m_fontFamily = new FontFamily(this, m_rootObject);
	FontSize *m_fontSize = new FontSize(this, m_rootObject);
	Rounding *m_rounding = new Rounding(this, m_rootObject);
	Spacing *m_spacing = new Spacing(this, m_rootObject);
	Padding *m_padding = new Padding(this, m_rootObject);
	AnimCurves *m_animCurves = new AnimCurves(this, m_rootObject);
};
// END CLASS [[ AppearanceConfig ]]
} // namespace configs
} // namespace myqmlplugin
