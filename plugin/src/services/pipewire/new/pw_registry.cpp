#include "pw_registry.h"

#include <utility>

#include <pipewire/core.h>
#include <pipewire/device.h>
#include <pipewire/extensions/metadata.h>
#include <pipewire/link.h>
#include <pipewire/node.h>
#include <pipewire/proxy.h>
#include <qobject.h>
#include <qtypes.h>

#include "pw_common.h"
#include "pw_core.h"
#include "pw_device.h"
#include "pw_globals.h"

namespace ns::pipewire {
Registry::Registry(QObject *parent) : QObject(parent) {}

Registry::~Registry() {
  if (m_registry) {
    pw_proxy_destroy(reinterpret_cast<pw_proxy *>(m_registry));
  }
}

const struct pw_registry_events Registry::REGISTRY_EVENTS = {
    .version       = PW_VERSION_REGISTRY_EVENTS,
    .global        = &Registry::on_global,
    .global_remove = &Registry::on_global_remove};

Core *Registry::core() { return Globals::instance()->core(); }

pw_registry *Registry::object() { return m_registry; }

void Registry::init() {
  auto c     = core();
  m_registry = pw_core_get_registry(c->m_core, PW_VERSION_REGISTRY, 0);
  pw_registry_add_listener(
      m_registry, &m_hook.hook, &Registry::REGISTRY_EVENTS, this);

  QObject::connect(c, &Core::synced, &Registry::onCoreSynced);
  m_coreSyncSeq = c->sync(PW_ID_CORE);
}

void Registry::reset() {
  auto c = core();
  if (c) {
    QObject::disconnect(c, nullptr, this, nullptr);
  }

  m_hook.remove();

  if (m_registry) {
    pw_proxy_destroy(reinterpret_cast<pw_proxy *>(m_registry));
    m_registry = nullptr;
  }

  // TODO: clean tracked objects
  for (auto it = m_devices.begin(); it != m_devices.end(); it++) {
    delete it.value();
  }
  m_devices.clear();

  m_initState   = InitState::SendingObjects;
  m_coreSyncSeq = 0;
  emit cleared();
}

void Registry::onCoreSynced(quint32 id, qint32 seq) {
  if (id != PW_ID_CORE || seq != m_coreSyncSeq) return;
  auto c = core();

  switch (m_initState) {
  case InitState::SendingObjects:
    m_coreSyncSeq = c->sync(PW_ID_CORE);
    m_initState   = InitState::Binding;
    break;
  case InitState::Binding:
    m_initState = InitState::Done;
    emit initialized();
    break;
  default: break;
  }
}

void Registry::on_global(void *data, uint32_t id, uint32_t perms,
    const char *type, uint32_t version, const struct spa_dict *props) {
  auto self = static_cast<Registry *>(data);

  if (cstr_equals(type, PW_TYPE_INTERFACE_Metadata)) {
  } else if (cstr_equals(type, PW_TYPE_INTERFACE_Link)) {
  } else if (cstr_equals(type, PW_TYPE_INTERFACE_Node)) {
  } else if (cstr_equals(type, PW_TYPE_INTERFACE_Device)) {
    auto dev = new TrackableDevice(id, perms);
    dev->initProps(props);

    self->m_devices.insert(id, dev);
  }
}

void Registry::on_global_remove(void *data, uint32_t id) {
  auto self = static_cast<Registry *>(data);
}
} // namespace ns::pipewire
