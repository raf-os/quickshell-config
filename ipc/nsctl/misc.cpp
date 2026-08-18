#include "misc.h"

#include <stdexcept>

#include <qcontainerfwd.h>

#include "dbus.h"
#include "logging.h"
#include "ns_ipc_client.h"

namespace misc {
static bool misc_toggle_launcher(::org::nightshell::socket *instance) {
  auto response = instance->ToggleLauncher().value();
  if (response) {
    log("ok");
  } else {
    logError("Failed toggling launcher.");
  }
  return 0;
}

static bool misc_open_launcher(::org::nightshell::socket *instance) {
  auto response = instance->OpenLauncher().value();
  if (response) {
    log("ok");
  } else {
    logError("Failed opening launcher.");
  }
  return 0;
}

bool parse_launcher_command(QStringList &args) {
  try {
    auto instance = db::getInterface();

    if (args.isEmpty()) {
      return misc_toggle_launcher(instance);
    }

    auto firstArg = args.takeFirst();
    if (firstArg == "toggle") return misc_toggle_launcher(instance);
    else if (firstArg == "open") return misc_open_launcher(instance);

    logError("Invalid command: {}", firstArg.toUtf8());
    return 1;
  } catch (const std::runtime_error &e) {
    logError("Caught exception: {}", e.what());
    return 1;
  }
}
} // namespace misc
