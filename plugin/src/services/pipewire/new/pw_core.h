#pragma once

#include <cstdint>

#include <pipewire/context.h>
#include <pipewire/core.h>
#include <pipewire/loop.h>
#include <qobject.h>
#include <qsocketnotifier.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "pw_spa_hook.h"

namespace ns::pipewire {
class Core : public QObject {
  Q_OBJECT

public:
  explicit Core(QObject *parent = nullptr);
  ~Core() override;
  Q_DISABLE_COPY_MOVE(Core)

  [[nodiscard]] bool isValid() const;
  qint32             sync(qint32 id) const;

signals:
  void polled();
  void synced(quint32 id, qint32 seq);
  void fatalError();

private slots:
  void onPoll();

private:
  struct {
    bool     is_set = false;
    uint32_t id;
    uint32_t cookie;
  } m_coreData;

  static const struct pw_core_events CORE_EVENTS;
  static void on_info(void *data, const struct pw_core_info *info);
  static void on_done(void *data, uint32_t id, int seq);
  static void on_error(
      void *data, uint32_t id, int seq, int res, const char *message);

  pw_core    *m_core    = nullptr;
  pw_context *m_context = nullptr;
  pw_loop    *m_loop    = nullptr;

  SpaHook         m_hook;
  QSocketNotifier m_socketNotifier;

  bool init(bool retry);
  void shutdown();

  friend class Registry;
  friend class Globals;
};
} // namespace ns::pipewire
