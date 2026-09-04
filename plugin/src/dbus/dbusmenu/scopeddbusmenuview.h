#pragma once

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qvariant.h>

#include "dbusmenumodel.h"

namespace ns::dbusmenu {
class ScopedDBusMenuView : public QObject {
  // Provides a scoped reference count algorithm for the QML side, as well as
  // some sort of async support for loading sub-menus
  //
  // Should be used alongside the DelegateModel QML type to actually display the
  // sub-items

  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QVariant modelIndex READ modelIndex WRITE setModelIndex NOTIFY
          modelIndexChanged)
  Q_PROPERTY(ns::dbusmenu::DBusMenuModel *model READ model WRITE setModel NOTIFY
          modelChanged)
  Q_PROPERTY(
      bool loaded READ default NOTIFY loadedChanged BINDABLE bindableLoaded)

public:
  explicit ScopedDBusMenuView(QObject *parent = nullptr);
  ~ScopedDBusMenuView() override;

  // No such thing as a write-only property, so this just returns a default
  // constructed QVariant. QModelIndex is supposed to be short lived, and using
  // a persistent index is more trouble than it's worth
  [[nodiscard]] QVariant modelIndex() const { return {}; }
  // Must be convertible to QModelIndex.
  void setModelIndex(const QVariant &index);

  [[nodiscard]] DBusMenuModel *model();
  void                         setModel(DBusMenuModel *newModel);

  [[nodiscard]] QBindable<bool> bindableLoaded() const { return &b_loaded; }

signals:
  void modelIndexChanged();
  void modelChanged();
  void loadedChanged();

private slots:
  void onModelDestroyed();
  void onItemDestroyed();
  void onLoadedComplete();

private:
  bool               m_isValid   = false;
  bool               m_isPending = false;
  DBusMenuModel     *m_model     = nullptr;
  DBusMenuModelItem *m_menu      = nullptr;

  void tryAttach();

  Q_OBJECT_BINDABLE_PROPERTY(
      ScopedDBusMenuView, bool, b_loaded, &ScopedDBusMenuView::loadedChanged);
};
} // namespace ns::dbusmenu
