#include "pw_defaults.h"

#include <array>
#include <cstring>
#include <memory>
#include <utility>

#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qobject.h>
#include <spa/utils/json-core.h>
#include <spa/utils/json.h>

#include "ns_pipewire_debug.h"
#include "pw_common.h"
#include "pw_globals.h"
#include "pw_metadata.h"
#include "pw_node.h"
#include "pw_registry.h"

namespace ns::pipewire {
DefaultTracker::DefaultTracker(QObject *parent) : QObject(parent) {
  return;
  auto reg = Globals::instance()->registry();
  QObject::connect(
      reg, &Registry::metadataAdded, this, &DefaultTracker::onMetadataAdded);
  QObject::connect(
      reg, &Registry::nodeAdded, this, &DefaultTracker::onNodeAdded);
}

void DefaultTracker::reset() {
  if (auto *meta = m_defaultsMetadata.get()) {
    QObject::disconnect(meta, nullptr, this, nullptr);
  }

  m_defaultsMetadata.reset();
  const std::array<Type, 4> types_array{
      {Sink, Source, ConfiguredSink, ConfiguredSource}
  };

  for (const auto &t : types_array) {
    setDefaultName(t, QString());
    setDefaultNode(t, nullptr);
  }
}

bool DefaultTracker::setConfiguredDefault(
    const char *key, const QString &value) {
  auto meta = m_defaultsMetadata;
  if (!meta || !meta->proxy()) {
    qCCritical(NS_LOG_PIPEWIRE)
        << "Cannot change default node: metadata is not ready";
    return false;
  }

  if (!meta->hasSetPermissions()) {
    qCCritical(NS_LOG_PIPEWIRE)
        << "Unable to change default node: no read/write permissions for"
        << meta.get();
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

void DefaultTracker::onMetadataAdded(TrackableMetadata *metadata) {
  if (metadata->name() == "default") {
    if (m_defaultsMetadata) {
      QObject::disconnect(m_defaultsMetadata.get(), nullptr, this, nullptr);
    }

    m_defaultsMetadata = metadata->get();
    QObject::connect(m_defaultsMetadata.get(), &Metadata::propertyChanged, this,
        &DefaultTracker::onMetadataProperty);
    QObject::connect(m_defaultsMetadata.get(), &QObject::destroyed, this,
        [this] { m_defaultsMetadata = nullptr; });
  }
}

void DefaultTracker::onNodeAdded(TrackableNode *node) {
  if (node->name().isEmpty()) return;

  if (!m_defaults.sink.node && node->name() == m_defaults.sink.name) {
    setDefaultNode(Type::Sink, node->get());
  }

  if (!m_defaults.source.node && node->name() == m_defaults.source.name) {
    setDefaultNode(Type::Source, node->get());
  }

  if (!m_defaults.configuredSink.node &&
      node->name() == m_defaults.configuredSink.name)
  {
    setDefaultNode(Type::ConfiguredSink, node->get());
  }

  if (!m_defaults.configuredSource.node &&
      node->name() == m_defaults.configuredSource.name)
  {
    setDefaultNode(Type::ConfiguredSource, node->get());
  }
}

void DefaultTracker::onMetadataProperty(
    const char *key, const char *type, const char *value) {
  Type t;
  if (key == nullptr) return;
  else if (cstr_equals(key, "default.audio.sink")) {
    t = Sink;
  } else if (cstr_equals(key, "default.audio.source")) {
    t = Source;
  } else if (cstr_equals(key, "default.configured.audio.sink")) {
    t = ConfiguredSink;
  } else if (cstr_equals(key, "default.configured.audio.source")) {
    t = ConfiguredSource;
  } else {
    return;
  }

  QString name;
  if (type && value && cstr_equals(type, "Spa:String:JSON")) {
    auto     fail = true;
    spa_json iter[2];
    spa_json_init(&iter[0], value, std::strlen(value));

    if (spa_json_enter_object(&iter[0], &iter[1]) > 0) {
      char buffer[1024];
      if (spa_json_get_string(&iter[1], buffer, sizeof(buffer)) > 0) {
        if (cstr_equals(buffer, "name")) {
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

  auto p = getDefaultProps(t);
  if (p) {
    auto reg = Globals::instance()->registry();
    p->name  = name;
    p->node  = reg->findNodeByName(name);
  }
}

DefaultTracker::DefaultNodeProps *DefaultTracker::getDefaultProps(Type type) {
  switch (type) {
  case Sink:             return &m_defaults.sink;
  case Source:           return &m_defaults.source;
  case ConfiguredSink:   return &m_defaults.configuredSink;
  case ConfiguredSource: return &m_defaults.configuredSource;
  default:               return nullptr;
  }
}

const DefaultTracker::DefaultNodeProps *DefaultTracker::getDefaultProps(
    Type type) const {
  return getDefaultProps(type);
}

Node *DefaultTracker::getNode(Type type) {
  auto p = getDefaultProps(type);
  if (p) return p->node.get();
  return nullptr;
}

std::shared_ptr<Node> DefaultTracker::getNodeSharedPtr(Type type) {
  auto p = getDefaultProps(type);
  if (p) return p->node;
  return nullptr;
}

QString DefaultTracker::getNodeName(Type type) const {
  auto p = getDefaultProps(type);
  if (p) return p->name;
  return {};
}

void DefaultTracker::setDefaultNode(Type type, std::shared_ptr<Node> node) {
  auto p = getDefaultProps(type);
  if (!p) return;

  if (p->node == node) return;
  if (p->onDestroyConnection) {
    QObject::disconnect(p->onDestroyConnection);
  }

  if (node) {
    p->node                = node;
    p->onDestroyConnection = QObject::connect(node.get(), &QObject::destroyed,
        this, [type, this] { onDefaultNodeDestroyed(type); });
  } else {
    p->node = nullptr;
  }
  emit defaultNodeChanged(type);
}

void DefaultTracker::setDefaultName(Type type, const QString &name) {
  auto p = getDefaultProps(type);
  if (!p) return;

  if (p->name == name) return;

  p->name = name;
  emit defaultNodeNameChanged(type);
}

void DefaultTracker::onDefaultNodeDestroyed(DefaultTracker::Type type) {
  auto p = getDefaultProps(type);
  if (!p) return;

  p->name = "";
  p->node = nullptr;
  QObject::disconnect(p->onDestroyConnection);

  emit defaultNodeChanged(type);
  emit defaultNodeNameChanged(type);
}
} // namespace ns::pipewire
