#pragma once 

#include "cserializable.h"

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qstring.h>
#include <qtmetamacros.h>

namespace myqmlplugin{
namespace configs{
// BEGIN CLASS [[ MediaConfig ]]
class MediaConfig : public myqmlplugin::configs::CSerializable {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(bool enabled READ enabled NOTIFY enabledChanged BINDABLE bindableEnabled RESET resetEnabled WRITE setEnabled)
	Q_PROPERTY(bool mediaPopoutEnabled READ mediaPopoutEnabled NOTIFY mediaPopoutEnabledChanged BINDABLE bindableMediaPopoutEnabled RESET resetMediaPopoutEnabled WRITE setMediaPopoutEnabled)
	Q_PROPERTY(QString defaultPlayer READ defaultPlayer NOTIFY defaultPlayerChanged BINDABLE bindableDefaultPlayer RESET resetDefaultPlayer WRITE setDefaultPlayer)

public:
	explicit MediaConfig(QObject *root = nullptr, QObject *parent = nullptr);
	Q_INVOKABLE [[nodiscard]] QString getClassName() const;

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
	const QString m_className = "MediaConfig";

	QProperty<bool> m_enabled{true};
	QProperty<bool> m_mediaPopoutEnabled{true};
	QProperty<QString> m_defaultPlayer{"Spotify"};
};
// END CLASS [[ MediaConfig ]]
} // namespace configs
} // namespace myqmlplugin
