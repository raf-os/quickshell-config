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

  enum ReturnCode {
    Success,
    LastRemainingLayout,
    IndexOutOfBounds,
    LayoutDoesNotExist,
    DuplicatedLayout,
    UnknownError
  };
  Q_ENUM(ReturnCode)

  [[nodiscard]] QList<myqmlplugin::HyprKeyboardLayout *> layouts() const;

  [[nodiscard]] myqmlplugin::HyprExtras *instance() const;
  void setInstance(myqmlplugin::HyprExtras *instance);

  [[nodiscard]] int selectedId() const;
  void setSelectedId(const int &value);

  Q_SLOT void refetchLayouts();

  Q_INVOKABLE int addLayout(const QString &name, const QString &variant);
  Q_INVOKABLE int removeLayout(const QString &name, const QString &variant);
  Q_INVOKABLE int removeLayoutAtIndex(const int &index);
  Q_INVOKABLE void applyChanges();

signals:
  void layoutsChanged();
  void instanceChanged();
  void selectedIdChanged();

private:
  int m_selectedId;
  QList<myqmlplugin::HyprKeyboardLayout *> m_layouts;
  myqmlplugin::HyprExtras *m_instance = nullptr;
  myqmlplugin::HyprInputConfig *m_inputConfig = nullptr;

  struct {
    bool idx;
    bool layouts;
  } m_dirtyFields;

  void cleanup();
};
} // namespace mscp
