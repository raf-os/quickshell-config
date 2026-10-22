#include "pw_defaults.h"

#include <qobject.h>

#include "pw_metadata.h"
#include "pw_node.h"
#include "pwregistry.h"

namespace ns::services::pipewire {
DefaultTracker::DefaultTracker(PwRegistry *registry) : m_registry(registry) {
  QObject::connect(m_registry, &PwRegistry::metadataAdded, this,
      &DefaultTracker::onMetadataAdded);
  QObject::connect(
      m_registry, &PwRegistry::nodeAdded, this, &DefaultTracker::onNodeAdded);
}

void DefaultTracker::onMetadataAdded(PwMetadata *metadata) {
  if (metadata->name() == "default") {
    if (m_defaultsMetadata.object()) {
      QObject::disconnect(m_defaultsMetadata.object(), nullptr, this, nullptr);
    }

    QObject::connect(metadata, &PwMetadata::propertyChanged, this,
        &DefaultTracker::onMetadataProperty);
    m_defaultsMetadata.setObject(metadata);
  }
}

void DefaultTracker::onMetadataProperty(
    const char *key, const char *type, const char *value) {}

void DefaultTracker::onNodeAdded(Node *node) {
  if (node->name().isEmpty()) return;

  if (m_defaultSink.node == nullptr && node->name() == m_defaultSink.name) {
    //
  }

  if (m_defaultSource.node == nullptr && node->name() == m_defaultSource.name) {
    //
  }
}

DefaultTracker::DefaultNodeProps DefaultTracker::defaultSink() const {
  return m_defaultSink;
}
DefaultTracker::DefaultNodeProps DefaultTracker::defaultSource() const {
  return m_defaultSource;
}
} // namespace ns::services::pipewire
