#pragma once

#include <optional>
#include <qdir.h>
#include <qhash.h>
#include <qlist.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qtmetamacros.h>

using StringHash = QHash<QString, QString>;

namespace ns {
struct SThemeData {
  QString name;
  QString xdgName;
  QString path;
};

class SoundThemeData : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QString themeName READ themeName CONSTANT)
  Q_PROPERTY(QString xdgName READ xdgName CONSTANT)
  Q_PROPERTY(QString themePath READ themePath CONSTANT)

public:
  explicit SoundThemeData(const SThemeData &data, QObject *parent = nullptr)
      : QObject(parent), m_themeName(data.name), m_xdgName(data.xdgName),
        m_themePath(data.path) {}

  [[nodiscard]] QString themeName() const { return m_themeName; }
  [[nodiscard]] QString xdgName() const { return m_xdgName; }
  [[nodiscard]] QString themePath() const { return m_themePath; }

private:
  const QString m_themeName;
  const QString m_xdgName;
  const QString m_themePath;
};

class SoundThemes : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QQmlListProperty<ns::SoundThemeData> themes READ themes NOTIFY
                 themesChanged)

public:
  explicit SoundThemes(QObject *parent = nullptr);

  [[nodiscard]] QQmlListProperty<SoundThemeData> themes();

  Q_INVOKABLE void buildThemeDb();

signals:
  void themesChanged();

private:
  QList<SoundThemeData *> m_themes;

  std::optional<QHash<QString, StringHash>> parseThemeFile(QFile &file);
};
} // namespace ns
