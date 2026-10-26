#include "pw_core.h"

#include <pipewire/context.h>
#include <pipewire/core.h>
#include <pipewire/loop.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qsocketnotifier.h>
#include <qtypes.h>

#include "ns_pipewire_debug.h"

namespace ns::pipewire {
void Core::on_info(void *data, const struct pw_core_info *info) {
  auto self        = static_cast<Core *>(data);
  self->m_coreData = {.is_set = true, .id = info->id, .cookie = info->cookie};
}

void Core::on_done(void *data, uint32_t id, int seq) {
  auto self = static_cast<Core *>(data);
  emit self->synced(id, static_cast<qint32>(seq));
}

void Core::on_error(
    void *data, uint32_t id, int seq, int res, const char *message) {
  // TODO
}

const struct pw_core_events Core::CORE_EVENTS = {
    .version     = PW_VERSION_CORE_EVENTS,
    .info        = &Core::on_info,
    .done        = &Core::on_done,
    .ping        = nullptr,
    .error       = Core::on_error,
    .remove_id   = nullptr,
    .bound_id    = nullptr,
    .add_mem     = nullptr,
    .remove_mem  = nullptr,
    .bound_props = nullptr,
};

Core::Core(QObject *parent)
    : QObject(parent), m_socketNotifier(QSocketNotifier::Read) {}

Core::~Core() { shutdown(); }

bool   Core::isValid() const { return m_core != nullptr; }
qint32 Core::sync(qint32 id) const {
  if (!m_core) return -1;
  return pw_core_sync(m_core, id, 0);
}

bool Core::init(bool retry) {
  if (m_core) return true;

  auto evalCreation = [retry, this](void *ptr, const QString &msg) {
    if (!ptr) {
      if (retry) qCInfo(NS_LOG_PIPEWIRE) << msg;
      else qCCritical(NS_LOG_PIPEWIRE) << msg;
      shutdown();
      return false;
    }
    return true;
  };

  m_loop = pw_loop_new(nullptr);
  if (!evalCreation(m_loop, "Failed to create pipewire event loop."))
    return false;
  pw_loop_enter(m_loop);

  m_context = pw_context_new(m_loop, nullptr, 0);
  if (!evalCreation(m_context, "Failed to create pipewire context."))
    return false;

  m_core = pw_context_connect(m_context, nullptr, 0);
  if (!evalCreation(m_core,
          QString("Failed to connect pipewire context. Errno: %1").arg(errno)))
    return false;

  pw_core_add_listener(m_core, &m_hook.hook, &Core::CORE_EVENTS, this);
  auto fd = pw_loop_get_fd(m_loop);
  m_socketNotifier.setSocket(fd);

  QObject::connect(
      &m_socketNotifier, &QSocketNotifier::activated, this, &Core::onPoll);
  m_socketNotifier.setEnabled(true);

  return true;
}

void Core::shutdown() {
  if (m_core) {
    m_hook.remove();
    pw_core_disconnect(m_core);
    m_core = nullptr;
  }

  if (m_context) {
    pw_context_destroy(m_context);
    m_context = nullptr;
  }

  if (m_loop) {
    pw_loop_leave(m_loop);
    pw_loop_destroy(m_loop);
    m_loop = nullptr;
  }

  m_socketNotifier.setEnabled(false);
  QObject::disconnect(&m_socketNotifier, nullptr, this, nullptr);
}

void Core::onPoll() {
  if (!m_loop) return;

  pw_loop_iterate(m_loop, 0);
  emit polled();
}
} // namespace ns::pipewire
