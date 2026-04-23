#pragma once 

#include <qtmetamacros.h>
#include <qobject.h>
#include <qstring.h>
#include <qqmlintegration.h>
#include <qbindable.h>

namespace myqmlplugin{
namespace configs{
class AppearanceConfig : public QObject {
Q_OBJECT
QML_ELEMENT
QML_UNCREATABLE("")


public:
	explicit AppearanceConfig(QObject *parent = nullptr);


private:
	auto m_FontFamily = FontFamily(this);

};

class FontFamily : public QObject {
Q_OBJECT
QML_ELEMENT
QML_UNCREATABLE("")

Q_PROPERTY(sans READ sans WRITE setSans NOTIFY sansChanged )
Q_PROPERTY(mono READ mono WRITE setMono NOTIFY monoChanged )
Q_PROPERTY(monoIcon READ monoIcon WRITE setMonoIcon NOTIFY monoIconChanged )
Q_PROPERTY(material READ material WRITE setMaterial NOTIFY materialChanged )
Q_PROPERTY(test READ test WRITE setTest NOTIFY testChanged )

public:
	explicit FontFamily(QObject *parent = nullptr);


	[[nodiscard]] QString sans() const;
	void setSans(const QString &sans);
	Q_SIGNAL void sansChanged();

	[[nodiscard]] QString mono() const;
	void setMono(const QString &mono);
	Q_SIGNAL void monoChanged();

	[[nodiscard]] QString monoIcon() const;
	void setMonoIcon(const QString &monoIcon);
	Q_SIGNAL void monoIconChanged();

	[[nodiscard]] QString material() const;
	void setMaterial(const QString &material);
	Q_SIGNAL void materialChanged();

	[[nodiscard]] QString test() const;
	void setTest(const QString &test);
	Q_SIGNAL void testChanged();

private:
	QProperty<QString> sans = "DejaVu Sans";
	QProperty<QString> mono = "RobotoMono Nerd Font Propo";
	QProperty<QString> monoIcon = "0xProto Nerd Font Mono";
	QProperty<QString> material = "Material Symbols Rounded";
	QProperty<QString> test;

};
} // namespace configs
} // namespace myqmlplugin
