#include "qml_pipewire.h"

#include <cstddef>
#include <memory>

#include <concepts>
#include <qabstractitemmodel.h>
#include <qlist.h>
#include <qobject.h>
#include <qqmlinfo.h>
#include <qtpreprocessorsupport.h>
#include <qtypes.h>
#include <qvariant.h>

#include "helpermacros.h"
#include "pw_common.h"
#include "pw_defaults.h"
#include "pw_globals.h"
#include "pw_node.h"
#include "pw_registry.h"
#include "qml/qml_objects.h"

namespace ns::pipewire {
static DefaultTracker *get_default_tracker() {
  return Globals::instance()->defaults();
}

namespace {
constexpr int COL_AUDIO_SINK   = 0;
constexpr int COL_AUDIO_SOURCE = 1;
constexpr int COL_MAX          = 2;

PipewireModel::Types map_foreign_flag(NodeType::Flags type) {
  using Type             = PipewireModel::Type;
  PipewireModel::Types t = Type::None;
  if (!type.testFlag(NodeType::Audio)) return Type::None;
  if (type.testFlag(NodeType::Source)) t.setFlag(Type::NodeSource);
  if (type.testFlag(NodeType::Sink)) t.setFlag(Type::NodeSink);
  return t;
}

template <typename T>
concept Comparable = requires(T a, T b) {
  { a > b } -> std::convertible_to<bool>;
};

template <Comparable T> int list_sorted_index(QList<T> *list, T value) {
  size_t i = 0;
  for (; i < list->size(); i++) {
    if (value > list->at(i)) break;
  }
  return i;
}
} // namespace

AUTO_MEYERS_SINGLETON_QML_IMPL(QMLPipewireGlobal)

QMLPipewireGlobal::QMLPipewireGlobal(QObject *parent) : QObject(parent) {
  auto tracker = get_default_tracker();

  QObject::connect(tracker, &DefaultTracker::defaultNodeChanged, this,
      &QMLPipewireGlobal::onDefaultNodeChanged);
}

void QMLPipewireGlobal::onDefaultNodeChanged(DefaultTracker::Type type) {
  auto tracker = get_default_tracker();
  using Type   = DefaultTracker::Type;
  return setDefault(type, tracker->getNodeSharedPtr(type));
}

void QMLPipewireGlobal::setDefault(
    DefaultTracker::Type type, std::shared_ptr<Node> newPtr) {
  using Type             = DefaultTracker::Type;
  change_signal_t signal = nullptr;
  member_node_t   node   = nullptr;

  switch (type) {
  case Type::Sink:
    signal = &QMLPipewireGlobal::defaultSinkChanged;
    node   = &m_defaultSink;
    break;
  case Type::Source:
    signal = &QMLPipewireGlobal::defaultSourceChanged;
    node   = &m_defaultSource;
    break;
  case Type::ConfiguredSink:
    signal = &QMLPipewireGlobal::defaultConfiguredSinkChanged;
    node   = &m_defaultConfiguredSink;
    break;
  case Type::ConfiguredSource:
    signal = &QMLPipewireGlobal::defaultConfiguredSourceChanged;
    node   = &m_defaultConfiguredSource;
    break;
  }

  if (!signal || !node) return;

  if (*node != nullptr) {
    if ((*node)->object() != newPtr.get()) {
      (*node)->deleteLater();
      *node = nullptr;
    }
  } else {
    if (!newPtr) return;
  }

  if (newPtr) {
    *node = new PwQMLNode(newPtr, this);
    QObject::connect(*node, &PwQMLNode::destroying, this,
        [this, type] { setDefault(type, nullptr); });
  } else {
    *node = nullptr;
  }

  emit(this->*signal)();
}
PwQMLNode *QMLPipewireGlobal::defaultSink() { return m_defaultSink; }
PwQMLNode *QMLPipewireGlobal::defaultSource() { return m_defaultSource; }
PwQMLNode *QMLPipewireGlobal::defaultConfiguredSink() {
  return m_defaultConfiguredSink;
}
PwQMLNode *QMLPipewireGlobal::defaultConfiguredSource() {
  return m_defaultConfiguredSource;
}

PipewireModel::PipewireModel(QObject *parent)
    : QAbstractItemModel(parent), m_registry(Globals::instance()->registry()) {}

QVariant PipewireModel::modelIndexFor(Type type) const {
  QModelIndex idx;
  switch (type) {
  case Type::NodeSink:   idx = createIndex(0, COL_AUDIO_SINK, nullptr); break;
  case Type::NodeSource: idx = createIndex(0, COL_AUDIO_SOURCE, nullptr); break;
  default:               break;
  }

  return QVariant::fromValue(idx);
}

int  PipewireModel::trackedTypes() const { return m_trackedTypes; }
void PipewireModel::setTrackedTypes(int value) {
  if (value < 0 || value >= Type::TYPE_MAX) {
    qmlWarning(this) << "Tracked type is not valid.";
    return;
  }
  if (m_trackedTypes == value) return;

  m_trackedTypes = static_cast<Type>(value);
  emit trackedTypesChanged();

  if (m_trackedTypes == Type::None) {
    beginResetModel();
    m_audioSinks.clear();
    m_audioSources.clear();
    endResetModel();
    m_trackedNodes.clear();
    return;
  }

  for (auto [k, v] : m_registry->m_nodes.asKeyValueRange()) {
    const auto existing      = m_trackedNodes.contains(k);
    const auto shouldInclude = testForeignFlag(v->type());
    if (shouldInclude) {
      onNodeAdded(v);
    } else if (existing) {
      removeNodeId(k);
      m_trackedNodes.erase(k);
    }
  }
}

bool PipewireModel::testForeignFlag(NodeType::Flags flag) {
  auto res = map_foreign_flag(flag);
  if (res == Type::None) return false;
  return res.testFlags(m_trackedTypes);
}

void PipewireModel::insertSorted(device_list_t &list, quint32 value) {
  auto col = getListCol(&list);
  return insertSorted(list, col, value);
}

void PipewireModel::insertSorted(device_list_t &list, int col, quint32 value) {
  if (col < 0 || col >= COL_MAX) return;
  if (list.contains(value)) return;

  auto idx = list_sorted_index(&list, value);
  beginInsertRows(createIndex(0, col, nullptr), idx, idx);
  list.insert(idx, value);
  endInsertRows();
  if (col == COL_AUDIO_SINK) emit audioSinksChanged();
  if (col == COL_AUDIO_SOURCE) emit audioSourcesChanged();
}

void PipewireModel::removeAt(device_list_t &list, int row) {
  auto col = getListCol(&list);
  return removeAt(list, col, row);
}

void PipewireModel::removeAt(device_list_t &list, int col, int row) {
  if (col < 0 || col >= COL_MAX || row < 0 || row > list.size()) return;
  beginRemoveRows(createIndex(0, col, nullptr), row, row);
  list.removeAt(row);
  endRemoveRows();
  if (col == COL_AUDIO_SINK) emit audioSinksChanged();
  if (col == COL_AUDIO_SOURCE) emit audioSourcesChanged();
}

void PipewireModel::removeNodeId(const quint32 &id) {
  auto sinkIdx   = m_audioSinks.indexOf(id);
  auto sourceIdx = m_audioSources.indexOf(id);

  if (sinkIdx != -1) {
    removeAt(m_audioSinks, sinkIdx);
  }

  if (sourceIdx != -1) {
    removeAt(m_audioSources, sourceIdx);
  }
}

QHash<int, QByteArray> PipewireModel::roleNames() const {
  return {
      {Roles::ModelDataRole,   "modelData"  },
      {Roles::IdRole,          "id"         },
      {Roles::NameRole,        "name"       },
      {Roles::DescriptionRole, "description"},
      {Roles::NicknameRole,    "nickname"   },
  };
}

int PipewireModel::rowCount(const QModelIndex &parent) const {
  switch (parent.column()) {
  case COL_AUDIO_SINK:   return m_audioSinks.size();
  case COL_AUDIO_SOURCE: return m_audioSources.size();
  default:               return 0;
  }
}

int PipewireModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)
  return 2;
}

