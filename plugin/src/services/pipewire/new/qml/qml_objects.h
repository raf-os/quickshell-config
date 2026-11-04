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
#include "pw_node_data.h"
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
  Q_PROPERTY(QString binary READ binary NOTIFY appDataChanged)
  Q_PROPERTY(QString iconName READ iconName NOTIFY appDataChanged)
  Q_PROPERTY(QVariantMap properties READ properties NOTIFY propertiesChanged)
  Q_PROPERTY(float volume READ averageVolume NOTIFY volumesChanged)
  Q_PROPERTY(bool muted READ muted NOTIFY mutedChanged)

public:
  explicit PwQMLNode(shared_ptr_t ptr, QObject *parent = nullptr);
  // TODO: Asan reports memory leak via new-delete-type-mismatch. Real?
  // If true, switch to composition rather than inheritance
  ~PwQMLNode() override;

  [[nodiscard]] quint32     id() const;
  [[nodiscard]] QVariantMap properties() const;

  [[nodiscard]] QString name() const;
  [[nodiscard]] QString description() const;
  [[nodiscard]] QString nickname() const;

  [[nodiscard]] QString binary() const;
  [[nodiscard]] QString iconName() const;

  [[nodiscard]] float averageVolume() const;
  Q_INVOKABLE void    setVolume(float volume);

  [[nodiscard]] bool muted() const;
  Q_INVOKABLE void   setMuted(bool value);

  [[nodiscard]] bool ready() const;

  [[nodiscard]] NodeType::Flags type() const;

signals:
  void appDataChanged();
  void readyChanged();
  void propertiesChanged();
  void volumesChanged();
  void mutedChanged();

private:
  NodeType::Flags m_type = NodeType::Untracked;

  NodeAudioData *m_audioData = nullptr;
};

class PwQMLLink : public PwTrackedQml<Link> {};
} // namespace ns::pipewire
