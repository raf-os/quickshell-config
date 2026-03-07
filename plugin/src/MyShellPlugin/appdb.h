#pragma once

#include <qcontainerfwd.h>
#include <qhash.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qregularexpression.h>
#include <qtmetamacros.h>
#include <qtypes.h>

// Credits to the Caelestia shell
// https://github.com/caelestia-dots/shell/tree/main

namespace myqmlplugin {
class AppEntry : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("No no no")

  Q_PROPERTY(QObject *entry READ entry CONSTANT)

  Q_PROPERTY(quint32 frequency READ frequency NOTIFY frequencyChanged)
  Q_PROPERTY(QString id READ id CONSTANT)
  Q_PROPERTY(QString name READ name NOTIFY nameChanged)
  Q_PROPERTY(QString comment READ comment NOTIFY commentChanged)
  Q_PROPERTY(QString execString READ execString NOTIFY execStringChanged)
  Q_PROPERTY(QString categories READ categories NOTIFY categoriesChanged)

public:
  explicit AppEntry(QObject *entry, quint32 frequency,
                    QObject *parent = nullptr);

  [[nodiscard]] QObject *entry() const;

  [[nodiscard]] quint32 frequency() const;
  void setFrequency(quint32 frequenct);
  void incrementFrequency();

  [[nodiscard]] QString id() const;
  [[nodiscard]] QString name() const;
  [[nodiscard]] QString comment() const;
  [[nodiscard]] QString execString() const;
  [[nodiscard]] QString categories() const;

signals:
  void frequencyChanged();
  void nameChanged();
  void commentChanged();
  void execStringChanged();
  void categoriesChanged();

private:
  QObject *m_entry;
  quint32 m_frequency;
};

class AppDb : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString uuid READ uuid CONSTANT)
  Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged REQUIRED)
  Q_PROPERTY(QObjectList entries READ entries WRITE setEntries NOTIFY
                 entriesChanged REQUIRED)
  Q_PROPERTY(QStringList favoriteApps READ favoriteApps WRITE setFavoriteApps
                 NOTIFY favoriteAppsChanged REQUIRED)
  Q_PROPERTY(
      QQmlListProperty<myqmlplugin::AppEntry> apps READ apps NOTIFY appsChanged)

public:
  explicit AppDb(QObject *parent = nullptr);

  [[nodiscard]] QString uuid() const;

  [[nodiscard]] QString path() const;
  void setPath(const QString &path);

  [[nodiscard]] QObjectList entries() const;
  void setEntries(const QObjectList &entries);

  [[nodiscard]] QStringList favoriteApps() const;
  void setFavoriteApps(const QStringList &favoriteApps);

  [[nodiscard]] QQmlListProperty<AppEntry> apps();

  Q_INVOKABLE void incrementFrequency(const QString &id);

signals:
  void pathChanged();
  void entriesChanged();
  void favoriteAppsChanged();
  void appsChanged();

private:
  const QString m_uuid;
  QString m_path;
  QObjectList m_entries;
  QStringList m_favoriteApps;
  QList<QRegularExpression> m_favoriteAppsRegex;
  QHash<QString, AppEntry *> m_apps;
  mutable QList<AppEntry *> m_sortedApps;

  QString regexifyString(const QString &original) const;
  void updateApps();
  bool isFavorite(const AppEntry *app) const;
  QList<AppEntry *> &getSortedApps() const;
  quint32 getFrequency(const QString &id) const;
  void updateAppFrequencies();
};
} // namespace myqmlplugin
