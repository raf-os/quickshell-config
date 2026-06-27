#pragma once

#include "ns_ipc_client.h"
#include <qdbusconnection.h>

namespace db {
namespace {
org::nightshell::socket *interface = nullptr;
}

org::nightshell::socket *getInterface();
} // namespace db
