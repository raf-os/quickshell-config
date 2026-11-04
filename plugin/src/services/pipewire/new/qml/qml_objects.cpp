#include "qml_objects.h"

#include <qcontainerfwd.h>
#include <qhashfunctions.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qtypes.h>
#include <qvariant.h>

#include "ns_pipewire_debug.h"
#include "pw_common.h"
#include "pw_node_data.h"

namespace ns::pipewire {
// PwQMLNode
PwQMLNode::PwQMLNode(shared_ptr_t ptr, QObject *parent)
    : PwTrackedQml<Node>(ptr, parent) {
  m_id = object()->m_id;

  QObject::connect(
      object(), &Node::readyChanged, this, &PwQMLNode::readyChanged);
  QObject::connect(
      object(), &Node::propertiesChanged, this, &PwQMLNode::propertiesChanged);
  QObject::connect(
      object(), &Node::clientAppDataChanged, this, &PwQMLNode::appDataChanged);

  if (object()->m_type.testFlag(NodeType::Audio)) {
    m_audioData = dynamic_cast<NodeAudioData *>(object()->data());
    QObject::connect(m_audioData, &NodeAudioData::volumesChanged, this,
        &PwQMLNode::volumesChanged);
    QObject::connect(m_audioData, &NodeAudioData::mutedChanged, this,
        &PwQMLNode::mutedChanged);
  }
}
PwQMLNode::~PwQMLNode() {}

bool    PwQMLNode::ready() const { return object()->m_ready; }
quint32 PwQMLNode::id() const { return m_id; }
QString PwQMLNode::name() const { return object()->m_name; }
QString PwQMLNode::description() const { return object()->m_description; }
QString PwQMLNode::nickname() const { return object()->m_nick; }

QString PwQMLNode::binary() const { return object()->clientBinary(); }
QString PwQMLNode::iconName() const { return object()->clientIconName(); }

QVariantMap PwQMLNode::properties() const {
  QVariantMap map;
  const auto  props = object()->properties();
  for (auto it = props.constKeyValueBegin(); it != props.constKeyValueEnd();
      it++)
  {
    map.insert(it->first, QVariant::fromValue(it->second));
  }
  return map;
}
NodeType::Flags PwQMLNode::type() const { return m_type; }

float PwQMLNode::averageVolume() const {
  if (!m_audioData) return 0;
  return m_audioData->averageVolume();
}
void PwQMLNode::setVolume(float volume) {
  if (!m_audioData) return;
  m_audioData->setAverageVolume(volume);
}

bool PwQMLNode::muted() const {
  if (!m_audioData) return false;
  return m_audioData->isMuted();
}
void PwQMLNode::setMuted(bool value) {
  if (!m_audioData) return;
  m_audioData->setMuted(value);
}
// PwQMLNode
} // namespace ns::pipewire
