#pragma once 

#include <qobject.h>
#include <qproperty.h>
#include <qtmetamacros.h>
#include <qqmlintegration.h>
#include <qstring.h>
#include <qtypes.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ FontSize ]]
class FontSize : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged BINDABLE bindableScale)
	Q_PROPERTY(int xxs READ xxs WRITE setXxs NOTIFY xxsChanged BINDABLE bindableXxs)

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

private:
	QProperty<qreal> m_scale;
	QProperty<int> m_xxs;

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
