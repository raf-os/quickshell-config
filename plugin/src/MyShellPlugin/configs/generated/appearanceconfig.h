#pragma once 

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
class AnimCurves : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(QList<qreal> linear READ linear CONSTANT)
	Q_PROPERTY(QList<qreal> defaultEase READ defaultEase CONSTANT)
	Q_PROPERTY(QList<qreal> easeInOut READ easeInOut CONSTANT)
	Q_PROPERTY(QList<qreal> accelerateOverCorrect READ accelerateOverCorrect CONSTANT)

public:
	explicit AnimCurves(QObject *parent = nullptr);

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
class Padding : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged BINDABLE bindableScale)
	Q_PROPERTY(int xxs READ xxs WRITE setXxs NOTIFY xxsChanged BINDABLE bindableXxs)
	Q_PROPERTY(int xs READ xs WRITE setXs NOTIFY xsChanged BINDABLE bindableXs)
	Q_PROPERTY(int sm READ sm WRITE setSm NOTIFY smChanged BINDABLE bindableSm)
	Q_PROPERTY(int md READ md WRITE setMd NOTIFY mdChanged BINDABLE bindableMd)
	Q_PROPERTY(int lg READ lg WRITE setLg NOTIFY lgChanged BINDABLE bindableLg)
	Q_PROPERTY(int xl READ xl WRITE setXl NOTIFY xlChanged BINDABLE bindableXl)
	Q_PROPERTY(int xxl READ xxl WRITE setXxl NOTIFY xxlChanged BINDABLE bindableXxl)

public:
	explicit Padding(QObject *parent = nullptr);

	[[nodiscard]] qreal scale() const;
	void setScale(qreal value);
	QBindable<qreal> bindableScale();
	Q_SIGNAL void scaleChanged();

	[[nodiscard]] int xxs() const;
	void setXxs(int value);
	QBindable<int> bindableXxs();
	Q_SIGNAL void xxsChanged();

	[[nodiscard]] int xs() const;
	void setXs(int value);
	QBindable<int> bindableXs();
	Q_SIGNAL void xsChanged();

	[[nodiscard]] int sm() const;
	void setSm(int value);
	QBindable<int> bindableSm();
	Q_SIGNAL void smChanged();

	[[nodiscard]] int md() const;
	void setMd(int value);
	QBindable<int> bindableMd();
	Q_SIGNAL void mdChanged();

	[[nodiscard]] int lg() const;
	void setLg(int value);
	QBindable<int> bindableLg();
	Q_SIGNAL void lgChanged();

	[[nodiscard]] int xl() const;
	void setXl(int value);
	QBindable<int> bindableXl();
	Q_SIGNAL void xlChanged();

	[[nodiscard]] int xxl() const;
	void setXxl(int value);
	QBindable<int> bindableXxl();
	Q_SIGNAL void xxlChanged();

private:
	QProperty<qreal> m_scale;
	QProperty<int> m_xxs;
	QProperty<int> m_xs;
	QProperty<int> m_sm;
	QProperty<int> m_md;
	QProperty<int> m_lg;
	QProperty<int> m_xl;
	QProperty<int> m_xxl;
};
// END CLASS [[ Padding ]]

// BEGIN CLASS [[ Spacing ]]
class Spacing : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged BINDABLE bindableScale)
	Q_PROPERTY(int xxs READ xxs WRITE setXxs NOTIFY xxsChanged BINDABLE bindableXxs)
	Q_PROPERTY(int xs READ xs WRITE setXs NOTIFY xsChanged BINDABLE bindableXs)
	Q_PROPERTY(int sm READ sm WRITE setSm NOTIFY smChanged BINDABLE bindableSm)
	Q_PROPERTY(int md READ md WRITE setMd NOTIFY mdChanged BINDABLE bindableMd)
	Q_PROPERTY(int lg READ lg WRITE setLg NOTIFY lgChanged BINDABLE bindableLg)
	Q_PROPERTY(int xl READ xl WRITE setXl NOTIFY xlChanged BINDABLE bindableXl)
	Q_PROPERTY(int xxl READ xxl WRITE setXxl NOTIFY xxlChanged BINDABLE bindableXxl)

