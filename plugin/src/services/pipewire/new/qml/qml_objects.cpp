#include "qml_objects.h"

#include <qcontainerfwd.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qtypes.h>
#include <qvariant.h>

#include "ns_pipewire_debug.h"
#include "pw_common.h"

namespace ns::pipewire {
// PwQMLNode
PwQMLNode::PwQMLNode(shared_ptr_t ptr, QObject *parent)
    : PwTrackedQml<Node>(ptr, parent) {}

quint32     PwQMLNode::id() const { return object()->m_id; }
QString     PwQMLNode::name() const { return object()->m_name; }
QString     PwQMLNode::description() const { return object()->m_description; }
QString     PwQMLNode::nickname() const { return object()->m_nick; }
QVariantMap PwQMLNode::properties() const {
  QVariantMap map;
  const auto  props = object()->m_properties;
  for (auto it = props.constKeyValueBegin(); it != props.constKeyValueEnd();
      it++)
  {
    map.insert(it->first, QVariant::fromValue(it->second));
  }
  return map;
}
NodeType::Flags PwQMLNode::type() const { return m_type; }
// PwQMLNode
} // namespace ns::pipewire
