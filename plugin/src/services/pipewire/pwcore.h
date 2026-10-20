#pragma once

#include <pipewire/context.h>
#include <pipewire/core.h>
#include <pipewire/loop.h>
#include <qobject.h>
#include <qsocketnotifier.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <spa/utils/hook.h>

#include "ns_services_pipewire_export.h"
#include "pw_spa_hook.h"

namespace ns::services::pipewire {
class NS_SERVICES_PIPEWIRE_EXPORT PwCore : public QObject {
  Q_OBJECT

public:
  explicit PwCore(QObject *parent = nullptr);
  ~PwCore() override;
  Q_DISABLE_COPY_MOVE(PwCore)

  bool init(bool retry);
  void shutdown();

  [[nodiscard]] bool   isValid() const;
  [[nodiscard]] qint32 sync(qint32 id) const;

  void               configure(quint32 id, quint32 cookie);
  [[nodiscard]] bool isConfigured() const;

  [[nodiscard]] quint32 id() const;

signals:
  void polled();
  void synced(quint32 id, qint32 seq);
  void fatalError();

private slots:
  void poll();

private:
  struct {
    bool    isSet = false;
    quint32 id;
    quint32 cookie;
  } m_coreData;

  pw_loop    *m_loop    = nullptr;
  pw_context *m_context = nullptr;
  pw_core    *m_core    = nullptr;

  SpaHook         m_listener;
  QSocketNotifier m_notifier;

  friend class PwRegistry;
};
} // namespace ns::services::pipewire
