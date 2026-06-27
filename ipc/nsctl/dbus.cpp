#include "dbus.h"
#include "ns_ipc_client.h"

#include <stdexcept>

namespace db {
org::nightshell::socket *getInterface() {
  if (interface == nullptr) {
    auto sbus = QDBusConnection::sessionBus();

    if (!sbus.isConnected()) {
      throw std::runtime_error("Cannot connect to DBus service.");
    }

    interface = new org::nightshell::socket(
        "org.nightshell.socket", "/org/nightshell/socket", sbus);
    return interface;
  } else {
    return interface;
  }
}
} // namespace db
