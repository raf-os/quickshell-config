#pragma once

#include <pipewire/node.h>
#include <qhashfunctions.h>
#include <qmap.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "ns_services_pipewire_export.h"
#include "pw_bindable.h"
#include "pw_common.h"
#include "pw_spa_hook.h"

namespace ns::services::pipewire {
class Device;

class NS_SERVICES_PIPEWIRE_EXPORT Node : public PwBindableBase {
  Q_OBJECT

  using StringMap = QMap<QString, QString>;

public:
  explicit Node(PwBindableProps props);

  void bindHooks() override;
  void unbindHooks() override;
  void initProps(const spa_dict *props) override;

private:
  PwBindableInterface<pw_node, PW_TYPE_INTERFACE_Node, PW_VERSION_NODE>
      m_interface{this};

  NodeType::Flags m_type = NodeType::Untracked;
  QString         m_name;
  QString         m_description;
  QString         m_nick;
  bool            m_isMonitor = false;
  StringMap       properties;

  Device *m_device      = nullptr;
  qint32  m_routeDevice = -1;

  SpaHook m_listener;
};
} // namespace ns::services::pipewire
