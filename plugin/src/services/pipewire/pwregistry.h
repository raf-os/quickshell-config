#pragma once

#include <pipewire/core.h>
#include <qhash.h>
#include <qobject.h>
#include <qstringview.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "pw_metadata.h"
#include "pw_spa_hook.h"

namespace ns::services::pipewire {
class PwCore;
class PwBindableBase;
class Device;
class Node;
class Link;

class PwRegistry : public QObject {
  Q_OBJECT

public:
  explicit PwRegistry(
      pw_registry *registry = nullptr, QObject *parent = nullptr);
  ~PwRegistry() override;
  Q_DISABLE_COPY_MOVE(PwRegistry)

  using BindableMap = QHash<quint32, PwBindableBase *>;

  [[nodiscard]] pw_registry *registry();
  [[nodiscard]] Node        *findNodeByName(QStringView name) const;

  void init(PwCore &core);
  void reset();

  [[nodiscard]] PwCore *core();

  QHash<quint32, PwMetadata *> m_metadata;
  QHash<quint32, Node *>       m_nodes;
  QHash<quint32, Device *>     m_devices;
  QHash<quint32, Link *>       m_links;

signals:
  void initialized();
  void cleared();
  void metadataAdded(PwMetadata *metadata);
  void nodeAdded(Node *node);
  void linkAdded(Link *link);

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
