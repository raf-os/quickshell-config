#pragma once

#include <pipewire/link.h>
#include <qproperty.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "pw_spa_hook.h"
#include "pw_trackable.h"
namespace ns::pipewire {
class Link : public TrackedObject<pw_link> {
  Q_OBJECT

public:
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
  void initProps(const string_map_t &props) override;

  [[nodiscard]] QBindable<Link::State> bindableState() const;
  [[nodiscard]] quint32                outputNode() const;
  [[nodiscard]] quint32                inputNode() const;

signals:
  void stateChanged();

private:
  SpaHook m_hook;

  quint32 m_outputNode = 0;
  quint32 m_inputNode  = 0;

  static const struct pw_link_events LINK_EVENTS;
  static void on_info(void *data, const struct pw_link_info *info);

  Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(
      Link, Link::State, b_state, State::Unlinked, &Link::stateChanged)
};

using TrackableLink =
    TrackableObject<Link, PW_TYPE_INTERFACE_Link, PW_VERSION_LINK>;
} // namespace ns::pipewire
