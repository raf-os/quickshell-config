#include "pwcore.h"

#include <cerrno>
#include <cstdint>

#include <pipewire/context.h>
#include <pipewire/core.h>
#include <pipewire/loop.h>
#include <pipewire/pipewire.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qsocketnotifier.h>
#include <qtypes.h>
#include <spa/utils/defs.h>
#include <spa/utils/hook.h>

#include "ns_services_pipewire_debug.h"

static void pw_event_on_info(void *data, const struct pw_core_info *info) {
  auto self = static_cast<ns::services::pipewire::PwCore *>(data);

  self->configure(info->id, info->cookie);
}

static void pw_event_on_done(void *data, uint32_t id, int seq) {
  auto self = static_cast<ns::services::pipewire::PwCore *>(data);
  emit self->synced(id, static_cast<qint32>(seq));
}
static void pw_event_on_error(
    void *data, uint32_t id, int seq, int res, const char *message) {
  auto self = static_cast<ns::services::pipewire::PwCore *>(data);

  if (res == -ENOENT) {
    qCDebug(ns::services::pipewire::NS_LOG_PIPEWIRE)
        << "Pipeware ENOENT on object" << id << "with code" << res << message;
    return;
  }

  qCWarning(ns::services::pipewire::NS_LOG_PIPEWIRE)
      << "Pipeware fatal error on object" << id << "with code" << res
      << message;

  emit self->fatalError();
}

static const struct pw_core_events NS_PW_CORE_EVENTS = {
    .version     = PW_VERSION_CORE_EVENTS,
    .info        = pw_event_on_info,
    .done        = pw_event_on_done,
    .ping        = nullptr,
    .error       = pw_event_on_error,
    .remove_id   = nullptr,
    .bound_id    = nullptr,
    .add_mem     = nullptr,
    .remove_mem  = nullptr,
    .bound_props = nullptr,
};

namespace ns::services::pipewire {
PwCore::PwCore(QObject *parent)
    : QObject(parent), m_notifier(QSocketNotifier::Read) {
  pw_init(nullptr, nullptr);
}

PwCore::~PwCore() {
  qCInfo(NS_LOG_PIPEWIRE) << "Destroying PwCore.";
  shutdown();
}

bool PwCore::init(bool retry) {
  if (m_core) return true;

#define PIPE_EVAL_CREATION(Ptr, Msg)                                           \
  if (!Ptr) {                                                                  \
    if (retry) {                                                               \
      qCInfo(NS_LOG_PIPEWIRE) << Msg;                                          \
    } else {                                                                   \
      qCCritical(NS_LOG_PIPEWIRE) << Msg;                                      \
    }                                                                          \
    shutdown();                                                                \
    return false;                                                              \
  }

  m_loop = pw_loop_new(nullptr);
  PIPE_EVAL_CREATION(m_loop, "Failed to create pipewire event loop.")
  pw_loop_enter(m_loop);

  m_context = pw_context_new(m_loop, nullptr, 0);
  PIPE_EVAL_CREATION(m_context, "Failed to create pipewire context.")

  m_core = pw_context_connect(m_context, nullptr, 0);
  PIPE_EVAL_CREATION(
      m_core, "Failed to connect pipewire context. Errno:" << errno)

#undef PIPE_EVAL_CREATION

  pw_core_add_listener(m_core, &m_listener.hook, &NS_PW_CORE_EVENTS, this);
  auto fd = pw_loop_get_fd(m_loop);
  m_notifier.setSocket(fd);

  QObject::connect(
      &m_notifier, &QSocketNotifier::activated, this, &PwCore::poll);

  m_notifier.setEnabled(true);
  return true;
}

void PwCore::shutdown() {
  if (m_core) {
    m_listener.remove();
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

  m_notifier.setEnabled(false);
  QObject::disconnect(&m_notifier, nullptr, this, nullptr);
}

bool PwCore::isValid() const { return (m_core != nullptr); }

void PwCore::poll() {
  if (!m_loop) return;

  pw_loop_iterate(m_loop, 0);
  emit polled();
}

void PwCore::configure(quint32 id, quint32 cookie) {
  m_coreData = {.isSet = true, .id = id, .cookie = cookie};
}

bool    PwCore::isConfigured() const { return m_coreData.isSet; }
quint32 PwCore::id() const { return m_coreData.id; }

qint32 PwCore::sync(qint32 id) const { return pw_core_sync(m_core, id, 0); }
} // namespace ns::services::pipewire
