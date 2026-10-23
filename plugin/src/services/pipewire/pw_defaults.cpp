#include "pw_defaults.h"

#include <cstring>

#include <qhashfunctions.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <spa/utils/json.h>

#include "ns_services_pipewire_debug.h"
#include "pw_bindable.h"
#include "pw_metadata.h"
#include "pw_node.h"
#include "pwregistry.h"

#define cstr_equal(S1, S2) strcmp(S1, S2) == 0

namespace ns::services::pipewire {
DefaultTracker::DefaultTracker(PwRegistry *registry) : m_registry(registry) {
  QObject::connect(m_registry, &PwRegistry::metadataAdded, this,
      &DefaultTracker::onMetadataAdded);
  QObject::connect(
      m_registry, &PwRegistry::nodeAdded, this, &DefaultTracker::onNodeAdded);
}

void DefaultTracker::reset() {
  if (auto *meta = m_defaultsMetadata.object()) {
    QObject::disconnect(meta, nullptr, this, nullptr);
  }

  m_defaultsMetadata.setObject(nullptr);
  setDefaultSink(nullptr);
  setDefaultSinkName(QString());
  setDefaultSource(nullptr);
  setDefaultSourceName(QString());
  setDefaultConfiguredSink(nullptr);
  setDefaultConfiguredSinkName(QString());
  setDefaultConfiguredSource(nullptr);
  setDefaultConfiguredSourceName(QString());
}

bool DefaultTracker::setConfiguredDefault(
    const char *key, const QString &value) {
  auto *meta = m_defaultsMetadata.object();

  if (!meta || !meta->proxy()) {
    qCCritical(NS_LOG_PIPEWIRE)
        << "Cannot change default node: metadata is not ready";
    return false;
  }

  if (!meta->hasSetPermissions()) {
    qCCritical(NS_LOG_PIPEWIRE)
        << "Unable to change default node: no read/write permissions for"
        << meta;
  }

  if (value.isEmpty()) {
    meta->setProperty(key, "Spa:String:JSON", nullptr);
  } else {
    QJsonObject obj{
        {"name", value}
    };
    auto json = QJsonDocument(obj).toJson(QJsonDocument::Compact);

    meta->setProperty(key, "Spa:String:JSON", json.constData());
  }

  return true;
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
    const char *key, const char *type, const char *value) {
  void (DefaultTracker::*nodeSetter)(Node *)          = nullptr;
  void (DefaultTracker::*nameSetter)(const QString &) = nullptr;

  if (key == nullptr) return;
  else if (cstr_equal(key, "default.audio.sink")) {
    nodeSetter = &DefaultTracker::setDefaultSink;
    nameSetter = &DefaultTracker::setDefaultSinkName;
  } else if (cstr_equal(key, "default.audio.source")) {
    nodeSetter = &DefaultTracker::setDefaultSource;
    nameSetter = &DefaultTracker::setDefaultSourceName;
  } else if (cstr_equal(key, "default.configured.audio.sink")) {
    nodeSetter = &DefaultTracker::setDefaultConfiguredSink;
    nameSetter = &DefaultTracker::setDefaultConfiguredSinkName;
  } else if (cstr_equal(key, "default.configured.audio.source")) {
    nodeSetter = &DefaultTracker::setDefaultConfiguredSource;
    nameSetter = &DefaultTracker::setDefaultConfiguredSourceName;
  }

  QString name;
  if (type && value && cstr_equal(type, "Spa:String:Json")) {
    auto     fail = true;
    spa_json iter[2];
    spa_json_init(&iter[0], value, std::strlen(value));

    if (spa_json_enter_object(&iter[0], &iter[1]) > 0) {
      char buffer[1024];

      if (spa_json_get_string(&iter[1], buffer, sizeof(buffer)) > 0) {
        if (cstr_equal(buffer, "name")) {
          if (spa_json_get_string(&iter[1], buffer, sizeof(buffer)) > 0) {
            name = buffer;
            fail = false;
          }
        }
      }

      if (fail) {
        qCWarning(NS_LOG_PIPEWIRE) << "Failed parsing SPA default json:"
                                   << QString::fromLocal8Bit(value);
      }
    }
  }

  (this->*nameSetter)(name);
  (this->*nodeSetter)(m_registry->findNodeByName(name));
}

void DefaultTracker::onNodeAdded(Node *node) {
  if (node->name().isEmpty()) return;

  if (m_defaultSink.node == nullptr && node->name() == m_defaultSink.name) {
    setDefaultSink(node);
  }
  if (m_defaultSource.node == nullptr && node->name() == m_defaultSource.name) {
    setDefaultSource(node);
  }
  if (m_defaultConfiguredSink.node == nullptr &&
      node->name() == m_defaultConfiguredSink.name)
  {
    setDefaultConfiguredSink(node);
  }
  if (m_defaultConfiguredSource.node == nullptr &&
      node->name() == m_defaultConfiguredSource.name)
  {
    setDefaultConfiguredSource(node);
  }
}

void DefaultTracker::setDefaultNodeGeneric(Node *node, DefaultNodeProps *member,
    destroy_slot_t onDestroySlot, change_signal_t onChangeSignal) {
  if (node == member->node) return;

  if (member->node) {
    // Only disconnect one slot, as this default node might also be the default
    // configured node, and vice versa
    QObject::disconnect(
        member->node, &PwBindableBase::destroying, this, onDestroySlot);
  }

  member->node = node;

  if (node) {
    QObject::connect(node, &PwBindableBase::destroying, this, onDestroySlot);
  }
  emit(this->*onChangeSignal)();
}

void DefaultTracker::setDefaultNodeNameGeneric(const QString &name,
    DefaultNodeProps *member, change_signal_t onChangeSignal) {
  if (name == member->name) return;
  member->name = name;
  emit(this->*onChangeSignal)();
}

// Implementation for fields defined in header
#define AUTO_NODE_IMPLS(Identifier, MemberVariable)                            \
  DefaultTracker::DefaultNodeProps DefaultTracker::default##Identifier()       \
      const {                                                                  \
    return MemberVariable;                                                     \
  }                                                                            \
  void DefaultTracker::setDefault##Identifier(Node *node) {                    \
    return setDefaultNodeGeneric(node, &MemberVariable,                        \
        &DefaultTracker::onDefault##Identifier##Destroyed,                     \
        &DefaultTracker::default##Identifier##Changed);                        \
  }                                                                            \
  void DefaultTracker::setDefault##Identifier##Name(const QString &name) {     \
    return setDefaultNodeNameGeneric(name, &MemberVariable,                    \
        &DefaultTracker::default##Identifier##NameChanged);                    \
  }                                                                            \
  void DefaultTracker::onDefault##Identifier##Destroyed() {                    \
    MemberVariable.node = nullptr;                                             \
    emit default##Identifier##Changed();                                       \
  }

AUTO_NODE_IMPLS(Sink, m_defaultSink)
AUTO_NODE_IMPLS(Source, m_defaultSource)
AUTO_NODE_IMPLS(ConfiguredSink, m_defaultConfiguredSink)
AUTO_NODE_IMPLS(ConfiguredSource, m_defaultConfiguredSource)
#undef AUTO_NODE_IMPLS
} // namespace ns::services::pipewire
