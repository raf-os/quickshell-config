#pragma once

#include <qobject.h>
#include <qtmetamacros.h>

#include "pw_bindable.h"
#include "pw_metadata.h"
#include "pw_node.h"
#include "pwregistry.h"

namespace ns::services::pipewire {
class DefaultTracker : public QObject {
  Q_OBJECT

public:
  explicit DefaultTracker(PwRegistry *registry);

  struct DefaultNodeProps {
    Node   *node = nullptr;
    QString name;
  };

  [[nodiscard]] DefaultNodeProps defaultSink() const;
  [[nodiscard]] DefaultNodeProps defaultSource() const;

signals:
  void defaultSinkChanged();
  void defaultSourceChanged();

private slots:
  void onMetadataAdded(PwMetadata *metadata);
  void onMetadataProperty(const char *key, const char *type, const char *value);

  void onNodeAdded(Node *node);

private:
  PwRegistry                      *m_registry;
  PwBindableRefCounted<PwMetadata> m_defaultsMetadata;

  DefaultNodeProps m_defaultSink;
  DefaultNodeProps m_defaultSource;
};
} // namespace ns::services::pipewire
