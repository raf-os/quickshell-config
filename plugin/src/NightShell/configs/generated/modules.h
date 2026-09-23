#pragma once

#include "serializableobject.h"

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qproperty.h>

namespace ns::configs {
class EnabledModules : public utils::SerializableObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(bool background READ default WRITE default NOTIFY backgroundChanged BINDABLE bindableBackground RESET resetBackground)
	Q_PROPERTY(bool notifications READ default WRITE default NOTIFY notificationsChanged BINDABLE bindableNotifications RESET resetNotifications)
	Q_PROPERTY(bool gamemode READ default WRITE default NOTIFY gamemodeChanged BINDABLE bindableGamemode RESET resetGamemode)

public:
	explicit EnabledModules(const QString &className, QObject *root, QObject *parent);
	[[nodiscard]] QBindable<bool> bindableBackground() const;
	void resetBackground();
	[[nodiscard]] QBindable<bool> bindableNotifications() const;
	void resetNotifications();
	[[nodiscard]] QBindable<bool> bindableGamemode() const;
	void resetGamemode();

signals:
	void backgroundChanged();
	void notificationsChanged();
	void gamemodeChanged();

private:
	Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(EnabledModules, bool, b_background, true, &EnabledModules::backgroundChanged)
	Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(EnabledModules, bool, b_notifications, true, &EnabledModules::notificationsChanged)
	Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(EnabledModules, bool, b_gamemode, true, &EnabledModules::gamemodeChanged)
};

class ModulesConfig : public utils::SerializableObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")


	Q_PROPERTY(ns::configs::EnabledModules *enabledModules READ enabledModules CONSTANT)
public:
	explicit ModulesConfig(const QString &className, QObject *root, QObject *parent);
	[[nodiscard]] EnabledModules *enabledModules();

private:
	EnabledModules m_enabledModules{"EnabledModules", this, this};
};
} // namespace ns::configs