#pragma once 

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ Sizes ]]
class Sizes : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(int innerHeight READ innerHeight WRITE setInnerHeight NOTIFY innerHeightChanged BINDABLE bindableInnerHeight)
	Q_PROPERTY(int trayMenuWidth READ trayMenuWidth WRITE setTrayMenuWidth NOTIFY trayMenuWidthChanged BINDABLE bindableTrayMenuWidth)
	Q_PROPERTY(int launcherWidth READ launcherWidth WRITE setLauncherWidth NOTIFY launcherWidthChanged BINDABLE bindableLauncherWidth)
	Q_PROPERTY(int mediaInfoWidth READ mediaInfoWidth WRITE setMediaInfoWidth NOTIFY mediaInfoWidthChanged BINDABLE bindableMediaInfoWidth)

public:
	explicit Sizes(QObject *parent = nullptr);

	[[nodiscard]] int innerHeight() const;
	void setInnerHeight(int value);
	QBindable<int> bindableInnerHeight();
	Q_SIGNAL void innerHeightChanged();

	[[nodiscard]] int trayMenuWidth() const;
	void setTrayMenuWidth(int value);
	QBindable<int> bindableTrayMenuWidth();
	Q_SIGNAL void trayMenuWidthChanged();

	[[nodiscard]] int launcherWidth() const;
	void setLauncherWidth(int value);
	QBindable<int> bindableLauncherWidth();
	Q_SIGNAL void launcherWidthChanged();

	[[nodiscard]] int mediaInfoWidth() const;
	void setMediaInfoWidth(int value);
	QBindable<int> bindableMediaInfoWidth();
	Q_SIGNAL void mediaInfoWidthChanged();

private:
	QProperty<int> m_innerHeight;
	QProperty<int> m_trayMenuWidth;
	QProperty<int> m_launcherWidth;
	QProperty<int> m_mediaInfoWidth;
};
// END CLASS [[ Sizes ]]

// BEGIN CLASS [[ BarConfig ]]
class BarConfig : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(Sizes *sizes READ sizes CONSTANT)

public:
	explicit BarConfig(QObject *parent = nullptr);

	[[nodiscard]] Sizes *sizes() const;

private:
	Sizes *m_sizes = new Sizes(this);
};
// END CLASS [[ BarConfig ]]
} // namespace configs
} // namespace myqmlplugin
