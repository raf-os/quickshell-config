#pragma once 

#include <qobject.h>
#include <qproperty.h>
#include <qstring.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ MediaConfig ]]
class MediaConfig : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged BINDABLE bindableEnabled)
	Q_PROPERTY(bool mediaPopoutEnabled READ mediaPopoutEnabled WRITE setMediaPopoutEnabled NOTIFY mediaPopoutEnabledChanged BINDABLE bindableMediaPopoutEnabled)
	Q_PROPERTY(QString defaultPlayer READ defaultPlayer WRITE setDefaultPlayer NOTIFY defaultPlayerChanged BINDABLE bindableDefaultPlayer)

public:
	explicit MediaConfig(QObject *parent = nullptr);

	[[nodiscard]] bool enabled() const;
	void setEnabled(bool value);
	QBindable<bool> bindableEnabled();
	Q_SIGNAL void enabledChanged();

	[[nodiscard]] bool mediaPopoutEnabled() const;
	void setMediaPopoutEnabled(bool value);
	QBindable<bool> bindableMediaPopoutEnabled();
	Q_SIGNAL void mediaPopoutEnabledChanged();

	[[nodiscard]] QString defaultPlayer() const;
	void setDefaultPlayer(const QString &value);
	QBindable<QString> bindableDefaultPlayer();
	Q_SIGNAL void defaultPlayerChanged();

private:
	QProperty<bool> m_enabled;
	QProperty<bool> m_mediaPopoutEnabled;
	QProperty<QString> m_defaultPlayer;
};
// END CLASS [[ MediaConfig ]]
} // namespace configs
} // namespace myqmlplugin
