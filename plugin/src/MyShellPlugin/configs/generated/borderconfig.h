#pragma once 

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ BorderConfig ]]
class BorderConfig : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(int thickness READ thickness WRITE setThickness NOTIFY thicknessChanged BINDABLE bindableThickness)
	Q_PROPERTY(int rounding READ rounding WRITE setRounding NOTIFY roundingChanged BINDABLE bindableRounding)

public:
	explicit BorderConfig(QObject *parent = nullptr);

	[[nodiscard]] int thickness() const;
	void setThickness(int value);
	QBindable<int> bindableThickness();
	Q_SIGNAL void thicknessChanged();

	[[nodiscard]] int rounding() const;
	void setRounding(int value);
	QBindable<int> bindableRounding();
	Q_SIGNAL void roundingChanged();

private:
	QProperty<int> m_thickness;
	QProperty<int> m_rounding;
};
// END CLASS [[ BorderConfig ]]
} // namespace configs
} // namespace myqmlplugin
