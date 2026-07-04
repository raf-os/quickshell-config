#include "desktopentriesmodel.h"
#include "config.h"
#include "desktopentry.h"
#include "entrymanager.h"
#include "generated/launcherconfig.h"

#include <algorithm>
#include <qabstractitemmodel.h>
#include <qcontainerfwd.h>
#include <qhash.h>
#include <qlist.h>
#include <qobject.h>
#include <qqmllist.h>
#include <qtimer.h>
#include <qtypes.h>
#include <qvariant.h>
#include <rapidfuzz/fuzz.hpp>
#include <string>
#include <string_view>

namespace ns::desktop::entries {
DesktopEntriesModel::DesktopEntriesModel(QObject *parent)
    : QAbstractListModel(parent),
      m_manager(EntryManager::instance()) {
  m_debouncer.setInterval(250);
  m_debouncer.setSingleShot(true);

  QObject::connect(&m_debouncer,
                   &QTimer::timeout,
                   this,
                   &DesktopEntriesModel::onDebounceTimeout);

  this->onEntriesChanged();

  QObject::connect(m_manager,
                   &EntryManager::applicationsChanged,
                   this,
                   &DesktopEntriesModel::onEntriesChanged);
  QObject::connect(
      m_manager, &QObject::destroyed, this, [this]() { this->deleteLater(); });

  QObject::connect(myqmlplugin::configs::Config::instance()->launcher(),
                   &myqmlplugin::configs::LauncherConfig::favoriteAppsChanged,
                   this,
                   &DesktopEntriesModel::onFavoriteEntriesChanged);
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

bool DesktopEntriesModel::sortCompare(DesktopEntry *a,
                                      DesktopEntry *b) {
  return a->bindableName().value().localeAwareCompare(
             b->bindableName().value()) < 0;
};

void DesktopEntriesModel::sortEntries(QList<DesktopEntry *> &list) {
  std::sort(list.begin(), list.end(), [this](DesktopEntry *a, DesktopEntry *b) {
    return a->bindableName().value().localeAwareCompare(
        b->bindableName().value());
  });
}

void DesktopEntriesModel::onEntriesChanged() {
  auto newList = m_manager->getEntries();

  this->beginResetModel();
  m_entries = newList.values();
  sortEntries(m_entries);
  this->endResetModel();
  emit entryListChanged();
}

void DesktopEntriesModel::onFavoriteEntriesChanged() {
  auto newFavs =
      myqmlplugin::configs::Config::instance()->launcher()->favoriteApps();

  if (newFavs == m_favoriteEntries)
    return;

  m_favoriteEntries = newFavs;

  this->beginResetModel();
  sortEntries(m_entries);
  this->endResetModel();
  emit entryListChanged();
}

QString DesktopEntriesModel::queryString() const { return m_queryString; }
void    DesktopEntriesModel::setQueryString(const QString &value) {
  if (value == m_queryString)
    return;

  m_queryString = value;
  emit queryStringChanged();

  m_debouncer.start();
}

bool DesktopEntriesModel::showTerminalOnly() const {
  return m_showTerminalOnly;
}
void DesktopEntriesModel::setShowTerminalOnly(bool value) {
  if (value == m_showTerminalOnly)
    return;

  m_showTerminalOnly = value;
  emit showTerminalOnlyChanged();

  m_debouncer.start();
}

void DesktopEntriesModel::onDebounceTimeout() {
  if (m_queryString == m_previousQueryString)
    return;
  m_previousQueryString = m_queryString;

  if (m_queryString.isEmpty()) {
    resetAllFilters();
    return;
  }

  const auto            originalEntries = m_manager->getEntries();
  QList<DesktopEntry *> filtered;
  filtered.reserve(originalEntries.count());

  std::string qstr = m_queryString.trimmed().toLower().toStdString();
  rapidfuzz::fuzz::CachedPartialRatio<char> scorer(qstr);

  for (auto it = originalEntries.constBegin(); it != originalEntries.constEnd();
       ++it) {
    const auto entry = it.value();
    const auto entryName =
        entry->bindableName().value().toLower().toStdString();

    if (entryName == qstr) {
      filtered.append(entry);
    } else if (entryName.starts_with(qstr)) {
      filtered.append(entry);
    } else {
      auto score = scorer.similarity(std::string_view(entryName), 80.0);
      if (score > 80.0) {
        filtered.append(entry);
      }
    }
  }

  if (filtered.size() == m_entries.size())
    return;

  filtered.squeeze();
  sortEntries(filtered);

  // There's likely a better way of going about this instead of resetting the
  // entire model state, but it will require a specialized algorithm - likely
  // some sort diff-like algorithm - but it's probably overengineering and qt
  // views can handle this just fine.
  this->beginResetModel();
  m_entries = std::move(filtered);
  this->endResetModel();
  emit entryListChanged();
}

void DesktopEntriesModel::resetAllFilters() {
  auto managerList = m_manager->getEntries().values();

  sortEntries(managerList);

  if (managerList == m_entries)
    return;

  this->beginResetModel();
  m_entries = std::move(managerList);
  this->endResetModel();
  emit entryListChanged();
}

DesktopEntry *
DesktopEntriesModel::entryListAt(QQmlListProperty<DesktopEntry> *property,
                                 qsizetype                       index) {
  auto list = static_cast<QList<DesktopEntry *> *>(property->data);
  return list->at(index);
}

qsizetype
DesktopEntriesModel::entryListCount(QQmlListProperty<DesktopEntry> *property) {
  auto list = static_cast<QList<DesktopEntry *> *>(property->data);
  return list->count();
}

// Returns read-only list
QQmlListProperty<DesktopEntry> DesktopEntriesModel::entryList() {
  return QQmlListProperty<DesktopEntry>(this,
                                        &m_entries,
                                        &DesktopEntriesModel::entryListCount,
                                        &DesktopEntriesModel::entryListAt);
}
} // namespace ns::desktop::entries
