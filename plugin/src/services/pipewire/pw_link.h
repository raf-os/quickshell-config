#pragma once

#include <pipewire/link.h>
#include <qdebug.h>
#include <qobject.h>
#include <qproperty.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "pw_bindable.h"
#include "pw_spa_hook.h"

namespace ns::services::pipewire {
class Link : public PwBindableBase {
  Q_OBJECT

public:
  explicit Link(PwBindableProps props);

  enum State : qint8 {
    Error        = PW_LINK_STATE_ERROR,
    Unlinked     = PW_LINK_STATE_UNLINKED,
    Initializing = PW_LINK_STATE_INIT,
    Negotiating  = PW_LINK_STATE_NEGOTIATING,
    Allocating   = PW_LINK_STATE_ALLOCATING,
    Paused       = PW_LINK_STATE_PAUSED,
    Active       = PW_LINK_STATE_ACTIVE
  };
  Q_ENUM(State)

  void bindHooks() override;
  void unbindHooks() override;
  void initProps(const spa_dict *props) override;

  [[nodiscard]] QBindable<State> bindableState() const;
  [[nodiscard]] quint32          outputNode() const;
  [[nodiscard]] quint32          inputNode() const;

  [[nodiscard]] State state() const;

signals:
  void stateChanged();

private:
  static const struct pw_link_events LINK_EVENTS;
  static void link_on_info(void *data, const struct pw_link_info *info);

  PwBindableInterface<pw_link, PW_TYPE_INTERFACE_Link, PW_VERSION_LINK>
      m_interface{this};

  quint32 m_outputNode = 0;
  quint32 m_inputNode  = 0;

  SpaHook m_listener;

  Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(
      Link, Link::State, b_state, State::Unlinked, &Link::stateChanged)
};

QDebug operator<<(QDebug debug, const Link *link);
} // namespace ns::services::pipewire
