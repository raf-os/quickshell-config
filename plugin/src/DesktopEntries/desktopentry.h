#pragma once

#include "entrymanager.h"
#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace ns::desktopentries {
class DesktopEntry : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QString name READ default WRITE default NOTIFY nameChanged BINDABLE
                 bindableName)
  Q_PROPERTY(QString icon READ default WRITE default NOTIFY iconChanged BINDABLE
                 bindableIcon)
  Q_PROPERTY(bool noDisplay READ default WRITE default NOTIFY noDisplayChanged
                 BINDABLE bindableNoDisplay)

public:
  explicit DesktopEntry(QObject *parent = nullptr);

  void updateState(const EntryData &newState);

  [[nodiscard]] QBindable<QString> bindableName() const {
    return &this->m_name;
  }
  [[nodiscard]] QBindable<QString> bindableIcon() const {
    return &this->m_icon;
  }
  [[nodiscard]] QBindable<bool> bindableNoDisplay() const {
    return &this->m_noDisplay;
  }

signals:
  void nameChanged();
  void iconChanged();
  void noDisplayChanged();

private:
  void updateActions(const QList<EntryActionData> &newActions);

  Q_OBJECT_BINDABLE_PROPERTY(ns::desktopentries::DesktopEntry,
                             QString,
                             m_name,
                             &DesktopEntry::nameChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::desktopentries::DesktopEntry,
                             QString,
                             m_icon,
                             &DesktopEntry::iconChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::desktopentries::DesktopEntry,
                             bool,
                             m_noDisplay,
                             &DesktopEntry::noDisplayChanged)

  EntryData m_state;
};
} // namespace ns::desktopentries
