#pragma once

#include <cstdint>
#include <memory>

#include <pipewire/core.h>
#include <pipewire/device.h>
#include <qhash.h>
#include <qobject.h>
#include <qstringview.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <spa/utils/dict.h>

#include "pw_core.h"
#include "pw_device.h"
#include "pw_link.h"
#include "pw_metadata.h"
#include "pw_node.h"
#include "pw_spa_hook.h"
#include "pw_trackable.h"

namespace ns::pipewire {
class Registry : public QObject {
  Q_OBJECT

public:
  explicit Registry(QObject *parent = nullptr);
  ~Registry() override;
  Q_DISABLE_COPY_MOVE(Registry)

  void init();
  void reset();

  [[nodiscard]] Core        *core();
  [[nodiscard]] pw_registry *object();

  [[nodiscard]] std::shared_ptr<Node>   findNodeByName(QStringView name);
  [[nodiscard]] std::shared_ptr<Node>   getNodeById(quint32 id);
  [[nodiscard]] std::shared_ptr<Device> getDeviceById(quint32 id);

signals:
  void cleared();
  void initialized();
  void metadataAdded(TrackableMetadata *metadata);
  void nodeAdded(TrackableNode *node);
  void linkAdded(TrackableLink *link);

private slots:
  void onCoreSynced(quint32 id, qint32 seq);

private:
  SpaHook m_hook;
  qint32  m_coreSyncSeq = 0;

  static const struct pw_registry_events REGISTRY_EVENTS;
  static void on_global(void *data, uint32_t id, uint32_t perms,
      const char *type, uint32_t version, const struct spa_dict *props);
  static void on_global_remove(void *data, uint32_t id);

  pw_registry *m_registry = nullptr;

  QHash<quint32, TrackableMetadata *> m_metadata;
  QHash<quint32, TrackableDevice *>   m_devices;
  QHash<quint32, TrackableNode *>     m_nodes;
  QHash<quint32, TrackableLink *>     m_links;

  enum class InitState : quint8 {
    SendingObjects,
    Binding,
    Done
  } m_initState = InitState::SendingObjects;

  friend class PipewireModel;
};
} // namespace ns::pipewire
