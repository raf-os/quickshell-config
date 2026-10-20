#pragma once

#include <pipewire/core.h>
#include <qhash.h>
#include <qobject.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "ns_services_pipewire_export.h"
#include "pw_metadata.h"
#include "pw_spa_hook.h"

namespace ns::services::pipewire {
class PwCore;
class PwBindableBase;

class NS_SERVICES_PIPEWIRE_EXPORT PwRegistry : public QObject {
  Q_OBJECT

public:
  explicit PwRegistry(
      pw_registry *registry = nullptr, QObject *parent = nullptr);
  ~PwRegistry() override;
  Q_DISABLE_COPY_MOVE(PwRegistry)

  using BindableMap = QHash<quint32, PwBindableBase *>;

  [[nodiscard]] pw_registry *registry();

  void init(PwCore &core);
  void reset();

  [[nodiscard]] PwCore *core();

  QHash<quint32, PwMetadata *> m_metadata;

signals:
  void initialized();
  void metadataAdded(PwMetadata *metadata);

private slots:
  void onCoreSync(quint32 id, qint32 seq);

private:
  PwCore *m_core        = nullptr;
  qint32  m_coreSyncSeq = 0;

  enum class InitState : quint8 {
    SendingObjects,
    Binding,
    Done
  } m_initState = InitState::SendingObjects;

  pw_registry *m_registry;
  SpaHook      m_listener;
};
} // namespace ns::services::pipewire
