#pragma once

#include <qcontainerfwd.h>
#include <qhashfunctions.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "ns_pipewire_export.h"
#include "pw_common.h"
#include "pw_link.h"
#include "pw_node.h"
#include "qml_object_interface.h"

namespace ns::pipewire {
class NS_PIPEWIRE_EXPORT PwQMLNode : public PwTrackedQml<Node> {
  Q_OBJECT
  QML_NAMED_ELEMENT(PwNode)
  QML_UNCREATABLE("")

  Q_PROPERTY(quint32 id READ id CONSTANT)
  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(QString description READ description CONSTANT)
  Q_PROPERTY(QString nickname READ nickname CONSTANT)
  Q_PROPERTY(QVariantMap properties READ properties NOTIFY propertiesChanged)

public:
  explicit PwQMLNode(shared_ptr_t ptr, QObject *parent = nullptr);

  void init() override;

  [[nodiscard]] quint32     id() const;
  [[nodiscard]] QString     name() const;
  [[nodiscard]] QString     description() const;
  [[nodiscard]] QString     nickname() const;
  [[nodiscard]] QVariantMap properties() const;

  [[nodiscard]] NodeType::Flags type() const;

signals:
  void propertiesChanged();

private:
  NodeType::Flags m_type = NodeType::Untracked;
};

class PwQMLLink : public PwTrackedQml<Link> {};
} // namespace ns::pipewire
