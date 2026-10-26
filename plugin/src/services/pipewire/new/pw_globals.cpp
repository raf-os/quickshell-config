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
#include "pw_registry.h"

namespace ns::pipewire {
AUTO_MEYERS_SINGLETON_IMPL(Globals)

Globals::Globals(QObject *parent)
    : QObject(parent), m_core(std::make_unique<Core>(this)),
      m_registry(std::make_unique<Registry>(this)) {
  pw_init(nullptr, nullptr);
}

Core     *Globals::core() { return m_core.get(); }
Registry *Globals::registry() { return m_registry.get(); }
} // namespace ns::pipewire
