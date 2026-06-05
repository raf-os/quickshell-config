#include "keyboardsettingsbuffer.h"
#include "hypr.h"

#include <optional>
#include <qcontainerfwd.h>
#include <qlist.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <utility>

namespace mscp {
KeyboardSettingsBuffer::KeyboardSettingsBuffer(QObject *parent)
    : QObject(parent) {}

QList<myqmlplugin::HyprKeyboardLayout *>
KeyboardSettingsBuffer::layouts() const {
  return m_layouts;
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

  m_instance = instance;
  m_inputConfig = m_instance->inputConfig();
  emit instanceChanged();

  refetchLayouts();

  QObject::connect(m_inputConfig, &myqmlplugin::HyprInputConfig::layoutsChanged,
                   this, &KeyboardSettingsBuffer::refetchLayouts);
}

void KeyboardSettingsBuffer::cleanup() {
  m_instance = nullptr;
  m_inputConfig = nullptr;
  emit instanceChanged();
  deleteLater();
}

void KeyboardSettingsBuffer::refetchLayouts() {
  if (m_instance == nullptr || m_inputConfig == nullptr)
    return;

  auto listBuf = m_inputConfig->layoutList();
  if (listBuf == m_layouts) {
    return;
  }

  m_dirtyFields = {.idx = false, .layouts = false};

  if (listBuf.count() == m_layouts.count()) {
    bool isEqual = false;
    for (int i = 0; i < listBuf.count() - 1; ++i) {
      if (listBuf.at(i) == m_layouts.at(i))
        continue;

      if ((listBuf.at(i)->layout() != m_layouts.at(i)->variant()) ||
          (listBuf.at(i)->variant() != m_layouts.at(i)->variant())) {
        isEqual = true;
        break;
      }
    }
    if (isEqual)
      return;
  }

  m_layouts.clear();

  for (const auto l : m_ownedLayouts) {
    l->deleteLater();
  }

  m_layouts = listBuf;
  emit layoutsChanged();

  m_selectedId = m_instance->kbdLayoutIndex();
  emit selectedIdChanged();
}

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

  auto layout = m_instance->getLayout(name, variant, this);

  if (layout == nullptr)
    return ReturnCode::LayoutDoesNotExist;

  m_layouts.append(layout);
  m_ownedLayouts.append(layout);
  m_dirtyFields.layouts = true;

  emit layoutsChanged();

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

  if (const auto ownedLayoutIdx = m_ownedLayouts.indexOf(m_layouts.at(index));
      ownedLayoutIdx != -1) {
    m_ownedLayouts.at(ownedLayoutIdx)->deleteLater();
    m_ownedLayouts.removeAt(index);
  }

  m_layouts.removeAt(index);
  m_dirtyFields.layouts = true;
  emit layoutsChanged();

  if (m_selectedId > m_layouts.size() - 1) {
    m_selectedId = m_layouts.size() - 1;
    m_dirtyFields.idx = true;
    emit selectedIdChanged();
  }

  return ReturnCode(Success);
}

void KeyboardSettingsBuffer::applyChanges() {
  if (m_instance == nullptr)
    return;
  if (!m_dirtyFields.idx || !m_dirtyFields.layouts)
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
