#pragma once

#include <qjsengine.h>
#include <qobject.h>
#include <qprocess.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qtimer.h>
#include <qtmetamacros.h>

#include "hyprevents.h"
#include "hyprinputconfig.h"
#include "toplevelmodel.h"

namespace ns::hyprland {
class Hyprland : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  Q_PROPERTY(ns::hyprland::HyprEvents *eventHandler READ eventHandler CONSTANT)
  Q_PROPERTY(
      ns::hyprland::ToplevelModel *toplevelModel READ toplevelModel CONSTANT)
  Q_PROPERTY(int keyboardLayoutIndex READ keyboardLayoutIndex NOTIFY
                 keyboardLayoutIndexChanged)

public:
  static Hyprland *instance() {
    static Hyprland *s_instance = new Hyprland();
    return s_instance;
  }

  static Hyprland *create(QQmlEngine *qmlEngine,
                          QJSEngine  *jsEngine) {
    auto inst = instance();
    if (qmlEngine)
      qmlEngine->setObjectOwnership(inst, QQmlEngine::CppOwnership);
    return inst;
  }

  [[nodiscard]] HyprEvents    *eventHandler();
  [[nodiscard]] ToplevelModel *toplevelModel();
  [[nodiscard]] int            keyboardLayoutIndex() const;
  void                         setKeyboardLayoutIndex(const int &value);

private slots:
  void queryHyprInputConfigs();
  void onInputQueryReadyToRead();
  void queryActiveDevices();

signals:
  void keyboardLayoutIndexChanged();

private:
  explicit Hyprland(QObject *parent = nullptr);

  HyprEvents    *m_eventHandler  = nullptr;
  ToplevelModel *m_toplevelModel = nullptr;
  int            m_keyboardLayoutIndex;

  bool      m_hyprInputQueryQueued = false;
  QProcess *m_hyprInputQueryProcess;

  bool      m_deviceQueryQueued = false;
  QTimer    m_deviceQueryCooldown;
  QProcess *m_deviceQueryProcess = nullptr;

  HyprInputConfig *m_inputConfig;
};
} // namespace ns::hyprland
