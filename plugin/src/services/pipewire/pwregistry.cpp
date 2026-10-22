#include "pwregistry.h"

#include <cstdint>
#include <cstring>

#include <pipewire/core.h>
#include <pipewire/device.h>
#include <pipewire/extensions/metadata.h>
#include <pipewire/link.h>
#include <pipewire/node.h>
#include <pipewire/proxy.h>
#include <qobject.h>
#include <qtypes.h>
#include <spa/utils/dict.h>

#include "pw_device.h"
#include "pw_metadata.h"
#include "pw_node.h"
#include "pwcore.h"

static void pw_registry_on_global(void *data, uint32_t id, uint32_t permissions,
    const char *type, uint32_t version, const struct spa_dict *props) {
  auto self = static_cast<ns::services::pipewire::PwRegistry *>(data);

  if (strcmp(type, PW_TYPE_INTERFACE_Metadata) == 0) {
    auto meta = new ns::services::pipewire::PwMetadata({self, id, permissions});
    meta->initProps(props);

    if (self->m_metadata.contains(id)) {
      auto v = self->m_metadata.value(id);
      v->kill();
    }
    self->m_metadata.emplace(id, meta);
    emit self->metadataAdded(meta);
  } else if (strcmp(type, PW_TYPE_INTERFACE_Link) == 0) {
  } else if (strcmp(type, PW_TYPE_INTERFACE_Node) == 0) {
    auto node = new ns::services::pipewire::Node({self, id, permissions});
    node->initProps(props);

    self->m_nodes.emplace(id, node);
    emit self->nodeAdded(node);
  } else if (strcmp(type, PW_TYPE_INTERFACE_Device) == 0) {
    auto device = new ns::services::pipewire::Device({self, id, permissions});
    device->initProps(props);

    self->m_devices.emplace(id, device);
  }
}

static void pw_registry_on_global_removed(void *data, uint32_t id) {
  auto self = static_cast<ns::services::pipewire::PwRegistry *>(data);
}

static const struct pw_registry_events NS_PW_REGISTRY_EVENTS = {
    .version       = PW_VERSION_REGISTRY_EVENTS,
    .global        = pw_registry_on_global,
    .global_remove = pw_registry_on_global_removed};

namespace ns::services::pipewire {
PwRegistry::PwRegistry(pw_registry *registry, QObject *parent)
    : QObject(parent), m_registry(registry) {}

PwRegistry::~PwRegistry() {
  if (m_core && m_registry) pw_registry_destroy(m_registry, m_core->id());
  else if (m_registry)
    pw_proxy_destroy(reinterpret_cast<pw_proxy *>(m_registry));
}

pw_registry *PwRegistry::registry() { return m_registry; }

void PwRegistry::init(PwCore &core) {
  m_core     = &core;
  m_registry = pw_core_get_registry(core.m_core, PW_VERSION_REGISTRY, 0);

  pw_registry_add_listener(
      m_registry, &m_listener.hook, &NS_PW_REGISTRY_EVENTS, 0);

  QObject::connect(m_core, &PwCore::synced, this, &PwRegistry::onCoreSync);
  m_coreSyncSeq = m_core->sync(PW_ID_CORE);
}

void PwRegistry::reset() {
  if (m_core) {
    QObject::disconnect(m_core, nullptr, this, nullptr);
  }

  m_listener.remove();

  if (m_registry) {
    pw_proxy_destroy(reinterpret_cast<pw_proxy *>(m_registry));
    m_registry = nullptr;
  }

  for (auto it = m_metadata.begin(); it != m_metadata.end(); it++) {
    it.value()->kill();
  }
  m_metadata.clear();

  for (auto it = m_nodes.begin(); it != m_nodes.end(); it++) {
    it.value()->kill();
  }
  m_nodes.clear();

  for (auto it = m_devices.begin(); it != m_devices.end(); it++) {
    it.value()->kill();
  }
  m_devices.clear();
}

PwCore *PwRegistry::core() { return m_core; }

void PwRegistry::onCoreSync(quint32 id, qint32 seq) {
  if (id != PW_ID_CORE || seq != m_coreSyncSeq) return;

  switch (m_initState) {
  case InitState::SendingObjects:
    m_coreSyncSeq = m_core->sync(PW_ID_CORE);
    m_initState   = InitState::Binding;
    break;
  case InitState::Binding:
    m_initState = InitState::Done;
    emit initialized();
    break;
  default: break;
  }
}
} // namespace ns::services::pipewire
