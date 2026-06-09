#include "keyboardsettingsbuffer.h"
#include "hypr.h"
#include "hyprevents.h"

#include <optional>
#include <qabstractitemmodel.h>
#include <qcontainerfwd.h>
#include <qlist.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqmllist.h>
#include <qvariant.h>
#include <utility>

namespace mscp {
KeyboardSettingsBuffer::KeyboardSettingsBuffer(QObject *parent)
    : QAbstractListModel(parent) {}

int KeyboardSettingsBuffer::rowCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : m_layouts.size();
}

QVariant KeyboardSettingsBuffer::data(const QModelIndex &index,
                                      int role) const {
  if (!index.isValid())
    return {};

  switch (role) {
  case Roles::ModelDataRole:
    return QVariant::fromValue(m_layouts.at(index.row()));
  default:
    return {};
  }
}

QQmlListProperty<KeyboardLayoutItem> KeyboardSettingsBuffer::layouts() {
  return QQmlListProperty<KeyboardLayoutItem>(this, &m_layouts);
}

bool KeyboardSettingsBuffer::isDirty() const {
  return (m_dirtyFields.idx == true || m_dirtyFields.layouts == true);
}

int KeyboardSettingsBuffer::selectedId() const { return m_selectedId; }
void KeyboardSettingsBuffer::setSelectedId(const int &value) {
  if (m_selectedId == value)
    return;

  int actualValue = value;

  if (actualValue < 0)
    actualValue = 0;
  else if (actualValue > m_layouts.count() - 1)
    actualValue = m_layouts.count() - 1;

  if (actualValue == m_selectedId)
    return;

  m_selectedId = actualValue;
  m_dirtyFields.idx = true;
  emit isDirtyChanged();
  emit selectedIdChanged();
}

myqmlplugin::HyprExtras *KeyboardSettingsBuffer::instance() const {
  return m_instance;
}
void KeyboardSettingsBuffer::setInstance(myqmlplugin::HyprExtras *instance) {
  if (m_instance == instance)
    return;

  if (m_inputConfig) {
    QObject::disconnect(this, nullptr, m_inputConfig, nullptr);
  }
  if (m_instance) {
    QObject::disconnect(this, nullptr, m_instance, nullptr);
  }
  if (m_eventListener) {
    QObject::disconnect(this, nullptr, m_eventListener, nullptr);
  }

  m_instance = instance;
  m_inputConfig = m_instance->inputConfig();
  m_eventListener = m_instance->eventListener();
  emit instanceChanged();

  refetchLayouts();

  QObject::connect(m_instance, &QObject::destroyed, this,
                   [this]() { deleteLater(); });
  QObject::connect(m_instance, &myqmlplugin::HyprExtras::kbdLayoutIndexChanged,
                   this, [this]() {
                     m_selectedId = m_instance->kbdLayoutIndex();
                     if (m_dirtyFields.idx == true) {
                       m_dirtyFields.idx = false;
                       emit isDirtyChanged();
                     }
                   });
  QObject::connect(m_inputConfig, &myqmlplugin::HyprInputConfig::layoutsChanged,
                   this, &KeyboardSettingsBuffer::refetchLayouts);
  QObject::connect(m_eventListener,
                   &myqmlplugin::HyprEvents::keyboardLayoutChanged, this,
                   [this]() { refetchLayouts(); });
}

void KeyboardSettingsBuffer::refetchLayouts() {
  if (m_instance == nullptr || m_inputConfig == nullptr)
    return;

  auto listBuf = m_inputConfig->layoutList();

  m_dirtyFields = {.idx = false, .layouts = false};

  emit isDirtyChanged();

  if (listBuf.count() == m_layouts.count()) {
    bool isEqual = false;
    for (int i = 0; i < listBuf.count() - 1; ++i) {
      if ((listBuf.at(i)->layout() != m_layouts.at(i)->variant()) ||
          (listBuf.at(i)->variant() != m_layouts.at(i)->variant())) {
        isEqual = true;
        break;
      }
    }
    if (isEqual)
      return;
  }

  for (const auto l : m_layouts) {
    l->deleteLater();
  }

  m_layouts.clear();

  beginResetModel();

  for (const auto litem : listBuf) {
    auto hl = new KeyboardLayoutItem(litem->layout(), litem->variant(),
                                     litem->description(), this);
    m_layouts.append(hl);
  }

  endResetModel();

  emit layoutsChanged();

  m_selectedId = m_instance->kbdLayoutIndex();
  emit selectedIdChanged();
}

void KeyboardSettingsBuffer::resetForm() { refetchLayouts(); }

