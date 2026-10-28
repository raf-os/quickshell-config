#pragma once

#include <memory>

#include <qobject.h>
#include <qobjectdefs.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "pw_metadata.h"
#include "pw_node.h"

namespace ns::pipewire {
class DefaultTracker : public QObject {
  Q_OBJECT

  using change_signal_t = void (DefaultTracker::*)();
  using destroy_slot_t  = void (DefaultTracker::*)();

public:
  explicit DefaultTracker(QObject *parent = nullptr);

  enum Type : quint8 {
    Sink,
    Source,
    ConfiguredSink,
    ConfiguredSource,
  };
  Q_ENUM(Type)

  void reset();

  bool setConfiguredDefault(const char *key, const QString &value);

  struct DefaultNodeProps {
    QString                 name;
    std::shared_ptr<Node>   node;
    QMetaObject::Connection onDestroyConnection;
  };

  [[nodiscard]] Node   *getNode(DefaultTracker::Type type);
  [[nodiscard]] QString getNodeName(DefaultTracker::Type type) const;

signals:
  void defaultNodeChanged(DefaultTracker::Type type);
  void defaultNodeNameChanged(DefaultTracker::Type type);

private slots:
  void onDefaultNodeDestroyed(DefaultTracker::Type type);
  void onMetadataAdded(TrackableMetadata *metadata);
  void onMetadataProperty(const char *key, const char *type, const char *value);
  void onNodeAdded(TrackableNode *node);

private:
  std::shared_ptr<Metadata> m_defaultsMetadata;

  struct {
    DefaultNodeProps sink;
    DefaultNodeProps source;
    DefaultNodeProps configuredSink;
    DefaultNodeProps configuredSource;
  } m_defaults;

  DefaultNodeProps       *getDefaultProps(DefaultTracker::Type type);
  const DefaultNodeProps *getDefaultProps(DefaultTracker::Type type) const;

  void setDefaultNode(DefaultTracker::Type type, std::shared_ptr<Node> node);
  void setDefaultName(DefaultTracker::Type type, const QString &name);
};
} // namespace ns::pipewire
