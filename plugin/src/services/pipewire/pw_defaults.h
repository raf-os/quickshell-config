#pragma once

#include <qhashfunctions.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include "pw_bindable.h"
#include "pw_metadata.h"
#include "pw_node.h"
#include "pwregistry.h"

namespace ns::services::pipewire {
class DefaultTracker : public QObject {
  Q_OBJECT

  using change_signal_t = void (DefaultTracker::*)();
  using destroy_slot_t  = void (DefaultTracker::*)();

public:
  explicit DefaultTracker(PwRegistry *registry);
  void reset();

  bool setConfiguredDefault(const char *key, const QString &value);

  struct DefaultNodeProps {
    QString name;
    Node   *node = nullptr;
  };

  // So much repetition - hopefully these macros clean things up a bit
  // This declares a getter, setters, node and name change signals, callback for
  // node destruction and a private member variable.
#define DECL_NODE_FIELDS(ID)                                                   \
public:                                                                        \
  [[nodiscard]] DefaultNodeProps default##ID() const;                          \
  Q_SIGNAL void default##ID##Changed();                                        \
  Q_SIGNAL void default##ID##NameChanged();                                    \
                                                                               \
private:                                                                       \
  Q_SLOT void      onDefault##ID##Destroyed();                                 \
  void             setDefault##ID(Node *node);                                 \
  void             setDefault##ID##Name(const QString &name);                  \
  DefaultNodeProps m_default##ID;

  DECL_NODE_FIELDS(Sink)
  DECL_NODE_FIELDS(Source)
  DECL_NODE_FIELDS(ConfiguredSink)
  DECL_NODE_FIELDS(ConfiguredSource)
#undef DECL_NODE_FIELDS

private slots:
  void onMetadataAdded(PwMetadata *metadata);
  void onMetadataProperty(const char *key, const char *type, const char *value);
  void onNodeAdded(Node *node);

private:
  PwRegistry                      *m_registry;
  PwBindableRefCounted<PwMetadata> m_defaultsMetadata;

  void setDefaultNodeGeneric(Node *node, DefaultNodeProps *member,
      destroy_slot_t onDestroySlot, change_signal_t onChangeSignal);
  void setDefaultNodeNameGeneric(const QString &name, DefaultNodeProps *member,
      change_signal_t onChangeSignal);
};
} // namespace ns::services::pipewire
