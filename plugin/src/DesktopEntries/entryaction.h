#pragma once

#include <qcontainerfwd.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace ns::desktop::entries {
class DesktopEntry;

class EntryAction : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QString id READ getId CONSTANT)

  Q_PROPERTY(QString name READ default WRITE default NOTIFY nameChanged BINDABLE
                 bindableName)
  Q_PROPERTY(QString icon READ default WRITE default NOTIFY iconChanged BINDABLE
                 bindableIcon)
  Q_PROPERTY(QStringList command READ default WRITE default NOTIFY
                 commandChanged BINDABLE bindableCommand)

public:
  explicit EntryAction(QString       id,
                       DesktopEntry *entry = nullptr);

  Q_INVOKABLE void execute();
  DesktopEntry    *entry();

  [[nodiscard]] QString getId() const;

  [[nodiscard]] QBindable<QString>     bindableName() const { return &m_name; }
  [[nodiscard]] QBindable<QString>     bindableIcon() const { return &m_icon; }
  [[nodiscard]] QBindable<QStringList> bindableCommand() const {
    return &m_command;
  }

signals:
  void nameChanged();
  void iconChanged();
  void commandChanged();

private:
  DesktopEntry *m_entry;
  QString       m_id;

  Q_OBJECT_BINDABLE_PROPERTY(ns::desktop::entries::EntryAction,
                             QString,
                             m_name,
                             &EntryAction::nameChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::desktop::entries::EntryAction,
                             QString,
                             m_icon,
                             &EntryAction::iconChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::desktop::entries::EntryAction,
                             QStringList,
                             m_command,
                             &EntryAction::commandChanged)

  friend class DesktopEntry;
};
} // namespace ns::desktop::entries
