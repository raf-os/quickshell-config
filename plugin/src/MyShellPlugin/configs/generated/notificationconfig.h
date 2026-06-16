#pragma once 

#include "cserializable.h"

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ NotificationConfig ]]
class NotificationConfig : public myqmlplugin::configs::CSerializable {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(bool enabled READ enabled NOTIFY enabledChanged BINDABLE bindableEnabled RESET resetEnabled WRITE setEnabled)

public:
	explicit NotificationConfig(QObject *root = nullptr, QObject *parent = nullptr);
	Q_INVOKABLE [[nodiscard]] QString getClassName() const;

	[[nodiscard]] bool enabled() const;
	void setEnabled(bool value);
	void resetEnabled();
	QBindable<bool> bindableEnabled();
	Q_SIGNAL void enabledChanged();

private:	
	const QString m_className = "NotificationConfig";

	QProperty<bool> m_enabled{true};
};
// END CLASS [[ NotificationConfig ]]
} // namespace configs
} // namespace myqmlplugin
