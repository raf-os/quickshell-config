#pragma once

#include <qlist.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qtmetamacros.h>

namespace myqmlplugin {
class HyprKeyboardLayout : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString layout READ layout)
  Q_PROPERTY(QString variant READ variant)

public:
  explicit HyprKeyboardLayout(QObject *parent = nullptr);

  [[nodiscard]] QString layout() const;
  [[nodiscard]] QString variant() const;

  bool isValid();

private:
  QString m_layout;
  QString m_variant;
};

class HyprInputConfig : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString kbModel READ kbModel)
  Q_PROPERTY(QString kbOptions READ kbOptions)
  Q_PROPERTY(QString kbRules READ kbRules)
  Q_PROPERTY(QMLListProperty<HyprKeyboardLayout> layouts READ layouts)

public:
  explicit HyprInputConfig(QObject *parent = nullptr);

  [[nodiscard]] QString kbModel() const;
  [[nodiscard]] QString kbOptions() const;
  [[nodiscard]] QString kbRules() const;
  [[nodiscard]] QQmlListProperty<HyprKeyboardLayout> layouts();

private:
  QString m_kbModel;
  QString m_kbOptions;
  QString m_kbRules;
  QList<HyprKeyboardLayout *> m_layouts;
};

class HyprExtras : public QObject {
  Q_OBJECT
  QML_SINGLETON
  QML_ELEMENT

  Q_PROPERTY(QString configPath READ configPath WRITE setConfigPath NOTIFY
                 configPathChanged)

public:
  explicit HyprExtras(QObject *parent = nullptr);

  [[nodiscard]] QString configpath() const;
  void setConfigPath(const QString &path);

  void parseInputConfig();
  Q_INVOKABLE void writeConfigsToFile();

signals:
  void configPathChanged();

private:
  QString m_configPath;
  HyprInputConfig *m_inputConfig = nullptr;
};
} // namespace myqmlplugin
