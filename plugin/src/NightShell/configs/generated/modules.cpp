#include "modules.h"
#include "serializableobject.h"

namespace ns::configs {
EnabledModules::EnabledModules(const QString &className, QObject *root, QObject *parent) : SerializableObject(className, root, parent) {}

QBindable<bool> EnabledModules::bindableBackground() const { return &b_background; }
void EnabledModules::resetBackground() { b_background = true; }
QBindable<bool> EnabledModules::bindableNotifications() const { return &b_notifications; }
void EnabledModules::resetNotifications() { b_notifications = true; }
QBindable<bool> EnabledModules::bindableGamemode() const { return &b_gamemode; }
void EnabledModules::resetGamemode() { b_gamemode = true; }
QBindable<bool> EnabledModules::bindableWeather() const { return &b_weather; }
void EnabledModules::resetWeather() { b_weather = true; }

Weather::Weather(const QString &className, QObject *root, QObject *parent) : SerializableObject(className, root, parent) {}

QBindable<qreal> Weather::bindableLatitude() const { return &b_latitude; }
void Weather::resetLatitude() { b_latitude = 0.0; }
QBindable<qreal> Weather::bindableLongitude() const { return &b_longitude; }
void Weather::resetLongitude() { b_longitude = 0.0; }

ModulesConfig::ModulesConfig(const QString &className, QObject *root, QObject *parent) : SerializableObject(className, root, parent) {}

EnabledModules *ModulesConfig::enabledModules() { return &m_enabledModules; }
Weather *ModulesConfig::weather() { return &m_weather; }
} // namespace ns::configs