int KeyboardSettingsBuffer::addLayout(const QString &name,
                                      const QString &variant) {
  QVariantMap response;
  bool isDuplicate = false;

  for (const auto l : m_layouts) {
    if (l->layout() == name && l->variant() == variant) {
      isDuplicate = true;
      break;
    }
  }
  if (isDuplicate)
    return ReturnCode::DuplicatedLayout;

  auto l = m_instance->getLayout(name, variant);

  if (!l.has_value()) {
    return ReturnCode::LayoutDoesNotExist;
  }

  auto layout = l.value();

  auto newLayout = new KeyboardLayoutItem(layout.layout, layout.variant,
                                          layout.description, this);

  beginInsertRows({}, m_layouts.size(), m_layouts.size());
  m_layouts.append(newLayout);
  endInsertRows();

  emit layoutsChanged();

  m_dirtyFields.layouts = true;
  emit isDirtyChanged();

  return ReturnCode::Success;
}

int KeyboardSettingsBuffer::removeLayout(const QString &name,
                                         const QString &variant) {
  if (m_layouts.count() <= 1) {
    return ReturnCode::LastRemainingLayout;
  }

  KeyboardLayoutItem *foundLayout = nullptr;
  for (const auto l : m_layouts) {
    if (l->layout() == name && l->variant() == variant) {
      foundLayout = l;
      break;
    }
  }

  if (foundLayout == nullptr) {
    return ReturnCode::LayoutDoesNotExist;
  }

  foundLayout->deleteLater();
  auto lidx = m_layouts.indexOf(foundLayout);

  beginRemoveRows({}, lidx, lidx);
  m_layouts.removeAt(lidx);
  m_dirtyFields.layouts = true;
  endRemoveRows();
  emit layoutsChanged();

  if (m_selectedId > m_layouts.size() - 1) {
    m_selectedId = m_layouts.size() - 1;
    m_dirtyFields.idx = true;
    emit selectedIdChanged();
  }

  emit isDirtyChanged();

  return ReturnCode::Success;
}

int KeyboardSettingsBuffer::removeLayoutAtIndex(const int &index) {
  QVariantMap response;

  if (m_layouts.count() <= 1) {
    return ReturnCode::LastRemainingLayout;
  }

  if (index < 0 || index > m_layouts.count() - 1) {
    return ReturnCode::IndexOutOfBounds;
  }

  beginRemoveRows({}, index, index);
  m_layouts.at(index)->deleteLater();
  m_layouts.removeAt(index);
  endRemoveRows();
  m_dirtyFields.layouts = true;
  emit layoutsChanged();

  if (m_selectedId > m_layouts.size() - 1) {
    m_selectedId = m_layouts.size() - 1;
    m_dirtyFields.idx = true;
    emit selectedIdChanged();
  }

  emit isDirtyChanged();

  return ReturnCode::Success;
}

void KeyboardSettingsBuffer::swapItems(int from, int to) {
  if (from == to)
    return;
  if (from < 0 || from >= m_layouts.size())
    return;
  if (to < 0 || to >= m_layouts.size())
    return;

  beginMoveRows({}, from, from, {}, to > from ? to + 1 : to);
  m_layouts.swapItemsAt(from, to);
  endMoveRows();

  if (m_selectedId == from) {
    setSelectedId(to);
  } else if (m_selectedId == to) {
    setSelectedId(from);
  }

  m_dirtyFields.idx = true;
  m_dirtyFields.layouts = true;

  emit isDirtyChanged();

  emit layoutsChanged();
}

void KeyboardSettingsBuffer::moveItemToEnd(int index) {
  if (index < 0 || index >= m_layouts.size())
    return;

  swapItems(index, m_layouts.size() - 1);
}

void KeyboardSettingsBuffer::applyChanges() {
  if (m_instance == nullptr)
    return;
  if (!m_dirtyFields.idx && !m_dirtyFields.layouts)
    return;

  std::optional<QList<std::pair<QString, QString>>> optLayouts;
  std::optional<int> optInt;

  if (m_dirtyFields.layouts) {
    QList<std::pair<QString, QString>> newLayouts(m_layouts.size(),
                                                  Qt::Uninitialized);
    for (int i = 0; i < m_layouts.size(); ++i) {
      std::pair<QString, QString> p;
      p.first = m_layouts.at(i)->layout();
      p.second = m_layouts.at(i)->variant();
      newLayouts[i] = std::move(p);
    }
    optLayouts = std::move(newLayouts);
  }

  if (m_dirtyFields.idx) {
    optInt = m_selectedId;
  }

  m_instance->changeSettings(optLayouts, optInt);
}
} // namespace mscp
