#pragma once

#include "hypr.h"

#include <qcontainerfwd.h>
#include <qlist.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace mscp {
class KeyboardSettingsBuffer : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QList<myqmlplugin::HyprKeyboardLayout *> layouts READ layouts
                 NOTIFY layoutsChanged)
  Q_PROPERTY(myqmlplugin::HyprExtras *instance READ instance WRITE setInstance
                 NOTIFY instanceChanged)
  Q_PROPERTY(int selectedId READ selectedId WRITE setSelectedId NOTIFY
                 selectedIdChanged)

public:
  explicit KeyboardSettingsBuffer(QObject *parent = nullptr);

  [[nodiscard]] QList<myqmlplugin::HyprKeyboardLayout *> layouts() const;

  [[nodiscard]] myqmlplugin::HyprExtras *instance() const;
  void setInstance(myqmlplugin::HyprExtras *instance);

  [[nodiscard]] int selectedId() const;
  void setSelectedId(const int &value);

  Q_SLOT void refetchLayouts();

  Q_INVOKABLE QVariantMap addLayout(const QString &name,
                                    const QString &variant);
  Q_INVOKABLE QVariantMap removeLayoutAtIndex(const int &index);

signals:
  void layoutsChanged();
  void instanceChanged();
  void selectedIdChanged();

private:
  int m_selectedId;
  QList<myqmlplugin::HyprKeyboardLayout *> m_layouts;
  myqmlplugin::HyprExtras *m_instance;
  myqmlplugin::HyprInputConfig *m_inputConfig;
};
} // namespace mscp
