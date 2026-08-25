#pragma once

#include <qdbusextratypes.h>
#include <qobject.h>
#include <qproperty.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "dbus_item.h"

namespace ns::systemtray {
class StatusNotifierItem : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString id READ default NOTIFY idChanged BINDABLE bindableId)
  Q_PROPERTY(QString category READ default NOTIFY categoryChanged BINDABLE
                 bindableCategory)
  Q_PROPERTY(
      QString title READ default NOTIFY titleChanged BINDABLE bindableTitle)
  Q_PROPERTY(
      QString status READ default NOTIFY statusChanged BINDABLE bindableStatus)
  Q_PROPERTY(QDBusObjectPath menuPath READ default NOTIFY menuPathChanged
                 BINDABLE bindableMenuPath)
  Q_PROPERTY(quint32 windowId READ default NOTIFY windowIdChanged BINDABLE
                 bindableWindowId)
  Q_PROPERTY(QString iconThemePath READ default NOTIFY iconThemePathChanged
                 BINDABLE bindableIconThemePath)
  Q_PROPERTY(QString iconName READ default NOTIFY iconNameChanged BINDABLE
                 bindableIconName)

public:
  explicit StatusNotifierItem(const QString &address,
                              QObject       *parent = nullptr);

  [[nodiscard]] bool isValid() const;
  [[nodiscard]] bool isReady() const;

  [[nodiscard]] QBindable<QString> bindableId() const { return &b_id; }
  [[nodiscard]] QBindable<QString> bindableCategory() const {
    return &b_category;
  }
  [[nodiscard]] QBindable<QString> bindableTitle() const { return &b_title; }
  [[nodiscard]] QBindable<QString> bindableStatus() const { return &b_status; }
  [[nodiscard]] QBindable<QDBusObjectPath> bindableMenuPath() const {
    return &b_menuPath;
  }
  [[nodiscard]] QBindable<quint32> bindableWindowId() const {
    return &b_windowId;
  }
  [[nodiscard]] QBindable<QString> bindableIconThemePath() const {
    return &b_iconThemePath;
  }
  [[nodiscard]] QBindable<QString> bindableIconName() const {
    return &b_iconName;
  }

signals:
  void ready();

  void idChanged();
  void categoryChanged();
  void titleChanged();
  void statusChanged();
  void menuPathChanged();
  void windowIdChanged();
  void iconThemePathChanged();
  void iconNameChanged();

private slots:
  void readAllParameters();

private:
  bool                     m_isReady = false;
  QDBusStatusNotifierItem *m_item    = nullptr;
  QString                  m_watcherId;

  Q_OBJECT_BINDABLE_PROPERTY(StatusNotifierItem,
                             QString,
                             b_id,
                             &StatusNotifierItem::idChanged)
  Q_OBJECT_BINDABLE_PROPERTY(StatusNotifierItem,
                             QString,
                             b_category,
                             &StatusNotifierItem::categoryChanged)
  Q_OBJECT_BINDABLE_PROPERTY(StatusNotifierItem,
                             QString,
                             b_title,
                             &StatusNotifierItem::titleChanged)
  Q_OBJECT_BINDABLE_PROPERTY(StatusNotifierItem,
                             QString,
                             b_status,
                             &StatusNotifierItem::statusChanged)
  Q_OBJECT_BINDABLE_PROPERTY(StatusNotifierItem,
                             QDBusObjectPath,
                             b_menuPath,
                             &StatusNotifierItem::menuPathChanged)
  Q_OBJECT_BINDABLE_PROPERTY(StatusNotifierItem,
                             quint32,
                             b_windowId,
                             &StatusNotifierItem::windowIdChanged)
  Q_OBJECT_BINDABLE_PROPERTY(StatusNotifierItem,
                             QString,
                             b_iconThemePath,
                             &StatusNotifierItem::iconThemePathChanged)
  Q_OBJECT_BINDABLE_PROPERTY(StatusNotifierItem,
                             QString,
                             b_iconName,
                             &StatusNotifierItem::iconNameChanged)
};
} // namespace ns::systemtray
