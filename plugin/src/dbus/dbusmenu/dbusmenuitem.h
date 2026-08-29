#pragma once

#include <QtGui/qpixmap.h>
#include <qcontainerfwd.h>
#include <qlist.h>
#include <qobject.h>
#include <qproperty.h>
#include <qsize.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "dbusimage.h"
#include "dbusmenu.h"

namespace ns::dbusmenu {
namespace ItemDisposition {
Q_NAMESPACE

enum Enum : quint8 { Normal = 0, Informative = 1, Warning = 2, Alert = 3 };

Q_ENUM_NS(Enum)

ItemDisposition::Enum fromString(const QString &str);
} // namespace ItemDisposition

namespace ItemToggleType {
Q_NAMESPACE

enum Enum : quint8 {
  None      = 0,
  Checkmark = 1,
  Radio     = 2,
};

Q_ENUM_NS(Enum)

ItemToggleType::Enum fromString(const QString &str);
} // namespace ItemToggleType

namespace ItemCheckState {
Q_NAMESPACE

enum Enum : quint8 { Unchecked = 0, Checked = 1, PartiallyChecked = 2 };

Q_ENUM_NS(Enum)
} // namespace ItemCheckState

struct ItemData {
  bool                  enabled     = true;
  bool                  visible     = true;
  ItemToggleType::Enum  toggleType  = ItemToggleType::None;
  ItemDisposition::Enum disposition = ItemDisposition::Normal;
  QString               type        = "standard";
  QString               label;
  QString               iconName;
  QString               childrenDisplay;
  QList<QStringList>    shortcut;
  qint32                toggleState;
  QByteArray            iconData;
};

class DBusMenuPngImage : public dbusprovider::BaseIndexedImageHandle {
public:
  [[nodiscard]] bool hasData() const { return !this->data.isEmpty(); }

  QPixmap requestPixmap(
      const QString &id, QSize *size, const QSize &requestedSize) override;

  QByteArray data;
};

class DBusMenuItem : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool isSeparator READ default NOTIFY isSeparatorChanged BINDABLE
          bindableIsSeparator)
  Q_PROPERTY(
      bool enabled READ default NOTIFY enabledChanged BINDABLE bindableEnabled)
  Q_PROPERTY(
      bool visible READ default NOTIFY visibleChanged BINDABLE bindableVisible)
  Q_PROPERTY(bool hasChildren READ default NOTIFY hasChildrenChanged BINDABLE
          bindableHasChildren)
  Q_PROPERTY(
      QString label READ default NOTIFY labelChanged BINDABLE bindableLabel)
  Q_PROPERTY(QString icon READ default NOTIFY iconChanged BINDABLE bindableIcon)
  Q_PROPERTY(ns::dbusmenu::ItemToggleType::Enum toggleType
          READ default NOTIFY toggleTypeChanged BINDABLE bindableToggleType)
  Q_PROPERTY(ns::dbusmenu::ItemDisposition::Enum disposition
          READ default NOTIFY dispositionChanged BINDABLE bindableDisposition)
  Q_PROPERTY(ns::dbusmenu::ItemCheckState::Enum checkState
          READ default NOTIFY checkStateChanged BINDABLE bindableCheckState)
public:
  explicit DBusMenuItem(qint32 id, DBusMenu *menuHandler, DBusMenuItem *parent);

  void updateProperties(
      const QVariantMap &properties, const QStringList &removedItems = {});

  [[nodiscard]] QBindable<bool> bindableIsSeparator() const {
    return &b_isSeparator;
  }
  [[nodiscard]] QBindable<bool> bindableEnabled() const { return &b_enabled; }
  [[nodiscard]] QBindable<bool> bindableVisible() const { return &b_visible; }
  [[nodiscard]] QBindable<bool> bindableHasChildren() const {
    return &b_hasChildren;
  }
  [[nodiscard]] QBindable<QString> bindableText() const { return &b_text; }
  [[nodiscard]] QBindable<QString> bindableLabel() const { return &b_label; }
  [[nodiscard]] QBindable<QString> bindableIcon() const { return &b_icon; }
  [[nodiscard]] QBindable<ItemToggleType::Enum> bindableToggleType() const {
    return &b_toggleType;
  }
  [[nodiscard]] QBindable<ItemDisposition::Enum> bindableDisposition() const {
    return &b_disposition;
  }
  [[nodiscard]] QBindable<ItemCheckState::Enum> bindableCheckState() const {
    return &b_checkState;
  }

signals:
  void layoutUpdated();

  void isSeparatorChanged();
  void enabledChanged();
  void visibleChanged();
  void hasChildrenChanged();
  void labelChanged();
  void iconChanged();
  void toggleTypeChanged();
  void dispositionChanged();
  void checkStateChanged();

private slots:
  void onChildrenUpdated();

private:
  bool          m_showChildren;
  bool          m_childrenLoaded;
  qint32        m_id;
  DBusMenu     *m_menuHandler = nullptr;
  DBusMenuItem *m_parentMenu  = nullptr;
  QString       m_rawLabel;

  DBusMenuPngImage m_image;
  QList<qint32>    m_children;

  Q_OBJECT_BINDABLE_PROPERTY(
      DBusMenuItem, bool, b_isSeparator, &DBusMenuItem::isSeparatorChanged)
  Q_OBJECT_BINDABLE_PROPERTY(
      DBusMenuItem, bool, b_enabled, &DBusMenuItem::enabledChanged)
  Q_OBJECT_BINDABLE_PROPERTY(
      DBusMenuItem, bool, b_visible, &DBusMenuItem::visibleChanged)
  Q_OBJECT_BINDABLE_PROPERTY(
      DBusMenuItem, bool, b_hasChildren, &DBusMenuItem::hasChildrenChanged)
  Q_OBJECT_BINDABLE_PROPERTY(DBusMenuItem, QString, b_text)
  Q_OBJECT_BINDABLE_PROPERTY(DBusMenuItem, QString, b_iconName)
  Q_OBJECT_BINDABLE_PROPERTY(
      DBusMenuItem, QString, b_label, &ns::dbusmenu::DBusMenuItem::labelChanged)
  Q_OBJECT_BINDABLE_PROPERTY(
      DBusMenuItem, QString, b_icon, &DBusMenuItem::iconChanged)
  Q_OBJECT_BINDABLE_PROPERTY(DBusMenuItem, ItemToggleType::Enum, b_toggleType,
      &DBusMenuItem::toggleTypeChanged)
  Q_OBJECT_BINDABLE_PROPERTY(DBusMenuItem, ItemDisposition::Enum, b_disposition,
      &DBusMenuItem::dispositionChanged)
  Q_OBJECT_BINDABLE_PROPERTY(DBusMenuItem, ItemCheckState::Enum, b_checkState,
      &DBusMenuItem::checkStateChanged)

  friend class DBusMenu;
};
} // namespace ns::dbusmenu
