#pragma once 

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qstring.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ MediaConfig ]]
class MediaConfig : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged RESET resetEnabled BINDABLE bindableEnabled)
	Q_PROPERTY(bool mediaPopoutEnabled READ mediaPopoutEnabled WRITE setMediaPopoutEnabled NOTIFY mediaPopoutEnabledChanged RESET resetMediaPopoutEnabled BINDABLE bindableMediaPopoutEnabled)
	Q_PROPERTY(QString defaultPlayer READ defaultPlayer WRITE setDefaultPlayer NOTIFY defaultPlayerChanged RESET resetDefaultPlayer BINDABLE bindableDefaultPlayer)

public:
	explicit MediaConfig(QObject *parent = nullptr);

	[[nodiscard]] bool enabled() const;
	void setEnabled(bool value);
	void resetEnabled();
	QBindable<bool> bindableEnabled();
	Q_SIGNAL void enabledChanged();

	[[nodiscard]] bool mediaPopoutEnabled() const;
	void setMediaPopoutEnabled(bool value);
	void resetMediaPopoutEnabled();
	QBindable<bool> bindableMediaPopoutEnabled();
	Q_SIGNAL void mediaPopoutEnabledChanged();

	[[nodiscard]] QString defaultPlayer() const;
	void setDefaultPlayer(const QString &value);
	void resetDefaultPlayer();
	QBindable<QString> bindableDefaultPlayer();
	Q_SIGNAL void defaultPlayerChanged();

private:
	QProperty<bool> m_enabled{true};
	QProperty<bool> m_mediaPopoutEnabled{true};
	QProperty<QString> m_defaultPlayer{"Spotify"};
};
// END CLASS [[ MediaConfig ]]
} // namespace configs
} // namespace myqmlplugin
