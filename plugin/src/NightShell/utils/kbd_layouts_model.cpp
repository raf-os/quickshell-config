#include "kbd_layouts_model.h"

#include <algorithm>
#include <iterator>
#include <memory>
#include <string>
#include <utility>

#include <qabstractitemmodel.h>
#include <qassert.h>
#include <qhash.h>
#include <qhashfunctions.h>
#include <qloggingcategory.h>
#include <qmap.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qsortfilterproxymodel.h>
#include <qtpreprocessorsupport.h>

#include "keyboard_layout_serializer.h"
#include "ns_utils_debug.h"
#include "utils_singletons.h"

namespace ns::utils {
KbdNode::KbdNode(KbdNode *parent) : m_parent(parent) {}

KbdNode *KbdNode::parent() { return m_parent; }

KbdNode *KbdNode::child(int row) {
  if (row < 0 || row > m_children.size()) return nullptr;

  auto it = std::ranges::next(m_children.begin(), row, m_children.end());
  if (it != m_children.end()) {
    return it->second.get();
  }
  Q_ASSERT(false); // should not happen
  return nullptr;
}

int KbdNode::childCount() const { return m_children.size(); }

int KbdNode::row() const {
  if (!m_parent) return 0;
  const auto it = std::ranges::find_if(m_parent->m_children.cbegin(),
      m_parent->m_children.cend(),
      [this](const auto &item) { return item.second.get() == this; });

  if (it != m_parent->m_children.cend()) {
    return std::distance(m_parent->m_children.cbegin(), it);
  }
  Q_ASSERT(false);
  return -1;
}

void KbdNode::appendChild(
    const std::string &name, std::unique_ptr<KbdNode> &&child) {
  m_children.insert_or_assign(name, std::move(child));
}

// KbdLayoutsModel
KbdLayoutsModel::KbdLayoutsModel(KeyboardLayoutXMLSerializer *parent)
    : QAbstractItemModel(parent), m_serializer(parent),
      m_root(std::make_unique<KbdNode>()) {
  if (m_serializer) {
    parseData();
  } else {
    m_serializer = UtilsSingleton::keyboardLayoutSerializer();
    parseData();
  }

  if (m_serializer) {
    QObject::connect(m_serializer, &KeyboardLayoutXMLSerializer::ready, this,
        &KbdLayoutsModel::parseData);
  }
}

KbdLayoutsModel::~KbdLayoutsModel() = default;

void KbdLayoutsModel::parseData() {
  if (!m_serializer) {
    qCWarning(NS_LOG_UTILS) << "Cannot populate model for keyboard layouts as "
                               "it has not been assigned a serializer object.";
    return;
  }

  const auto table = m_serializer->getLayoutTable();

  beginResetModel();
  m_root->m_children.clear();

  for (const auto &[key, value] : table.asKeyValueRange()) {
    auto node  = std::make_unique<KbdNode>();
    node->name = value.item.name;

    for (const auto &[key, value] : value.variants.asKeyValueRange()) {
      auto cNode     = std::make_unique<KbdNode>();
      cNode->name    = node->name;
      cNode->variant = value.name;
      node->appendChild(cNode->variant.toStdString(), std::move(cNode));
    }

    m_root->appendChild(node->name.toStdString(), std::move(node));
  }
  endResetModel();
}

QHash<int, QByteArray> KbdLayoutsModel::roleNames() const {
  return {
      {Roles::Name,             "name"            },
      {Roles::Variant,          "variant"         },
      {Roles::Description,      "description"     },
      {Roles::ShortDescription, "shortDescription"}
  };
}

Qt::ItemFlags KbdLayoutsModel::flags(const QModelIndex &index) const {
  return index.isValid() ? QAbstractItemModel::flags(index)
                         : Qt::ItemFlags(Qt::NoItemFlags);
}

int KbdLayoutsModel::rowCount(const QModelIndex &parent) const {
  if (parent.column() > 0) return 0;

  const KbdNode *parentItem =
      parent.isValid() ? static_cast<const KbdNode *>(parent.internalPointer())
                       : m_root.get();

  return parentItem->childCount();
}

int KbdLayoutsModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)
  return 1;
}

QModelIndex KbdLayoutsModel::index(
    int row, int column, const QModelIndex &parent) const {
  if (!hasIndex(row, column, parent)) return {};

  KbdNode *parentItem = parent.isValid()
                            ? static_cast<KbdNode *>(parent.internalPointer())
                            : m_root.get();

  if (auto *child = parentItem->child(row)) {
    return createIndex(row, column, child);
  }
  return {};
}

QModelIndex KbdLayoutsModel::parent(const QModelIndex &index) const {
  if (!index.isValid()) return {};

  auto    *child  = static_cast<KbdNode *>(index.internalPointer());
  KbdNode *parent = child->parent();

  return parent != m_root.get() ? createIndex(parent->row(), 0, parent)
                                : QModelIndex();
}

QVariant KbdLayoutsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) return {};

  const auto *item = static_cast<const KbdNode *>(index.internalPointer());

  const auto iName    = item->name;
  const auto iVariant = item->variant;

  const auto metaOpt = m_serializer->getConfigItem(iName, iVariant);
  if (!metaOpt.has_value()) return {};

  const auto meta = metaOpt.value();

  switch (role) {
  case Roles::Name:        return QVariant::fromValue(item->name);
  case Roles::Variant:     return QVariant::fromValue(item->variant);
  case Roles::Description: return QVariant::fromValue(meta.description);
  case Roles::ShortDescription:
    return QVariant::fromValue(meta.shortDescription);
  }

  return {};
}
} // namespace ns::utils
