#pragma once 

#include "cserializable.h"

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ Sizes ]]
class Sizes : public myqmlplugin::configs::CSerializable {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(int innerHeight READ innerHeight NOTIFY innerHeightChanged BINDABLE bindableInnerHeight RESET resetInnerHeight WRITE setInnerHeight)
	Q_PROPERTY(int trayMenuWidth READ trayMenuWidth NOTIFY trayMenuWidthChanged BINDABLE bindableTrayMenuWidth RESET resetTrayMenuWidth WRITE setTrayMenuWidth)
	Q_PROPERTY(int launcherWidth READ launcherWidth NOTIFY launcherWidthChanged BINDABLE bindableLauncherWidth RESET resetLauncherWidth WRITE setLauncherWidth)
	Q_PROPERTY(int mediaInfoWidth READ mediaInfoWidth NOTIFY mediaInfoWidthChanged BINDABLE bindableMediaInfoWidth RESET resetMediaInfoWidth WRITE setMediaInfoWidth)

public:
	explicit Sizes(QObject *root = nullptr, QObject *parent = nullptr);
	Q_INVOKABLE [[nodiscard]] QString getClassName() const;

	[[nodiscard]] int innerHeight() const;
	void setInnerHeight(int value);
	void resetInnerHeight();
	QBindable<int> bindableInnerHeight();
	Q_SIGNAL void innerHeightChanged();

	[[nodiscard]] int trayMenuWidth() const;
	void setTrayMenuWidth(int value);
	void resetTrayMenuWidth();
	QBindable<int> bindableTrayMenuWidth();
	Q_SIGNAL void trayMenuWidthChanged();

	[[nodiscard]] int launcherWidth() const;
	void setLauncherWidth(int value);
	void resetLauncherWidth();
	QBindable<int> bindableLauncherWidth();
	Q_SIGNAL void launcherWidthChanged();

	[[nodiscard]] int mediaInfoWidth() const;
	void setMediaInfoWidth(int value);
	void resetMediaInfoWidth();
	QBindable<int> bindableMediaInfoWidth();
	Q_SIGNAL void mediaInfoWidthChanged();

private:	
	const QString m_className = "Sizes";

	QProperty<int> m_innerHeight{24};
	QProperty<int> m_trayMenuWidth{200};
	QProperty<int> m_launcherWidth{420};
	QProperty<int> m_mediaInfoWidth{180};
};
// END CLASS [[ Sizes ]]

// BEGIN CLASS [[ BarConfig ]]
class BarConfig : public myqmlplugin::configs::CSerializable {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(myqmlplugin::configs::Sizes *sizes READ sizes CONSTANT)

public:
	explicit BarConfig(QObject *root = nullptr, QObject *parent = nullptr);
	Q_INVOKABLE [[nodiscard]] QString getClassName() const;

	[[nodiscard]] Sizes *sizes() const;

private:	
	const QString m_className = "BarConfig";

	Sizes *m_sizes = new Sizes(this, m_rootObject);
};
// END CLASS [[ BarConfig ]]
} // namespace configs
} // namespace myqmlplugin
