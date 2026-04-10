#pragma once

#include "kbd.h"
#include <hyprlang.hpp>
#include <qcontainerfwd.h>
#include <qglobalstatic.h>
#include <qlist.h>
#include <qobject.h>
#include <qprocess.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qtimer.h>
#include <qtmetamacros.h>

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
  Q_PROPERTY(QQmlListProperty<HyprKeyboardLayout> layouts READ layouts NOTIFY
                 layoutsChanged)

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
  Q_PROPERTY(KeyboardLayoutHandler *keyboardLayoutHandler READ
                 keyboardLayoutHandler WRITE setKeyboardLayoutHandler NOTIFY
                     keyboardLayoutHandlerChanged REQUIRED)
  Q_PROPERTY(
      HyprInputConfig *inputConfig READ inputConfig NOTIFY inputConfigChanged)

public:
  explicit HyprExtras(QObject *parent = nullptr);
  ~HyprExtras() override;

  [[nodiscard]] bool isSaving() const;

  [[nodiscard]] QString configPath() const;
  void setConfigPath(const QString &path);

  [[nodiscard]] QString shellConfigPath() const;
  void setShellConfigPath(const QString &path);

  [[nodiscard]] int kbdLayoutIndex() const;

  [[nodiscard]] KeyboardLayoutHandler *keyboardLayoutHandler() const;
  void setKeyboardLayoutHandler(KeyboardLayoutHandler *kbh);

  [[nodiscard]] HyprInputConfig *inputConfig() const;

  void hyprlangParse();
  void parseInputConfig();
  void queryCurrentDevices();

  static Hyprlang::CConfig *s_hyprlangConfig;
  static Hyprlang::CParseResult hyprlangHandleSource(const char *COMMAND,
                                                     const char *VALUE);

  Q_INVOKABLE void updateCurrentKeyboardConfig();
  Q_INVOKABLE void writeInputConfigToFile();
  Q_INVOKABLE void initConfigParse();

signals:
  void isSavingChanged();
  void configPathChanged();
  void shellConfigPathChanged();
  void keyboardLayoutHandlerChanged();
  void kbdLayoutIndexChanged();
  void inputConfigChanged();

  void inputConfigSaved();

private:
  bool m_isSavingFlag;
  QTimer *m_lookupCooldownTimer = nullptr;
  QProcess *m_inputQueryProcess = nullptr;
  QByteArray m_ipProcessBuffer;
  int m_kbLayoutIndex = 0;
  QString m_configPath;
  QString m_shellConfigPath;
  HyprInputConfig *m_inputConfig;
  KeyboardLayoutHandler *m_kbLayoutHandler = nullptr;

  void parseProcessData();
  void saveInputConfig();
  void setIsSaving(bool val);
};

// Juuuuust to be sure
struct HandlerGuard {
  ~HandlerGuard() {
    if (HyprExtras::s_hyprlangConfig != nullptr) {
      HyprExtras::s_hyprlangConfig->unregisterHandler("scope");
      HyprExtras::s_hyprlangConfig = nullptr;
    }
  }
};
} // namespace myqmlplugin
