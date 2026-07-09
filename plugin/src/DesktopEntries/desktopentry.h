#pragma once

#include "entryaction.h"
#include "entryscanner.h"

#include <qcontainerfwd.h>
#include <qlist.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qtypes.h>

namespace ns::desktop::entries {
class DesktopEntry : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QString id READ id CONSTANT)
  Q_PROPERTY(QString name READ default WRITE default NOTIFY nameChanged BINDABLE
                 bindableName)
  Q_PROPERTY(QString genericName READ default WRITE default NOTIFY
                 genericNameChanged BINDABLE bindableGenericName)
  Q_PROPERTY(QString startupClass READ default WRITE default NOTIFY
                 startupClassChanged BINDABLE bindableStartupClass)
  Q_PROPERTY(QString icon READ default WRITE default NOTIFY iconChanged BINDABLE
                 bindableIcon)
  Q_PROPERTY(QString comment READ default WRITE default NOTIFY commentChanged
                 BINDABLE bindableComment)
  Q_PROPERTY(QString execString READ default WRITE default NOTIFY
                 execStringChanged BINDABLE bindableExecString)
  Q_PROPERTY(QStringList command READ default WRITE default NOTIFY
                 commandChanged BINDABLE bindableCommand)
  Q_PROPERTY(QStringList categories READ default WRITE default NOTIFY
                 categoriesChanged BINDABLE bindableCategories)
  Q_PROPERTY(QStringList keywords READ default WRITE default NOTIFY
                 keywordsChanged BINDABLE bindableKeywords)
  Q_PROPERTY(QString workingDirectory READ default WRITE default NOTIFY
                 workingDirectoryChanged BINDABLE bindableWorkingDirectory)
  Q_PROPERTY(bool runInTerminal READ default WRITE default NOTIFY
                 runInTerminalChanged BINDABLE bindableRunInTerminal)
  Q_PROPERTY(bool hidden READ default WRITE default NOTIFY hiddenChanged
                 BINDABLE bindableHidden)
  Q_PROPERTY(bool noDisplay READ default WRITE default NOTIFY noDisplayChanged
                 BINDABLE bindableNoDisplay)
  Q_PROPERTY(quint32 frequency READ default WRITE default NOTIFY
                 frequencyChanged BINDABLE bindableFrequency)

public:
  explicit DesktopEntry(QString  m_id,
                        QObject *parent = nullptr);

  void                  updateState(const EntryData &newState);
  bool                  isValid();
  [[nodiscard]] QString id() const;

  Q_INVOKABLE void execute();

  // clang-format off
  [[nodiscard]] QBindable<QString> bindableName() const { return &this->m_name; }
  [[nodiscard]] QBindable<QString> bindableGenericName() const { return &this->m_genericName; }
  [[nodiscard]] QBindable<QString> bindableStartupClass() const { return &this->m_startupClass; }
  [[nodiscard]] QBindable<QString> bindableIcon() const { return &this->m_icon; }
  [[nodiscard]] QBindable<QString> bindableComment() const { return &this->m_comment; }
  [[nodiscard]] QBindable<QString> bindableExecString() const { return &this->m_execString; }
  [[nodiscard]] QBindable<QStringList> bindableCommand() const { return &this->m_command; }
  [[nodiscard]] QBindable<QString> bindableWorkingDirectory() const { return &this->m_workingDirectory; }
  [[nodiscard]] QBindable<QStringList> bindableCategories() const { return &this->m_categories; }
  [[nodiscard]] QBindable<QStringList> bindableKeywords() const { return &this->m_keywords; }
  [[nodiscard]] QBindable<bool> bindableRunInTerminal() const { return &this->m_runInTerminal; }
  [[nodiscard]] QBindable<bool> bindableHidden() const { return &this->m_hidden; }
  [[nodiscard]] QBindable<bool> bindableNoDisplay() const { return &this->m_noDisplay; }
  [[nodiscard]] QBindable<quint32> bindableFrequency() const { return &this->m_frequency; }
  // clang-format on

  void setFrequency(quint32 freq);

public slots:
  void incrementFrequency();

signals:
  void nameChanged();
  void genericNameChanged();
  void startupClassChanged();
  void iconChanged();
  void commentChanged();
  void execStringChanged();
  void commandChanged();
  void workingDirectoryChanged();
  void categoriesChanged();
  void keywordsChanged();
  void runInTerminalChanged();
  void hiddenChanged();
  void noDisplayChanged();
  void frequencyChanged();

  void isFavoriteChanged();

private:
  void updateActions(const QList<EntryActionData> &newActions);

  Q_OBJECT_BINDABLE_PROPERTY(ns::desktop::entries::DesktopEntry,
                             QString,
                             m_name,
                             &DesktopEntry::nameChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::desktop::entries::DesktopEntry,
                             QString,
                             m_genericName,
                             &DesktopEntry::genericNameChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::desktop::entries::DesktopEntry,
                             QString,
                             m_startupClass,
                             &DesktopEntry::startupClassChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::desktop::entries::DesktopEntry,
                             QString,
                             m_icon,
                             &DesktopEntry::iconChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::desktop::entries::DesktopEntry,
                             QString,
                             m_comment,
                             &DesktopEntry::commentChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::desktop::entries::DesktopEntry,
                             QString,
                             m_execString,
                             &DesktopEntry::execStringChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::desktop::entries::DesktopEntry,
                             QStringList,
                             m_command,
                             &DesktopEntry::commandChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::desktop::entries::DesktopEntry,
                             QString,
                             m_workingDirectory,
                             &DesktopEntry::workingDirectoryChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::desktop::entries::DesktopEntry,
                             QStringList,
                             m_categories,
                             &DesktopEntry::categoriesChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::desktop::entries::DesktopEntry,
                             QStringList,
                             m_keywords,
                             &DesktopEntry::keywordsChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::desktop::entries::DesktopEntry,
                             bool,
                             m_runInTerminal,
                             &DesktopEntry::runInTerminalChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::desktop::entries::DesktopEntry,
                             bool,
                             m_hidden,
                             &DesktopEntry::hiddenChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::desktop::entries::DesktopEntry,
                             bool,
                             m_noDisplay,
                             &DesktopEntry::noDisplayChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::desktop::entries::DesktopEntry,
                             quint32,
                             m_frequency,
                             &DesktopEntry::frequencyChanged)

  QString              m_id;
  EntryData            m_state;
  QList<EntryAction *> m_actions;
};
} // namespace ns::desktop::entries
