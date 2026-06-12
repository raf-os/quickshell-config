#pragma once 

#include "cserializable.h"

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qstring.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ SoundsConfig ]]
class SoundsConfig : public myqmlplugin::configs::CSerializable {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(bool enabled READ enabled NOTIFY enabledChanged BINDABLE bindableEnabled RESET resetEnabled WRITE setEnabled)
	Q_PROPERTY(QString theme READ theme NOTIFY themeChanged BINDABLE bindableTheme RESET resetTheme WRITE setTheme)

public:
	explicit SoundsConfig(QObject *root = nullptr, QObject *parent = nullptr);
	Q_INVOKABLE [[nodiscard]] QString getClassName() const;

	[[nodiscard]] bool enabled() const;
	void setEnabled(bool value);
	void resetEnabled();
	QBindable<bool> bindableEnabled();
	Q_SIGNAL void enabledChanged();

	[[nodiscard]] QString theme() const;
	void setTheme(const QString &value);
	void resetTheme();
	QBindable<QString> bindableTheme();
	Q_SIGNAL void themeChanged();

private:	
	const QString m_className = "SoundsConfig";

	QProperty<bool> m_enabled{true};
	QProperty<QString> m_theme{"freedesktop"};
};
// END CLASS [[ SoundsConfig ]]
} // namespace configs
} // namespace myqmlplugin
