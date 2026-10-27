#include "pw_defaults.h"

#include <cstring>

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
DefaultTracker::DefaultTracker() {
  auto reg = Globals::instance()->registry();
  QObject::connect(
      reg, &Registry::metadataAdded, this, &DefaultTracker::onMetadataAdded);
  QObject::connect(
      reg, &Registry::nodeAdded, this, &DefaultTracker::onNodeAdded);
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

void DefaultTracker::onMetadataAdded(Metadata *metadata) {}
void DefaultTracker::onNodeAdded(Node *node) {}

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
    p->node  = reg->findNodeByName(name).get();
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
  if (p) return p->node;
  return nullptr;
}

QString DefaultTracker::getNodeName(Type type) const {
  auto p = getDefaultProps(type);
  if (p) return p->name;
  return {};
}

void DefaultTracker::setDefaultNode(Type type, Node *node) {
  auto p = getDefaultProps(type);
  if (!p) return;

  if (p->node == node) return;
  p->node = node;
  if (node) {
    // connect destroy signal
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
} // namespace ns::pipewire
