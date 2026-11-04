#pragma once

#include <cstdint>
#include <memory>

#include <pipewire/node.h>
#include <qhashfunctions.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <spa/pod/pod.h>
#include <spa/utils/dict.h>

#include "pw_client.h"
#include "pw_common.h"
#include "pw_node_data.h"
#include "pw_spa_hook.h"
#include "pw_trackable.h"

namespace ns::pipewire {
class Device;

namespace NodeState {
Q_NAMESPACE

enum Enum {
  Error     = PW_NODE_STATE_ERROR,
  Creating  = PW_NODE_STATE_CREATING,
  Suspended = PW_NODE_STATE_SUSPENDED,
  Idle      = PW_NODE_STATE_IDLE,
  Running   = PW_NODE_STATE_RUNNING
};

Q_ENUM_NS(Enum)
} // namespace NodeState

class Node : public TrackedObject<pw_node> {
  Q_OBJECT

public:
  explicit Node(const TrackedObjectProps &props, QObject *parent = nullptr);
  ~Node() override;

  [[nodiscard]] INodeData *data();
  [[nodiscard]] bool       ready() const;

  [[nodiscard]] string_map_t properties() const;

  [[nodiscard]] bool    shouldUseDevice() const;
  [[nodiscard]] QString name() const;

  [[nodiscard]] Device *getDevice();
  [[nodiscard]] qint32  routeDevice() const;

  [[nodiscard]] QString clientName() const;
  [[nodiscard]] QString clientBinary() const;
  [[nodiscard]] QString clientIconName() const;

signals:
  void readyChanged();
  void stateChanged();
  void propertiesChanged();
  void clientAppDataChanged();

private slots:
  void onCoreSync(quint32 id, qint32 seq);
  void onDeviceDestroyed();

private:
  SpaHook m_hook;

  NodeState::Enum m_state = NodeState::Error;
  NodeType::Flags m_type  = NodeType::Untracked;
  QString         m_name;
  QString         m_description;
  QString         m_nick;
  bool            m_isMonitor    = false;
  bool            m_proAudio     = false;
  bool            m_ready        = false;
  quint64         m_objectSerial = 0;
  string_map_t    m_properties;

  INodeData *m_data = nullptr;

  std::shared_ptr<Device> m_device = nullptr;
  std::shared_ptr<Client> m_client = nullptr;

  qint32 m_routeDevice = -1;
  qint32 m_syncSeq     = 0;

  static const struct pw_node_events NODE_EVENTS;
  static void on_info(void *data, const struct pw_node_info *info);
  static void on_param(void *data, int seq, uint32_t id, uint32_t index,
      uint32_t next, const struct spa_pod *param);

  friend class PwQMLNode;
  friend class PipewireModel;
};

class TrackableNode
    : public TrackableObject<Node, PW_TYPE_INTERFACE_Node, PW_VERSION_NODE> {
public:
  explicit TrackableNode(quint32 id, quint32 perms)
      : TrackableObject(id, perms), m_id(id) {}

  [[nodiscard]] QString         name() const;
  [[nodiscard]] quint32         id() const;
  [[nodiscard]] NodeType::Flags type() const;

  void initProps(const spa_dict *props) override;

private:
  quint32         m_id;
  NodeType::Flags m_type = NodeType::Untracked;
  QString         m_name;
};
} // namespace ns::pipewire
