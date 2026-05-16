#pragma once 

#include "cserializable.h"

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ KeyMapConfig ]]
class KeyMapConfig : public myqmlplugin::configs::CSerializable {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(bool enabled READ enabled NOTIFY enabledChanged BINDABLE bindableEnabled RESET resetEnabled WRITE setEnabled)

public:
	explicit KeyMapConfig(QObject *root = nullptr, QObject *parent = nullptr);

	[[nodiscard]] bool enabled() const;
	void setEnabled(bool value);
	void resetEnabled();
	QBindable<bool> bindableEnabled();
	Q_SIGNAL void enabledChanged();

private:
	QProperty<bool> m_enabled{true};
};
// END CLASS [[ KeyMapConfig ]]
} // namespace configs
} // namespace myqmlplugin
