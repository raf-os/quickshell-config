#pragma once

#include <memory>

#include <qabstractitemmodel.h>
#include <qflags.h>
#include <qhash.h>
#include <qhashfunctions.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qvariant.h>

#include "helpermacros.h"
#include "pw_common.h"
#include "pw_defaults.h"
#include "pw_node.h"
#include "qml/qml_objects.h"

namespace ns::pipewire {
class Registry;

class QMLPipewireGlobal : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(Pipewire)
  QML_SINGLETON

private:
  using change_signal_t = void (QMLPipewireGlobal::*)();
  // Pointer to own tracked node (which is also a pointer)
  using member_node_t = PwQMLNode **;

  AUTO_MEYERS_SINGLETON_QML_DECL(QMLPipewireGlobal)

#define PROP(Name)                                                             \
  Q_PROPERTY(PwQMLNode                                                         \
          *default##Name READ default##Name NOTIFY default##Name##Changed)     \
public:                                                                        \
  [[nodiscard]] PwQMLNode *default##Name();                                    \
  Q_SIGNAL void default##Name##Changed();                                      \
                                                                               \
private:                                                                       \
  PwQMLNode *m_default##Name = nullptr;

  PROP(Sink)
  PROP(Source)
  PROP(ConfiguredSink)
  PROP(ConfiguredSource)
#undef PROP

private slots:
  void onDefaultNodeChanged(DefaultTracker::Type type);
  void setDefault(DefaultTracker::Type type, std::shared_ptr<Node> newPtr);

private:
  explicit QMLPipewireGlobal(QObject *parent = nullptr);
};

class PipewireModel : public QAbstractItemModel {
  Q_OBJECT
  QML_NAMED_ELEMENT(PipewireModel)

  Q_PROPERTY(int trackedTypes READ trackedTypes WRITE setTrackedTypes NOTIFY
          trackedTypesChanged REQUIRED)

  using device_list_t = QList<quint32>;

public:
  enum Roles {
    ModelDataRole = Qt::UserRole,
    IdRole,
    NameRole,
    DescriptionRole,
    NicknameRole,
  };

  enum Type : quint8 {
    None           = 0,
    NodeSink       = 1 << 0,
    NodeSource     = 1 << 1,
    NodeSinkSource = NodeSink | NodeSource,
    TYPE_MAX       = 1 << 2
  };
  Q_DECLARE_FLAGS(Types, Type)
  Q_FLAG(Type)

  explicit PipewireModel(QObject *parent = nullptr);

  [[nodiscard]] int trackedTypes() const;
  void              setTrackedTypes(int value);

  // To be used with DelegateModel qml type
  [[nodiscard]] Q_INVOKABLE QVariant modelIndexFor(Type type) const;

  QHash<int, QByteArray> roleNames() const override;
  int                    rowCount(const QModelIndex &parent) const override;
  int                    columnCount(const QModelIndex &parent) const override;

  QModelIndex index(
      int row, int column, const QModelIndex &parent) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  QVariant    data(const QModelIndex &index, int role) const override;

signals:
  void audioSinksChanged();
  void audioSourcesChanged();
  void trackedTypesChanged();

private slots:
  void onNodeAdded(TrackableNode *node);
  void onNodeDestroyed();

private:
  Registry                 *m_registry;
  Types                     m_trackedTypes = Type::None;
  QHash<quint32, PwQMLNode> m_trackedNodes;

  [[nodiscard]] int  getListCol(device_list_t *listPtr);
  [[nodiscard]] bool testForeignFlag(NodeType::Flags flag);

  void insertSorted(device_list_t &list, quint32 value);
  void insertSorted(device_list_t &list, int col, quint32 value);

  void removeAt(device_list_t &list, int row);
  void removeAt(device_list_t &list, int col, int row);

  void removeNodeId(const quint32 &id);

  device_list_t m_audioSinks;
  device_list_t m_audioSources;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PipewireModel::Types)
} // namespace ns::pipewire
