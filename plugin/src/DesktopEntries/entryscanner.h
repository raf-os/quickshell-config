#pragma once

#include <qcontainerfwd.h>
#include <qdir.h>
#include <qlist.h>
#include <qobject.h>
#include <qrunnable.h>
#include <qtmetamacros.h>

namespace ns::desktop::entries {
struct EntryActionData {
  QString                 id;
  QString                 name;
  QString                 icon;
  QString                 execString;
  QStringList             command;
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

class DesktopEntryScanner : public QObject, public QRunnable {
  Q_OBJECT

public:
  explicit DesktopEntryScanner(QObject *parent = nullptr);

  void run() override;
  void scanDirectory(const QString    &path,
                     const QString    &idPrefix,
                     QList<EntryData> &entries);

signals:
  void scanCompleted(const QList<EntryData> &results);
};
} // namespace ns::desktop::entries