QModelIndex PipewireModel::index(
    int row, int column, const QModelIndex &parent) const {
  if (!this->hasIndex(row, column, parent)) return {};
  if (!parent.isValid()) return {};

  return createIndex(row, column, nullptr);
}

QModelIndex PipewireModel::parent(const QModelIndex &index) const {
  switch (index.column()) {
  case COL_AUDIO_SINK:   return createIndex(0, COL_AUDIO_SINK, nullptr);
  case COL_AUDIO_SOURCE: return createIndex(0, COL_AUDIO_SOURCE, nullptr);
  }
  return {};
}

QVariant PipewireModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) return {};

  const QList<quint32> *listPtr = nullptr;
  switch (index.column()) {
  case COL_AUDIO_SINK:   listPtr = const_cast<device_list_t *>(&m_audioSinks);
  case COL_AUDIO_SOURCE: listPtr = const_cast<device_list_t *>(&m_audioSources);
  }

  if (!listPtr) return {};

  auto devId = listPtr->at(index.row());
  auto it    = m_trackedNodes.find(devId);

  if (it == m_trackedNodes.cend()) return {};
  auto node = it->second.get();

  switch (role) {
  case ModelDataRole:   return QVariant::fromValue(node);
  case IdRole:          return QVariant::fromValue(node->id());
  case NameRole:        return QVariant::fromValue(node->name());
  case DescriptionRole: return QVariant::fromValue(node->description());
  case NicknameRole:    return QVariant::fromValue(node->nickname());
  }

  return {};
}

int PipewireModel::getListCol(device_list_t *listPtr) {
  if (listPtr == &m_audioSinks) return COL_AUDIO_SINK;
  else if (listPtr == &m_audioSources) return COL_AUDIO_SOURCE;
  return -1;
}

void PipewireModel::onNodeAdded(TrackableNode *node) {
  auto mapped = map_foreign_flag(node->type());
  if (mapped == Type::None) return;

  const auto nodeId   = node->id();
  auto       newNode  = std::make_unique<PwQMLNode>(node->get(), this);
  auto       insertIt = m_trackedNodes.emplace(nodeId, std::move(newNode));

  if (!insertIt.second) return;

  QObject::connect(insertIt.first->second.get(), &PwQMLNode::destroying, this,
      &PipewireModel::onNodeDestroyed);

  if (mapped.testFlag(Type::NodeSink)) {
    insertSorted(m_audioSinks, nodeId);
  }
  if (mapped.testFlag(Type::NodeSource)) {
    insertSorted(m_audioSources, nodeId);
  }
}

void PipewireModel::onNodeDestroyed() {
  auto node = static_cast<Node *>(sender());

  m_trackedNodes.erase(node->m_id);
  removeNodeId(node->m_id);
}
} // namespace ns::pipewire
