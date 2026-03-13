#pragma once

#include <qhash.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qtmetamacros.h>

namespace myqmlplugin {
class CmdEntry : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("Cannot create.")

  Q_PROPERTY(QString suffix READ suffix NOTIFY suffixChanged)
  Q_PROPERTY(QString separator READ separator NOTIFY separatorChanged)
  Q_PROPERTY(QString relativePath READ relativePath NOTIFY relativePathChanged)
  Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)

public:
  explicit CmdEntry(QString basePath, QObject *parent = nullptr);

  [[nodiscard]] QString suffix() const;
  [[nodiscard]] QString separator() const;
  [[nodiscard]] QString relativePath() const;

signals:
  void suffixChanged();
  void separatorChanged();
  void relativePathChanged();
  void descriptionChanged();
};

class CmdHandler : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged REQUIRED)
  Q_PROPERTY(QString basePath READ basePath WRITE setBasePath NOTIFY
                 basePathChanged REQUIRED)
  Q_PROPERTY(QQmlListProperty<myqmlplugin::CmdEntry> entries READ entries NOTIFY
                 entriesChanged)

public:
  explicit CmdHandler(QObject *parent = nullptr);

  [[nodiscard]] QString path() const;
  void setPath(const QString &newPath);

  [[nodiscard]] QString basePath() const;
  void setBasePath(const QString &newPath);

  [[nodiscard]] QQmlListProperty<CmdEntry> entries();

  Q_INVOKABLE void refreshCommandList();

  Q_INVOKABLE QString executeCommand(const QString &command);

signals:
  void pathChanged();
  void entriesChanged();

private:
  QString m_path;
  QString m_basePath;
  QHash<QString, CmdEntry *> m_cmdEntries;

  void iterateAndUpdateFromPaths();
  QSet<QString> pathIterate(const QString &path, bool *isDirty = nullptr);
  void updateEntries();
};

} // namespace myqmlplugin
