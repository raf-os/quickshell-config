#pragma once

#include "hyprevents.h"
#include "kbd.h"
#include <optional>
#include <qcontainerfwd.h>
#include <qglobalstatic.h>
#include <qlist.h>
#include <qlocalsocket.h>
#include <qobject.h>
#include <qprocess.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qstringview.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <utility>

namespace myqmlplugin {
class HyprKeyboardLayout : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("No reason")

  Q_PROPERTY(QString layout READ layout NOTIFY layoutChanged)
  Q_PROPERTY(QString variant READ variant NOTIFY variantChanged)
  Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY
                 descriptionChanged)

public:
  explicit HyprKeyboardLayout(const QString &layout, const QString &variant,
                              QObject *parent = nullptr);
  explicit HyprKeyboardLayout(const QString &layout, const QString &variant,
                              const QString &description,
                              QObject *parent = nullptr);

  [[nodiscard]] QString layout() const;
  [[nodiscard]] QString variant() const;
  [[nodiscard]] QString description() const;
  void setDescription(const QString &desc);

  bool isValid();

signals:
  void layoutChanged();
  void variantChanged();
  void descriptionChanged();

private:
  QString m_layout;
  QString m_variant;
  QString m_description;
};

class HyprInputConfig : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("No reason")

  Q_PROPERTY(QString kbModel READ kbModel NOTIFY kbModelChanged)
  Q_PROPERTY(QString kbOptions READ kbOptions NOTIFY kbOptionsChanged)
  Q_PROPERTY(QString kbRules READ kbRules NOTIFY kbRulesChanged)
  Q_PROPERTY(QQmlListProperty<myqmlplugin::HyprKeyboardLayout> layouts READ
                 layouts NOTIFY layoutsChanged)

public:
  explicit HyprInputConfig(QObject *parent = nullptr);

  [[nodiscard]] QString kbModel() const;
  void setKbModel(const QString &model);

  [[nodiscard]] QString kbOptions() const;
  void setKbOptions(const QString &opts);

  [[nodiscard]] QString kbRules() const;
  void setKbRules(const QString &rules);

  [[nodiscard]] QQmlListProperty<HyprKeyboardLayout> layouts();
  void setLayouts(const QStringList &layouts, const QStringList &variants);
  void setLayouts(const QList<std::pair<QString, QString>> &layouts);

  [[nodiscard]] QList<HyprKeyboardLayout *> layoutList() const;

  [[nodiscard]] QByteArray *tryFetchWriteBuffer();

  void attachKeyboardHandler(KeyboardLayoutHandler *obj);
  bool compileCommandFileString();

signals:
  void kbModelChanged();
  void kbOptionsChanged();
  void kbRulesChanged();
  void layoutsChanged();

  void fileBufferReadyToWrite();

private:
  QString m_kbModel;
  QString m_kbOptions;
  QString m_kbRules;
  bool m_bufferReadyFlag = true;
  QByteArray m_confWriteBuffer;
  mutable QList<HyprKeyboardLayout *> m_layouts;
  KeyboardLayoutHandler *m_kbLayoutHandler = nullptr;
};

class HyprExtras : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(bool isSaving READ isSaving NOTIFY isSavingChanged)
  Q_PROPERTY(
      int kbdLayoutIndex READ kbdLayoutIndex NOTIFY kbdLayoutIndexChanged)
  Q_PROPERTY(QString configPath READ configPath WRITE setConfigPath NOTIFY
                 configPathChanged REQUIRED)
  Q_PROPERTY(QString shellConfigPath READ shellConfigPath WRITE
                 setShellConfigPath NOTIFY shellConfigPathChanged)
  Q_PROPERTY(QString cachePath READ cachePath WRITE setCachePath NOTIFY
                 cachePathChanged)
  Q_PROPERTY(myqmlplugin::KeyboardLayoutHandler *keyboardLayoutHandler READ
                 keyboardLayoutHandler WRITE setKeyboardLayoutHandler NOTIFY
                     keyboardLayoutHandlerChanged REQUIRED)
  Q_PROPERTY(
      myqmlplugin::HyprInputConfig *inputConfig READ inputConfig CONSTANT)
  Q_PROPERTY(myqmlplugin::HyprEvents *eventListener READ eventListener CONSTANT)

public:
  explicit HyprExtras(QObject *parent = nullptr);
  ~HyprExtras() override;

  [[nodiscard]] bool isSaving() const;

  [[nodiscard]] QString configPath() const;
  void setConfigPath(const QString &path);

  [[nodiscard]] QString shellConfigPath() const;
  void setShellConfigPath(const QString &path);

  [[nodiscard]] QString cachePath() const;
  void setCachePath(const QString &path);

  [[nodiscard]] int kbdLayoutIndex() const;

  [[nodiscard]] myqmlplugin::KeyboardLayoutHandler *
  keyboardLayoutHandler() const;
  void setKeyboardLayoutHandler(KeyboardLayoutHandler *kbh);

  [[nodiscard]] myqmlplugin::HyprInputConfig *inputConfig() const;

  [[nodiscard]] myqmlplugin::HyprEvents *eventListener() const;

  void hyprlangParse();
  void parseInputConfig();
  void queryCurrentDevices();

  HyprKeyboardLayout *getLayout(const QString &layout, const QString &variant,
                                QObject *parent);

  Q_INVOKABLE void updateCurrentKeyboardConfig();
  Q_INVOKABLE void writeInputConfigToFile();
  Q_INVOKABLE void initConfigParse();
  Q_INVOKABLE void hyprctl(const QStringList &commands);

  void
  changeSettings(std::optional<QList<std::pair<QString, QString>>> newLayouts,
                 std::optional<int> newIndex);

signals:
  void isSavingChanged();
  void configPathChanged();
  void shellConfigPathChanged();
  void cachePathChanged();
  void keyboardLayoutHandlerChanged();
  void kbdLayoutIndexChanged();

  void inputConfigSaved();

private:
  HyprEvents *m_hyprEvents = nullptr;
  QProcess *m_hyprctlProcess = nullptr;
  bool m_isSavingFlag;
  bool m_useLuaConfig = true;
  QTimer *m_lookupCooldownTimer = nullptr;
  QTimer *m_hyprInputQueryDebouncer = nullptr;
  QProcess *m_inputQueryProcess = nullptr;
  QProcess *m_hyprInputQueryProcess = nullptr;
  QByteArray m_ipProcessBuffer;
  int m_kbLayoutIndex = 0;
  QString m_configPath;
  QString m_shellConfigPath;
  QString m_cachePath;
  HyprInputConfig *m_inputConfig = nullptr;
  KeyboardLayoutHandler *m_kbLayoutHandler = nullptr;

  void parseProcessData();
  void saveInputConfig();
  void setIsSaving(bool val);
  void saveDataToCache();
  void queryHyprInputConfigs();
  void parseHyprInputConfigs(QByteArray &buf);
};
} // namespace myqmlplugin
