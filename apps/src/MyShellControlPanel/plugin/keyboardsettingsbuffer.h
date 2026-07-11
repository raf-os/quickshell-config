#pragma once

#include <qabstractitemmodel.h>
#include <qcontainerfwd.h>
#include <qhash.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qstringview.h>
#include <qtmetamacros.h>

#include "hypr.h"
#include "hyprevents.h"

namespace mscp {
class KeyboardLayoutItem : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QString layout READ layout CONSTANT)
  Q_PROPERTY(QString variant READ variant CONSTANT)
  Q_PROPERTY(QString description READ description CONSTANT)

public:
  explicit KeyboardLayoutItem(const QString &layout,
                              const QString &variant,
                              const QString &description,
                              QObject       *parent = nullptr)
      : QObject(parent),
        m_layout(layout),
        m_variant(variant),
        m_description(description) {}

  [[nodiscard]] QString layout() const { return m_layout; }
  [[nodiscard]] QString variant() const { return m_variant; }
  [[nodiscard]] QString description() const { return m_description; }

private:
  QString m_layout;
  QString m_variant;
  QString m_description;
};

class KeyboardSettingsBuffer : public QAbstractListModel {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QQmlListProperty<mscp::KeyboardLayoutItem> layouts READ layouts
                 NOTIFY layoutsChanged)
  Q_PROPERTY(myqmlplugin::HyprExtras *instance READ instance WRITE setInstance
                 NOTIFY instanceChanged)
  Q_PROPERTY(int selectedId READ selectedId WRITE setSelectedId NOTIFY
                 selectedIdChanged)
  Q_PROPERTY(bool isDirty READ isDirty NOTIFY isDirtyChanged)

public:
  explicit KeyboardSettingsBuffer(QObject *parent = nullptr);

  enum Roles { ModelDataRole = Qt::UserRole + 1 };

  enum ReturnCode {
    Success,
    LastRemainingLayout,
    IndexOutOfBounds,
    LayoutDoesNotExist,
    DuplicatedLayout,
    UnknownError
  };
  Q_ENUM(ReturnCode)

  int      rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index,
                int                role) const override;
  QHash<int,
        QByteArray>
  roleNames() const override {
    return {
        {Roles::ModelDataRole, "modelData"}
    };
  }

  [[nodiscard]] QQmlListProperty<KeyboardLayoutItem> layouts();

  [[nodiscard]] bool isDirty() const;

  [[nodiscard]] myqmlplugin::HyprExtras *instance() const;
  void setInstance(myqmlplugin::HyprExtras *instance);

  [[nodiscard]] int selectedId() const;
  void              setSelectedId(const int &value);

  Q_SLOT void refetchLayouts();

  Q_INVOKABLE int  addLayout(const QString &name,
                             const QString &variant);
  Q_INVOKABLE int  removeLayout(const QString &name,
                                const QString &variant);
  Q_INVOKABLE int  removeLayoutAtIndex(const int &index);
  Q_INVOKABLE void applyChanges();
  Q_INVOKABLE void resetForm();

  Q_INVOKABLE void swapItems(int from,
                             int to);
  Q_INVOKABLE void moveItemToEnd(int index);

signals:
  void layoutsChanged();
  void instanceChanged();
  void selectedIdChanged();
  void isDirtyChanged();

private:
  int                           m_selectedId;
  QList<KeyboardLayoutItem *>   m_layouts;
  myqmlplugin::HyprExtras      *m_instance      = nullptr;
  myqmlplugin::HyprInputConfig *m_inputConfig   = nullptr;
  ns::hyprland::HyprEvents     *m_eventListener = nullptr;

  struct {
    bool idx;
    bool layouts;
  } m_dirtyFields;

  void cleanup();
};
} // namespace mscp
