#pragma once

#include "serializableobject.h"

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qproperty.h>
#include <qtypes.h>
#include <qstring.h>

namespace ns::configs {
class EnabledModules : public utils::SerializableObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(bool background READ default WRITE default NOTIFY backgroundChanged BINDABLE bindableBackground RESET resetBackground)
	Q_PROPERTY(bool notifications READ default WRITE default NOTIFY notificationsChanged BINDABLE bindableNotifications RESET resetNotifications)
	Q_PROPERTY(bool gamemode READ default WRITE default NOTIFY gamemodeChanged BINDABLE bindableGamemode RESET resetGamemode)
	Q_PROPERTY(bool weather READ default WRITE default NOTIFY weatherChanged BINDABLE bindableWeather RESET resetWeather)
	Q_PROPERTY(bool mpris READ default WRITE default NOTIFY mprisChanged BINDABLE bindableMpris RESET resetMpris)

public:
	explicit EnabledModules(const QString &className, QObject *root, QObject *parent);
	[[nodiscard]] QBindable<bool> bindableBackground() const;
	void resetBackground();
	[[nodiscard]] QBindable<bool> bindableNotifications() const;
	void resetNotifications();
	[[nodiscard]] QBindable<bool> bindableGamemode() const;
	void resetGamemode();
	[[nodiscard]] QBindable<bool> bindableWeather() const;
	void resetWeather();
	[[nodiscard]] QBindable<bool> bindableMpris() const;
	void resetMpris();

signals:
	void backgroundChanged();
	void notificationsChanged();
	void gamemodeChanged();
	void weatherChanged();
	void mprisChanged();

private:
	Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(EnabledModules, bool, b_background, true, &EnabledModules::backgroundChanged)
	Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(EnabledModules, bool, b_notifications, true, &EnabledModules::notificationsChanged)
	Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(EnabledModules, bool, b_gamemode, true, &EnabledModules::gamemodeChanged)
	Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(EnabledModules, bool, b_weather, true, &EnabledModules::weatherChanged)
	Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(EnabledModules, bool, b_mpris, true, &EnabledModules::mprisChanged)
};

class Weather : public utils::SerializableObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(qreal latitude READ default WRITE default NOTIFY latitudeChanged BINDABLE bindableLatitude RESET resetLatitude)
	Q_PROPERTY(qreal longitude READ default WRITE default NOTIFY longitudeChanged BINDABLE bindableLongitude RESET resetLongitude)

public:
	explicit Weather(const QString &className, QObject *root, QObject *parent);
	[[nodiscard]] QBindable<qreal> bindableLatitude() const;
	void resetLatitude();
	[[nodiscard]] QBindable<qreal> bindableLongitude() const;
	void resetLongitude();

signals:
	void latitudeChanged();
	void longitudeChanged();

private:
	Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Weather, qreal, b_latitude, 0.0, &Weather::latitudeChanged)
	Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Weather, qreal, b_longitude, 0.0, &Weather::longitudeChanged)
};

class Mpris : public utils::SerializableObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(QString position READ default WRITE default NOTIFY positionChanged BINDABLE bindablePosition RESET resetPosition)

public:
	explicit Mpris(const QString &className, QObject *root, QObject *parent);
	[[nodiscard]] QBindable<QString> bindablePosition() const;
	void resetPosition();

signals:
	void positionChanged();

private:
	Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Mpris, QString, b_position, "BottomLeft", &Mpris::positionChanged)
};

class ModulesConfig : public utils::SerializableObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")


	Q_PROPERTY(ns::configs::EnabledModules *enabledModules READ enabledModules CONSTANT)
	Q_PROPERTY(ns::configs::Weather *weather READ weather CONSTANT)
	Q_PROPERTY(ns::configs::Mpris *mpris READ mpris CONSTANT)
public:
	explicit ModulesConfig(const QString &className, QObject *root, QObject *parent);
	[[nodiscard]] EnabledModules *enabledModules();
	[[nodiscard]] Weather *weather();
	[[nodiscard]] Mpris *mpris();

private:
	EnabledModules m_enabledModules{"EnabledModules", this, this};
	Weather m_weather{"Weather", this, this};
	Mpris m_mpris{"Mpris", this, this};
};
} // namespace ns::configs