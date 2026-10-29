#pragma once

#include <cstdint>
#include <memory>

#include <pipewire/node.h>
#include <qhashfunctions.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <spa/pod/pod.h>
#include <spa/utils/dict.h>

#include "pw_common.h"
#include "pw_node_data.h"
#include "pw_spa_hook.h"
#include "pw_trackable.h"

namespace ns::pipewire {
class Device;

class Node : public TrackedObject<pw_node> {
  Q_OBJECT

public:
  explicit Node(const TrackedObjectProps &props, QObject *parent = nullptr)
      : TrackedObject(props, nullptr) {}
  ~Node() override {}

  void bindHooks() override;
  void unbindHooks() override;
  void initProps(const string_map_t &props) override;

  [[nodiscard]] bool    shouldUseDevice() const;
  [[nodiscard]] QString name() const;

  [[nodiscard]] Device *getDevice() const;
  [[nodiscard]] qint32  routeDevice() const;

signals:
  void readyChanged();
  void propertiesChanged();

private slots:
  void onCoreSync(quint32 id, qint32 seq);
  void onDeviceDestroyed();

private:
  SpaHook m_hook;

  NodeType::Flags m_type = NodeType::Untracked;
  QString         m_name;
  QString         m_description;
  QString         m_nick;
  bool            m_isMonitor    = false;
  bool            m_proAudio     = false;
  bool            m_ready        = false;
  quint64         m_objectSerial = 0;
  string_map_t    m_properties;

  INodeData *m_data;

  std::shared_ptr<Device> m_device      = nullptr;
  qint32                  m_routeDevice = -1;
  qint32                  m_syncSeq     = 0;

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
  explicit TrackableNode(quint32 id, quint32 perms, Node *dummy = nullptr)
      : TrackableObject(id, perms, dummy), m_id(id) {}

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
