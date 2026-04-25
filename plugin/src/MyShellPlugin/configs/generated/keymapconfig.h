#pragma once 

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ KeyMapConfig ]]
class KeyMapConfig : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged BINDABLE bindableEnabled)

public:
	explicit KeyMapConfig(QObject *parent = nullptr);

	[[nodiscard]] bool enabled() const;
	void setEnabled(bool value);
	QBindable<bool> bindableEnabled();
	Q_SIGNAL void enabledChanged();

private:
	QProperty<bool> m_enabled;
};
// END CLASS [[ KeyMapConfig ]]
} // namespace configs
} // namespace myqmlplugin
