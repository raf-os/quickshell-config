#include "task_switcher.h"

#include <qcontainerfwd.h>

#include "dbus.h"
#include "logging.h"
#include "ns_ipc_client.h"

namespace tsw {
static int tsw_toggle(::org::nightshell::socket *instance) {
  auto response = instance->ToggleTaskSwitcher().value();
  if (response) log("ok");
  else logError("Error toggling task switcher.");
  return 0;
}

int parse_task_switcher_command(QStringList &args) {
  auto instance = db::getInterface();

  if (args.isEmpty()) {
    return tsw_toggle(instance);
  }

  auto firstArg = args.takeFirst();
  return 1;
}
} // namespace tsw
