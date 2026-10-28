#include "qml_pipewire.h"

#include <array>

#include <qabstractitemmodel.h>
#include <qlist.h>
#include <qobject.h>
#include <qtpreprocessorsupport.h>
#include <qvariant.h>

#include "helpermacros.h"
#include "pw_common.h"
#include "pw_defaults.h"
#include "pw_globals.h"
#include "pw_node.h"

namespace ns::pipewire {
static DefaultTracker *get_default_tracker() {
  return Globals::instance()->defaults();
}

namespace {
constexpr int COL_AUDIO_SINK   = 0;
constexpr int COL_AUDIO_SOURCE = 1;
constexpr int COL_MAX          = 2;
} // namespace

AUTO_MEYERS_SINGLETON_QML_IMPL(QMLPipewireGlobal)

QMLPipewireGlobal::QMLPipewireGlobal(QObject *parent) : QObject(parent) {
  auto tracker = get_default_tracker();

  QObject::connect(tracker, &DefaultTracker::defaultNodeChanged, this,
      &QMLPipewireGlobal::onDefaultNodeChanged);
}

void QMLPipewireGlobal::onDefaultNodeChanged(DefaultTracker::Type type) {
  using Type = DefaultTracker::Type;
  switch (type) {
  case Type::Sink:             emit defaultSinkChanged(); break;
  case Type::Source:           emit defaultSourceChanged(); break;
  case Type::ConfiguredSink:   emit defaultConfiguredSinkChanged(); break;
  case Type::ConfiguredSource: emit defaultConfiguredSourceChanged(); break;
  }
}

Node *QMLPipewireGlobal::defaultSink() {
  return get_default_tracker()->getNode(DefaultTracker::Type::Sink);
}
Node *QMLPipewireGlobal::defaultSource() {
  return get_default_tracker()->getNode(DefaultTracker::Type::Source);
}
Node *QMLPipewireGlobal::defaultConfiguredSink() {
  return get_default_tracker()->getNode(DefaultTracker::Type::ConfiguredSink);
}
Node *QMLPipewireGlobal::defaultConfiguredSource() {
  return get_default_tracker()->getNode(DefaultTracker::Type::ConfiguredSource);
}

QHash<int, QByteArray> PipewireModel::roleNames() const {
  return {
      {Roles::ModelDataRole,   "modelData"  },
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
  return COL_MAX;
}

QModelIndex PipewireModel::index(
    int row, int column, const QModelIndex &parent) const {
  if (!this->hasIndex(row, column, parent)) return {};
  if (parent.isValid()) return {};

  return createIndex(row, column, nullptr);
}

QModelIndex PipewireModel::parent(const QModelIndex &index) const { return {}; }

QVariant PipewireModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) return {};

  const QList<quint32> *listPtr = nullptr;
  switch (index.column()) {
  case COL_AUDIO_SINK:   listPtr = const_cast<device_list_t *>(&m_audioSinks);
  case COL_AUDIO_SOURCE: listPtr = const_cast<device_list_t *>(&m_audioSources);
  }

  if (!listPtr) return {};

  auto devId = listPtr->at(index.row());
  auto it    = m_trackedNodes.constFind(devId);

  if (it == m_trackedNodes.constEnd()) return {};
  auto node = it.value().get();

  switch (role) {
  case ModelDataRole:   return QVariant::fromValue(node);
  case NameRole:        return QVariant::fromValue(node->m_name);
  case DescriptionRole: return QVariant::fromValue(node->m_description);
  case NicknameRole:    return QVariant::fromValue(node->m_nick);
  }

  return {};
}

QModelIndex PipewireModel::listToIndex(device_list_t *list, int row) const {
  if (list == &m_audioSinks) return createIndex(row, COL_AUDIO_SINK, nullptr);
  else if (list == &m_audioSources)
    return createIndex(row, COL_AUDIO_SOURCE, nullptr);
  return {};
}

void PipewireModel::onNodeAdded(TrackableNode *node) {
  const auto isAudioSink   = node->type().testFlag(NodeType::AudioSink);
  const auto isAudioSource = node->type().testFlag(NodeType::AudioSource);

  if (!isAudioSink && !isAudioSource) return;

  device_list_t *listPtr;

  if (isAudioSink) listPtr = &m_audioSinks;
  else if (isAudioSource) listPtr = &m_audioSources;
  else return; // Technically, this should never happen

  const auto nodeId   = node->id();
  auto       insertIt = m_trackedNodes.insert(nodeId, node->get());

  QObject::connect(insertIt.value().get(), &QObject::destroyed, this,
      &PipewireModel::onNodeDestroyed);

  if (!listPtr->contains(nodeId)) {
    auto idx = listToIndex(listPtr, listPtr->size() - 1);
    if (!idx.isValid()) return;
    beginInsertRows(idx, listPtr->size(), listPtr->size());
    listPtr->append(nodeId);
    endInsertRows();
  }
}

void PipewireModel::onNodeDestroyed() {
  auto node = static_cast<Node *>(sender());

  m_trackedNodes.remove(node->m_id);

  const std::array<device_list_t *, 2> nodeArray = {
      &m_audioSinks, &m_audioSources};
  for (auto &list : nodeArray) {
    if (!list->contains(node->m_id)) continue;

    auto itemIdx = list->indexOf(node->m_id);

    auto listId = listToIndex(list, itemIdx);
    if (!listId.isValid()) continue;

    beginRemoveRows(listId, itemIdx, itemIdx);
    list->removeAt(itemIdx);
    endRemoveRows();
  }
}
} // namespace ns::pipewire
