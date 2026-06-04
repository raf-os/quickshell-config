#include "keyboardsettingsbuffer.h"
#include "hypr.h"
#include <qcontainerfwd.h>
#include <qlist.h>
#include <qobject.h>

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
  emit selectedIdChanged();
}

myqmlplugin::HyprExtras *KeyboardSettingsBuffer::instance() const {
  return m_instance;
}
void KeyboardSettingsBuffer::setInstance(myqmlplugin::HyprExtras *instance) {
  if (m_instance == instance)
    return;

  if (m_instance != nullptr) {
    QObject::disconnect(m_instance, nullptr, this, nullptr);
  }

  if (m_inputConfig != nullptr) {
    QObject::disconnect(m_inputConfig, nullptr, this, nullptr);
  }

  m_instance = instance;
  m_inputConfig = m_instance->inputConfig();
  emit instanceChanged();

  QObject::connect(m_instance, &QObject::destroyed, this,
                   [this]() { deleteLater(); });
  QObject::connect(m_inputConfig, &QObject::destroyed, this,
                   [this]() { deleteLater(); });

  QObject::connect(m_inputConfig, &myqmlplugin::HyprInputConfig::layoutsChanged,
                   this, &KeyboardSettingsBuffer::refetchLayouts);
}

void KeyboardSettingsBuffer::refetchLayouts() {
  if (m_instance == nullptr || m_inputConfig == nullptr)
    return;

  auto listBuf = m_inputConfig->layoutList();

  if (listBuf == m_layouts)
    return;

  m_layouts = listBuf;
  emit layoutsChanged();

  m_selectedId = m_instance->kbdLayoutIndex();
  emit selectedIdChanged();
}

QVariantMap KeyboardSettingsBuffer::addLayout(const QString &name,
                                              const QString &variant) {
  QVariantMap response;
  return response;
}

QVariantMap KeyboardSettingsBuffer::removeLayoutAtIndex(const int &index) {
  QVariantMap response;

  if (m_layouts.count() <= 1) {
    response["success"] = false;
    response["message"] = "Cannot remove last keyboard layout!";
    return response;
  }

  if (index < 0 || index > m_layouts.count() - 1) {
    response["success"] = false;
    return response;
  }

  m_layouts.removeAt(index);
  emit layoutsChanged();

  response["success"] = true;
  return response;
}
} // namespace mscp
