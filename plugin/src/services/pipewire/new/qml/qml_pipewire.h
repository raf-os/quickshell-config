#pragma once

#include <memory>
#include <unordered_map>

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
  QML_NAMED_ELEMENT(PipewireGlobal)
  QML_SINGLETON

  using change_signal_t = void (QMLPipewireGlobal::*)();
  // Pointer to own tracked node (which is also a pointer)
  using member_node_t = PwQMLNode **;

  AUTO_MEYERS_SINGLETON_QML_DECL(QMLPipewireGlobal)

  // Q_PROPERTY(PwQMLNode *defaultSink READ defaultSink NOTIFY
  // defaultSinkChanged)

public:
  [[nodiscard]] PwQMLNode *defaultSink();
  Q_SIGNAL void            defaultSinkChanged();

  [[nodiscard]] PwQMLNode *defaultSource();
  Q_SIGNAL void            defaultSourceChanged();

  [[nodiscard]] PwQMLNode *defaultConfiguredSink();
  Q_SIGNAL void            defaultConfiguredSinkChanged();

  [[nodiscard]] PwQMLNode *defaultConfiguredSource();
  Q_SIGNAL void            defaultConfiguredSourceChanged();

private slots:
  void onDefaultNodeChanged(DefaultTracker::Type type);
  void setDefault(DefaultTracker::Type type, std::shared_ptr<Node> newPtr);

private:
  explicit QMLPipewireGlobal(QObject *parent = nullptr);

  PwQMLNode *m_defaultSink             = nullptr;
  PwQMLNode *m_defaultSource           = nullptr;
  PwQMLNode *m_defaultConfiguredSink   = nullptr;
  PwQMLNode *m_defaultConfiguredSource = nullptr;
};

class PipewireModel : public QAbstractItemModel {
  Q_OBJECT
  QML_ELEMENT

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
  Registry *m_registry;
  Types     m_trackedTypes = Type::None;
  std::unordered_map<quint32, std::unique_ptr<PwQMLNode>> m_trackedNodes;

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