public:
	explicit Spacing(QObject *parent = nullptr);

	[[nodiscard]] qreal scale() const;
	void setScale(qreal value);
	QBindable<qreal> bindableScale();
	Q_SIGNAL void scaleChanged();

	[[nodiscard]] int xxs() const;
	void setXxs(int value);
	QBindable<int> bindableXxs();
	Q_SIGNAL void xxsChanged();

	[[nodiscard]] int xs() const;
	void setXs(int value);
	QBindable<int> bindableXs();
	Q_SIGNAL void xsChanged();

	[[nodiscard]] int sm() const;
	void setSm(int value);
	QBindable<int> bindableSm();
	Q_SIGNAL void smChanged();

	[[nodiscard]] int md() const;
	void setMd(int value);
	QBindable<int> bindableMd();
	Q_SIGNAL void mdChanged();

	[[nodiscard]] int lg() const;
	void setLg(int value);
	QBindable<int> bindableLg();
	Q_SIGNAL void lgChanged();

	[[nodiscard]] int xl() const;
	void setXl(int value);
	QBindable<int> bindableXl();
	Q_SIGNAL void xlChanged();

	[[nodiscard]] int xxl() const;
	void setXxl(int value);
	QBindable<int> bindableXxl();
	Q_SIGNAL void xxlChanged();

private:
	QProperty<qreal> m_scale;
	QProperty<int> m_xxs;
	QProperty<int> m_xs;
	QProperty<int> m_sm;
	QProperty<int> m_md;
	QProperty<int> m_lg;
	QProperty<int> m_xl;
	QProperty<int> m_xxl;
};
// END CLASS [[ Spacing ]]

// BEGIN CLASS [[ Rounding ]]
class Rounding : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged BINDABLE bindableScale)
	Q_PROPERTY(int xs READ xs WRITE setXs NOTIFY xsChanged BINDABLE bindableXs)
	Q_PROPERTY(int sm READ sm WRITE setSm NOTIFY smChanged BINDABLE bindableSm)
	Q_PROPERTY(int md READ md WRITE setMd NOTIFY mdChanged BINDABLE bindableMd)
	Q_PROPERTY(int lg READ lg WRITE setLg NOTIFY lgChanged BINDABLE bindableLg)
	Q_PROPERTY(int full READ full WRITE setFull NOTIFY fullChanged BINDABLE bindableFull)

public:
	explicit Rounding(QObject *parent = nullptr);

	[[nodiscard]] qreal scale() const;
	void setScale(qreal value);
	QBindable<qreal> bindableScale();
	Q_SIGNAL void scaleChanged();

	[[nodiscard]] int xs() const;
	void setXs(int value);
	QBindable<int> bindableXs();
	Q_SIGNAL void xsChanged();

	[[nodiscard]] int sm() const;
	void setSm(int value);
	QBindable<int> bindableSm();
	Q_SIGNAL void smChanged();

	[[nodiscard]] int md() const;
	void setMd(int value);
	QBindable<int> bindableMd();
	Q_SIGNAL void mdChanged();

	[[nodiscard]] int lg() const;
	void setLg(int value);
	QBindable<int> bindableLg();
	Q_SIGNAL void lgChanged();

	[[nodiscard]] int full() const;
	void setFull(int value);
	QBindable<int> bindableFull();
	Q_SIGNAL void fullChanged();

private:
	QProperty<qreal> m_scale;
	QProperty<int> m_xs;
	QProperty<int> m_sm;
	QProperty<int> m_md;
	QProperty<int> m_lg;
	QProperty<int> m_full;
};
// END CLASS [[ Rounding ]]

// BEGIN CLASS [[ FontSize ]]
class FontSize : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged BINDABLE bindableScale)
	Q_PROPERTY(int xxs READ xxs WRITE setXxs NOTIFY xxsChanged BINDABLE bindableXxs)
	Q_PROPERTY(int xs READ xs WRITE setXs NOTIFY xsChanged BINDABLE bindableXs)
	Q_PROPERTY(int sm READ sm WRITE setSm NOTIFY smChanged BINDABLE bindableSm)
	Q_PROPERTY(int md READ md WRITE setMd NOTIFY mdChanged BINDABLE bindableMd)
	Q_PROPERTY(int lg READ lg WRITE setLg NOTIFY lgChanged BINDABLE bindableLg)
	Q_PROPERTY(int xl READ xl WRITE setXl NOTIFY xlChanged BINDABLE bindableXl)
	Q_PROPERTY(int xxl READ xxl WRITE setXxl NOTIFY xxlChanged BINDABLE bindableXxl)

