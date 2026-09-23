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

ModulesConfig::ModulesConfig(const QString &className, QObject *root, QObject *parent) : SerializableObject(className, root, parent) {}

EnabledModules *ModulesConfig::enabledModules() { return &m_enabledModules; }
} // namespace ns::configs