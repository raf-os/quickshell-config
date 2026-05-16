#pragma once 

#include "cserializable.h"

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ BorderConfig ]]
class BorderConfig : public myqmlplugin::configs::CSerializable {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(int thickness READ thickness NOTIFY thicknessChanged BINDABLE bindableThickness RESET resetThickness WRITE setThickness)
	Q_PROPERTY(int rounding READ rounding NOTIFY roundingChanged BINDABLE bindableRounding RESET resetRounding WRITE setRounding)

public:
	explicit BorderConfig(QObject *root = nullptr, QObject *parent = nullptr);

	[[nodiscard]] int thickness() const;
	void setThickness(int value);
	void resetThickness();
	QBindable<int> bindableThickness();
	Q_SIGNAL void thicknessChanged();

	[[nodiscard]] int rounding() const;
	void setRounding(int value);
	void resetRounding();
	QBindable<int> bindableRounding();
	Q_SIGNAL void roundingChanged();

private:
	QProperty<int> m_thickness{8};
	QProperty<int> m_rounding{16};
};
// END CLASS [[ BorderConfig ]]
} // namespace configs
} // namespace myqmlplugin