public:
	explicit FontSize(QObject *parent = nullptr);

	[[nodiscard]] qreal scale() const;
	void setScale(qreal value);
	QBindable<qreal> bindableScale();
	Q_SIGNAL void scaleChanged();

	[[nodiscard]] int xxs() const;
	void setXxs(int value);
	QBindable<int> bindableXxs();
	Q_SIGNAL void xxsChanged();

	[[nodiscard]] int xs() const;
	void setXs(int value);
	QBindable<int> bindableXs();
	Q_SIGNAL void xsChanged();

	[[nodiscard]] int sm() const;
	void setSm(int value);
	QBindable<int> bindableSm();
	Q_SIGNAL void smChanged();

	[[nodiscard]] int md() const;
	void setMd(int value);
	QBindable<int> bindableMd();
	Q_SIGNAL void mdChanged();

	[[nodiscard]] int lg() const;
	void setLg(int value);
	QBindable<int> bindableLg();
	Q_SIGNAL void lgChanged();

	[[nodiscard]] int xl() const;
	void setXl(int value);
	QBindable<int> bindableXl();
	Q_SIGNAL void xlChanged();

	[[nodiscard]] int xxl() const;
	void setXxl(int value);
	QBindable<int> bindableXxl();
	Q_SIGNAL void xxlChanged();

private:
	QProperty<qreal> m_scale;
	QProperty<int> m_xxs;
	QProperty<int> m_xs;
	QProperty<int> m_sm;
	QProperty<int> m_md;
	QProperty<int> m_lg;
	QProperty<int> m_xl;
	QProperty<int> m_xxl;
};
// END CLASS [[ FontSize ]]

// BEGIN CLASS [[ FontFamily ]]
class FontFamily : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(QString sans READ sans WRITE setSans NOTIFY sansChanged BINDABLE bindableSans)
	Q_PROPERTY(QString mono READ mono WRITE setMono NOTIFY monoChanged BINDABLE bindableMono)
	Q_PROPERTY(QString monoIcon READ monoIcon WRITE setMonoIcon NOTIFY monoIconChanged BINDABLE bindableMonoIcon)
	Q_PROPERTY(QString material READ material WRITE setMaterial NOTIFY materialChanged BINDABLE bindableMaterial)

public:
	explicit FontFamily(QObject *parent = nullptr);

	[[nodiscard]] QString sans() const;
	void setSans(const QString &value);
	QBindable<QString> bindableSans();
	Q_SIGNAL void sansChanged();

	[[nodiscard]] QString mono() const;
	void setMono(const QString &value);
	QBindable<QString> bindableMono();
	Q_SIGNAL void monoChanged();

	[[nodiscard]] QString monoIcon() const;
	void setMonoIcon(const QString &value);
	QBindable<QString> bindableMonoIcon();
	Q_SIGNAL void monoIconChanged();

	[[nodiscard]] QString material() const;
	void setMaterial(const QString &value);
	QBindable<QString> bindableMaterial();
	Q_SIGNAL void materialChanged();

private:
	QProperty<QString> m_sans;
	QProperty<QString> m_mono;
	QProperty<QString> m_monoIcon;
	QProperty<QString> m_material;
};
// END CLASS [[ FontFamily ]]

// BEGIN CLASS [[ AppearanceConfig ]]
class AppearanceConfig : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(FontFamily *fontFamily READ fontFamily CONSTANT)
	Q_PROPERTY(FontSize *fontSize READ fontSize CONSTANT)
	Q_PROPERTY(Rounding *rounding READ rounding CONSTANT)
	Q_PROPERTY(Spacing *spacing READ spacing CONSTANT)
	Q_PROPERTY(Padding *padding READ padding CONSTANT)
	Q_PROPERTY(AnimCurves *animCurves READ animCurves CONSTANT)

public:
	explicit AppearanceConfig(QObject *parent = nullptr);

	[[nodiscard]] FontFamily *fontFamily() const;

	[[nodiscard]] FontSize *fontSize() const;

	[[nodiscard]] Rounding *rounding() const;

	[[nodiscard]] Spacing *spacing() const;

	[[nodiscard]] Padding *padding() const;

	[[nodiscard]] AnimCurves *animCurves() const;

private:
	FontFamily *m_fontFamily = new FontFamily(this);
	FontSize *m_fontSize = new FontSize(this);
	Rounding *m_rounding = new Rounding(this);
	Spacing *m_spacing = new Spacing(this);
	Padding *m_padding = new Padding(this);
	AnimCurves *m_animCurves = new AnimCurves(this);
};
// END CLASS [[ AppearanceConfig ]]
} // namespace configs
} // namespace myqmlplugin
