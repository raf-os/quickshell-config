#pragma once

#include <optional>
#include <qcontainerfwd.h>
#include <qdir.h>
#include <qhash.h>
#include <qjsengine.h>
#include <qlist.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qrunnable.h>
#include <qtmetamacros.h>

namespace ns::desktopentries {
struct EntryActionData {
  QString                 id;
  QString                 name;
  QString                 icon;
  QString                 execString;
  QList<QString>          command;
  QHash<QString, QString> entries;
};

struct EntryData {
  QString                 id;
  QString                 name;
  QString                 genericName;
  QString                 startupClass;
  QString                 icon;
  QString                 comment;
  QString                 execStr;
  QString                 workingDirectory;
  QList<QString>          command;
  QList<QString>          categories;
  QList<QString>          keywords;
  QList<EntryActionData>  actions;
  QHash<QString, QString> entries;
  bool                    noDisplay = false;
  bool                    hidden    = false;
  bool                    terminal  = false;
};

class DesktopEntryManager;

class DesktopEntryScanner : public QRunnable {
public:
  explicit DesktopEntryScanner(DesktopEntryManager *manager);

  void run() override;
  void scanDirectory(const QString    &path,
                     const QString    &idPrefix,
                     QList<EntryData> &entries);

private:
  DesktopEntryManager *m_manager;
};

class DesktopEntryManager : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  static DesktopEntryManager *instance() {
    static DesktopEntryManager *s_instance = new DesktopEntryManager();
    return s_instance;
  }

  static DesktopEntryManager *create(QQmlEngine *qmlEngine,
                                     QJSEngine * /* unused */) {
    auto inst = instance();
    if (qmlEngine)
      qmlEngine->setObjectOwnership(inst, QQmlEngine::CppOwnership);
    return inst;
  }

  static const QStringList       &desktopPaths();
  static std::optional<EntryData> parseText(const QString &id,
                                            const QString &filePath);
  static QStringList              parseExecString(const QString &execString);

private slots:
  void onScanCompleted(const QList<EntryData> &results);

private:
  explicit DesktopEntryManager(QObject *parent = nullptr);

  bool m_scanInProgress = false;
  bool m_scanQueued     = false;
};
} // namespace ns::desktopentries
