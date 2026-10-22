#pragma once

#include <cstdint>

#include <pipewire/node.h>
#include <qhashfunctions.h>
#include <qmap.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <spa/pod/pod.h>

#include "ns_services_pipewire_export.h"
#include "pw_bindable.h"
#include "pw_common.h"
#include "pw_spa_hook.h"

namespace ns::services::pipewire {
class Device;
class NodeBoundAudio;
class INodeBoundData;

class NS_SERVICES_PIPEWIRE_EXPORT Node : public PwBindableBase {
  Q_OBJECT

  using StringMap = QMap<QString, QString>;

public:
  explicit Node(PwBindableProps props);

  void bindHooks() override;
  void unbindHooks() override;
  void initProps(const spa_dict *props) override;

  [[nodiscard]] bool    shouldUseDevice() const;
  [[nodiscard]] QString name() const;

signals:
  void propertiesChanged();
  void readyChanged();

private slots:
  void onCoreSync(quint32 id, qint32 seq);

private:
  PwBindableInterface<pw_node, PW_TYPE_INTERFACE_Node, PW_VERSION_NODE>
      m_interface{this};

  static void pw_node_on_info(void *data, const struct pw_node_info *info);
  static void pw_node_on_param(void *data, int seq, uint32_t id, uint32_t index,
      uint32_t next, const struct spa_pod *param);
  static const struct pw_node_events NODE_EVENTS;

  NodeType::Flags m_type = NodeType::Untracked;
  QString         m_name;
  QString         m_description;
  QString         m_nick;
  bool            m_isMonitor = false;
  bool            m_proAudio  = false;
  bool            m_ready;
  quint64         m_objectSerial = 0;
  StringMap       m_properties;

  INodeBoundData *m_boundData   = nullptr;
  Device         *m_device      = nullptr;
  qint32          m_routeDevice = -1;
  qint32          m_syncSeq     = 0;

  SpaHook m_listener;

  friend class NodeBoundAudio;
};
} // namespace ns::services::pipewire
