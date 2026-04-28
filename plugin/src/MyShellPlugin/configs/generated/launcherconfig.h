#pragma once 

#include <qlist.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qstring.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ LauncherConfig ]]
class LauncherConfig : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged RESET resetEnabled BINDABLE bindableEnabled)
	Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged RESET resetWidth BINDABLE bindableWidth)
	Q_PROPERTY(QString commandPrefix READ commandPrefix WRITE setCommandPrefix NOTIFY commandPrefixChanged RESET resetCommandPrefix BINDABLE bindableCommandPrefix)
	Q_PROPERTY(QList<QString> favoriteApps READ favoriteApps WRITE setFavoriteApps NOTIFY favoriteAppsChanged)

public:
	explicit LauncherConfig(QObject *parent = nullptr);

	[[nodiscard]] bool enabled() const;
	void setEnabled(bool value);
	void resetEnabled();
	QBindable<bool> bindableEnabled();
	Q_SIGNAL void enabledChanged();

	[[nodiscard]] int width() const;
	void setWidth(int value);
	void resetWidth();
	QBindable<int> bindableWidth();
	Q_SIGNAL void widthChanged();

	[[nodiscard]] QString commandPrefix() const;
	void setCommandPrefix(const QString &value);
	void resetCommandPrefix();
	QBindable<QString> bindableCommandPrefix();
	Q_SIGNAL void commandPrefixChanged();

	[[nodiscard]] QList<QString> &favoriteApps();
	void setFavoriteApps(const QList<QString> &newList);
	Q_SIGNAL void favoriteAppsChanged();

private:
	QProperty<bool> m_enabled{true};
	QProperty<int> m_width{420};
	QProperty<QString> m_commandPrefix{"/"};
	mutable QList<QString> m_favoriteApps = {};
};
// END CLASS [[ LauncherConfig ]]
} // namespace configs
} // namespace myqmlplugin
