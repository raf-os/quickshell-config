#pragma once 

#include <qproperty.h>
#include <qtypes.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qstring.h>
#include <qobject.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ FontSize ]]
class FontSize : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(scale READ scale WRITE setScale NOTIFY scaleChanged BINDABLE bindableScale)
	Q_PROPERTY(xxs READ xxs WRITE setXxs NOTIFY xxsChanged BINDABLE bindableXxs)

public:
	explicit FontSize(QObject *parent = nullptr);

	[[nodiscard]] qreal scale() const;
	void setScale(qreal scale);
	QBindable<qreal> bindableScale();
	Q_SIGNAL void scaleChanged();

	[[nodiscard]] int xxs() const;
	void setXxs(int xxs);
	QBindable<int> bindableXxs();
	Q_SIGNAL void xxsChanged();

private:
	QProperty<qreal> m_scale = 1.0;
	QProperty<int> m_xxs;

};
// END CLASS [[ FontSize ]]

// BEGIN CLASS [[ FontFamily ]]
class FontFamily : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(sans READ sans WRITE setSans NOTIFY sansChanged BINDABLE bindableSans)
	Q_PROPERTY(mono READ mono WRITE setMono NOTIFY monoChanged BINDABLE bindableMono)
	Q_PROPERTY(monoIcon READ monoIcon WRITE setMonoIcon NOTIFY monoIconChanged BINDABLE bindableMonoIcon)
	Q_PROPERTY(material READ material WRITE setMaterial NOTIFY materialChanged BINDABLE bindableMaterial)

public:
	explicit FontFamily(QObject *parent = nullptr);

	[[nodiscard]] QString sans() const;
	void setSans(const QString &sans);
	QBindable<QString> bindableSans();
	Q_SIGNAL void sansChanged();

	[[nodiscard]] QString mono() const;
	void setMono(const QString &mono);
	QBindable<QString> bindableMono();
	Q_SIGNAL void monoChanged();

	[[nodiscard]] QString monoIcon() const;
	void setMonoIcon(const QString &monoIcon);
	QBindable<QString> bindableMonoIcon();
	Q_SIGNAL void monoIconChanged();

	[[nodiscard]] QString material() const;
	void setMaterial(const QString &material);
	QBindable<QString> bindableMaterial();
	Q_SIGNAL void materialChanged();

private:
	QProperty<QString> m_sans = "DejaVu Sans";
	QProperty<QString> m_mono = "RobotoMono Nerd Font Propo";
	QProperty<QString> m_monoIcon = "0xProto Nerd Font Mono";
	QProperty<QString> m_material = "Material Symbols Rounded";

};
// END CLASS [[ FontFamily ]]

// BEGIN CLASS [[ AppearanceConfig ]]
class AppearanceConfig : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(FontFamily *fontFamily READ fontFamily CONSTANT)
	Q_PROPERTY(FontSize *fontSize READ fontSize CONSTANT)

public:
	explicit AppearanceConfig(QObject *parent = nullptr);

	[[nodiscard]] FontFamily *fontFamily() const;
	[[nodiscard]] FontSize *fontSize() const;
private:

	FontFamily *m_fontFamily = new FontFamily(this);
	FontSize *m_fontSize = new FontSize(this);
};
// END CLASS [[ AppearanceConfig ]]
} // namespace configs
} // namespace myqmlplugin
