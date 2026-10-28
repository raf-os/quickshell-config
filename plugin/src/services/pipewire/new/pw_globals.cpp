#include "pw_globals.h"

#include <memory>

#include <pipewire/context.h>
#include <pipewire/core.h>
#include <pipewire/loop.h>
#include <pipewire/pipewire.h>
#include <qobject.h>
#include <qsocketnotifier.h>

#include "helpermacros.h"
#include "pw_core.h"
#include "pw_defaults.h"
#include "pw_registry.h"

namespace ns::pipewire {
AUTO_MEYERS_SINGLETON_IMPL(Globals)

Globals::Globals(QObject *parent)
    : QObject(parent), m_core(std::make_unique<Core>(this)),
      m_registry(std::make_unique<Registry>(this)),
      m_defaults(std::make_unique<DefaultTracker>()) {
  pw_init(nullptr, nullptr);
}

Core           *Globals::core() { return m_core.get(); }
Registry       *Globals::registry() { return m_registry.get(); }
DefaultTracker *Globals::defaults() { return m_defaults.get(); }
} // namespace ns::pipewire
