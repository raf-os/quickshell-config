#include "desktopentriesmodel.h"
#include "desktopentry.h"
#include "entrymanager.h"

#include <algorithm>
#include <qabstractitemmodel.h>
#include <qobject.h>
#include <qtypes.h>
#include <qvariant.h>

namespace ns::desktop::entries {
DesktopEntriesModel::DesktopEntriesModel(QObject *parent)
    : QAbstractListModel(parent) {
  const auto instance = EntryManager::instance();

  this->onEntriesChanged();

  QObject::connect(instance,
                   &EntryManager::applicationsChanged,
                   this,
                   &DesktopEntriesModel::onEntriesChanged);
}

qint32 DesktopEntriesModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return static_cast<qint32>(this->m_entries.size());
}

QVariant DesktopEntriesModel::data(const QModelIndex &index,
                                   qint32             role) const {
  if (!index.isValid())
    return {};

  switch (role) {
  case Roles::ModelDataRole:
    return QVariant::fromValue(m_entries.at(index.row()));
  default:
    return {};
  }
}

void DesktopEntriesModel::onEntriesChanged() {
  const auto manager = EntryManager::instance();
  auto       newList = manager->getEntries();

  this->beginResetModel();
  m_entries = newList.values();
  std::sort(m_entries.begin(),
            m_entries.end(),
            [this](DesktopEntry *a, DesktopEntry *b) {
              return a->bindableName().value().localeAwareCompare(
                         b->bindableName().value()) < 0;
            });
  this->endResetModel();
}
} // namespace ns::desktop::entries
