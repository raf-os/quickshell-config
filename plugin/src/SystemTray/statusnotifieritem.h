#pragma once

#include <qdbusextratypes.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qsize.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "dbus_item.h"
#include "dbustypes.h"

namespace ns::systemtray {
namespace Status {
Q_NAMESPACE
QML_ELEMENT

enum Enum : quint8 { Passive = 0, Active = 1, NeedsAttention = 2 };

Q_ENUM_NS(Enum)

Status::Enum fromString(const QString &value);
} // namespace Status

namespace Category {
Q_NAMESPACE
QML_ELEMENT

enum Enum : quint8 {
  Hardware          = 0,
  SystemServices    = 1,
  ApplicationStatus = 2,
  Communications    = 3
};

Q_ENUM_NS(Enum)

Category::Enum fromString(const QString &value);
} // namespace Category

class StatusNotifierItem : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString id READ default NOTIFY idChanged BINDABLE bindableId)
  Q_PROPERTY(ns::systemtray::Category::Enum category
                 READ default NOTIFY categoryChanged BINDABLE bindableCategory)
  Q_PROPERTY(
      QString title READ default NOTIFY titleChanged BINDABLE bindableTitle)
  Q_PROPERTY(ns::systemtray::Status::Enum status
                 READ default NOTIFY statusChanged BINDABLE bindableStatus)
  Q_PROPERTY(QDBusObjectPath menuPath READ default NOTIFY menuPathChanged
                 BINDABLE bindableMenuPath)
  Q_PROPERTY(quint32 windowId READ default NOTIFY windowIdChanged BINDABLE
                 bindableWindowId)
  Q_PROPERTY(QString iconThemePath READ default NOTIFY iconThemePathChanged
                 BINDABLE bindableIconThemePath)
  Q_PROPERTY(QString iconUrl READ default NOTIFY iconUrlChanged BINDABLE
                 bindableIconUrl)
  Q_PROPERTY(QString iconName READ default NOTIFY iconNameChanged BINDABLE
                 bindableIconName)
  Q_PROPERTY(QString attentionIconName READ default NOTIFY
                 attentionIconNameChanged BINDABLE bindableAttentionIconName)
  Q_PROPERTY(QString overlayIconName READ default NOTIFY overlayIconNameChanged
                 BINDABLE bindableOverlayIconName)
  Q_PROPERTY(QString tooltipTitle READ default NOTIFY tooltipTitleChanged
                 BINDABLE bindableTooltipTitle)
  Q_PROPERTY(QString tooltipDescription READ default NOTIFY
                 tooltipDescriptionChanged BINDABLE bindableTooltipDescription)
  Q_PROPERTY(
      bool hasMenu READ default NOTIFY hasMenuChanged BINDABLE bindableHasMenu)
  Q_PROPERTY(bool isMenuOnly READ default NOTIFY isMenuOnlyChanged BINDABLE
                 bindableIsMenuOnly)

public:
  explicit StatusNotifierItem(const QString &address,
                              QObject       *parent = nullptr);

  [[nodiscard]] bool isValid() const;
  [[nodiscard]] bool isReady() const;

  void createPixmap(const QSize &size);

  [[nodiscard]] QBindable<QString>        bindableId() const { return &b_id; }
  [[nodiscard]] QBindable<Category::Enum> bindableCategory() const {
    return &b_category;
  }
  [[nodiscard]] QBindable<QString> bindableTitle() const { return &b_title; }
  [[nodiscard]] QBindable<Status::Enum> bindableStatus() const {
    return &b_status;
  }
  [[nodiscard]] QBindable<QDBusObjectPath> bindableMenuPath() const {
    return &b_menuPath;
  }
  [[nodiscard]] QBindable<quint32> bindableWindowId() const {
    return &b_windowId;
  }
  [[nodiscard]] QBindable<QString> bindableIconThemePath() const {
    return &b_iconThemePath;
  }
  [[nodiscard]] QBindable<QString> bindableIconUrl() const {
    return &b_iconUrl;
  }
  [[nodiscard]] QBindable<QString> bindableIconName() const {
    return &b_iconName;
  }
  [[nodiscard]] QBindable<QString> bindableAttentionIconName() const {
    return &b_attentionIconName;
  }
  [[nodiscard]] QBindable<QString> bindableOverlayIconName() const {
    return &b_overlayIconName;
  }
  [[nodiscard]] QBindable<QString> bindableTooltipTitle() const {
    return &b_tooltipTitle;
  }
  [[nodiscard]] QBindable<QString> bindableTooltipDescription() const {
    return &b_tooltipDescription;
  }
  [[nodiscard]] QBindable<bool> bindableHasMenu() const { return &b_hasMenu; }
  [[nodiscard]] QBindable<bool> bindableIsMenuOnly() const {
    return &b_isMenuOnly;
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
  void iconUrlChanged();
  void iconNameChanged();
  void attentionIconNameChanged();
  void overlayIconNameChanged();
  void tooltipTitleChanged();
  void tooltipDescriptionChanged();
  void hasMenuChanged();
  void isMenuOnlyChanged();

private slots:
  void readAllParameters();
  void readTooltip();
  void readIconData();

private:
  bool                     m_isReady = false;
  QDBusStatusNotifierItem *m_item    = nullptr;
  QString                  m_watcherId;

  DBusTrayIconPixmapList m_pixmapList;
  DBusTrayIconPixmapList m_attentionPixmapList;
  DBusTrayIconPixmapList m_overlayPixmapList;

  Q_OBJECT_BINDABLE_PROPERTY(StatusNotifierItem,
                             QString,
                             b_id,
                             &StatusNotifierItem::idChanged)
  Q_OBJECT_BINDABLE_PROPERTY(StatusNotifierItem,
                             Category::Enum,
                             b_category,
                             &StatusNotifierItem::categoryChanged)
  Q_OBJECT_BINDABLE_PROPERTY(StatusNotifierItem,
                             QString,
                             b_title,
                             &StatusNotifierItem::titleChanged)
  Q_OBJECT_BINDABLE_PROPERTY(StatusNotifierItem,
                             Status::Enum,
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
                             b_iconUrl,
                             &StatusNotifierItem::iconUrlChanged)
  Q_OBJECT_BINDABLE_PROPERTY(StatusNotifierItem,
                             QString,
                             b_iconName,
                             &StatusNotifierItem::iconNameChanged)
  Q_OBJECT_BINDABLE_PROPERTY(StatusNotifierItem,
                             QString,
                             b_attentionIconName,
                             &StatusNotifierItem::attentionIconNameChanged)
  Q_OBJECT_BINDABLE_PROPERTY(StatusNotifierItem,
                             QString,
                             b_overlayIconName,
                             &StatusNotifierItem::overlayIconNameChanged)
  Q_OBJECT_BINDABLE_PROPERTY(StatusNotifierItem,
                             QString,
                             b_tooltipTitle,
                             &StatusNotifierItem::tooltipTitleChanged)
  Q_OBJECT_BINDABLE_PROPERTY(StatusNotifierItem,
                             QString,
                             b_tooltipDescription,
                             &StatusNotifierItem::tooltipDescriptionChanged)
  Q_OBJECT_BINDABLE_PROPERTY(StatusNotifierItem,
                             bool,
                             b_hasMenu,
                             &StatusNotifierItem::hasMenuChanged)
  Q_OBJECT_BINDABLE_PROPERTY(StatusNotifierItem,
                             bool,
                             b_isMenuOnly,
                             &StatusNotifierItem::isMenuOnlyChanged)
};
} // namespace ns::systemtray
