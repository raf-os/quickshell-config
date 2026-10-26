#pragma once

#include <memory>

#include <qobject.h>
#include <qsocketnotifier.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>

#include "helpermacros.h"

namespace ns::pipewire {
class Core;
class Registry;

class Globals : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY_MOVE(Globals)

  AUTO_MEYERS_SINGLETON_DECL(Globals)

  [[nodiscard]] Core     *core();
  [[nodiscard]] Registry *registry();

private:
  explicit Globals(QObject *parent = nullptr);

  std::unique_ptr<Core>     m_core;
  std::unique_ptr<Registry> m_registry;
};
} // namespace ns::pipewire
