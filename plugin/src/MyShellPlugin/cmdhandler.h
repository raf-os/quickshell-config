#pragma once

#include <qhash.h>
#include <qjsonobject.h>
#include <qobject.h>
#include <qprocess.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qtmetamacros.h>

namespace myqmlplugin {
class CmdEntry : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("Cannot create.")

  Q_PROPERTY(QString prefix READ prefix NOTIFY prefixChanged)
  Q_PROPERTY(QString separator READ separator NOTIFY separatorChanged)
  Q_PROPERTY(QString path READ path NOTIFY pathChanged)
  Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
  Q_PROPERTY(QString label READ label NOTIFY labelChanged)
  Q_PROPERTY(QString icon READ icon NOTIFY iconChanged)
  Q_PROPERTY(int arguments READ arguments NOTIFY argumentsChanged)
  Q_PROPERTY(bool captureOutput READ captureOutput)
  Q_PROPERTY(bool isCoreCommand READ isCoreCommand NOTIFY isCoreCommandChanged)

public:
  explicit CmdEntry(QString basePath, QJsonObject jsonData,
                    QObject *parent = nullptr);

  [[nodiscard]] QString prefix() const;
  [[nodiscard]] QString separator() const;
  [[nodiscard]] QString path() const;
  [[nodiscard]] QString description() const;
  [[nodiscard]] QString label() const;
  [[nodiscard]] QString icon() const;
  [[nodiscard]] int arguments() const;
  [[nodiscard]] bool captureOutput() const;
  [[nodiscard]] bool isCoreCommand() const;
  void setIsCoreCommand(bool coreCommand);

signals:
  void prefixChanged();
  void separatorChanged();
  void pathChanged();
  void descriptionChanged();
  void labelChanged();
  void iconChanged();
  void argumentsChanged();
  void isCoreCommandChanged();

private:
  QJsonObject m_data;
  QString m_path;
  bool m_isCoreCommand = false;
};

class CmdHandler : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged REQUIRED)
  Q_PROPERTY(QString basePath READ basePath WRITE setBasePath NOTIFY
                 basePathChanged REQUIRED)
  Q_PROPERTY(QQmlListProperty<myqmlplugin::CmdEntry> entries READ entries NOTIFY
                 entriesChanged)
  Q_PROPERTY(QString queryString READ queryString WRITE setQueryString NOTIFY
                 queryStringChanged)
  Q_PROPERTY(bool isProcessRunning READ isProcessRunning NOTIFY
                 isProcessRunningChanged)
  Q_PROPERTY(QList<QString> processOutput READ processOutput NOTIFY
                 processOutputChanged)

public:
  explicit CmdHandler(QObject *parent = nullptr);

  enum EntryUpdateFlags { None = 0, SkipPath = 1 << 0, SkipBasePath = 1 << 1 };

  [[nodiscard]] QString path() const;
  void setPath(const QString &newPath);

  [[nodiscard]] QString basePath() const;
  void setBasePath(const QString &newPath);

  [[nodiscard]] QQmlListProperty<CmdEntry> entries();

  [[nodiscard]] QString queryString() const;
  void setQueryString(const QString &newString);

  [[nodiscard]] QList<QString> processOutput() const;

  [[nodiscard]] bool isProcessRunning();

  Q_INVOKABLE void refreshCommandList();

  Q_INVOKABLE QVariantMap executeCommand(const QString &command);

signals:
  void pathChanged();
  void basePathChanged();
  void entriesChanged();
  void queryStringChanged();
  void isProcessRunningChanged();
  void processOutputChanged();

private:
  QString m_path;
  QString m_basePath;
  QString m_queryString = "";
  QProcess *m_runningProcess = nullptr;
  QList<QString> m_processOutput;
  QHash<QString, CmdEntry *> m_cmdEntries;
  mutable QList<CmdEntry *> m_sortedCommands;
  mutable QList<CmdEntry *> m_filteredCommands;

  bool m_shouldReSort = true;

  template <typename Func>
  QSet<QString> pathIterate(const QString &path, bool *isDirty, Func callback);

  QSet<QString> pathIterate(const QString &path, bool *isDirty = nullptr);

  void updateEntries(int flag = 0);
  QList<CmdEntry *> &getFilteredCommands();
  QList<CmdEntry *> &getSortedCommands();
};

} // namespace myqmlplugin
