#pragma once

#include <qjsengine.h>
#include <qobject.h>
#include <qprocess.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include "hyprevents.h"

namespace ns::hyprland {
class Hyprland : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  Q_PROPERTY(myqmlplugin::HyprEvents *eventHandler READ eventHandler CONSTANT)
  Q_PROPERTY(int keyboardLayoutIndex READ keyboardLayoutIndex NOTIFY
                 keyboardLayoutIndexChanged)

public:
  static Hyprland *instance() {
    static Hyprland *s_instance = new Hyprland();
    return s_instance;
  }

  static Hyprland *create(QQmlEngine *qmlEngine,
                          QJSEngine /*unused*/) {
    auto inst = instance();
    if (qmlEngine)
      qmlEngine->setObjectOwnership(inst, QQmlEngine::CppOwnership);
    return inst;
  }

  [[nodiscard]] myqmlplugin::HyprEvents *eventHandler();
  [[nodiscard]] int                      keyboardLayoutIndex() const;
  void setKeyboardLayoutIndex(const int &value);

private slots:
  void queryHyprInputConfigs();
  void onInputQueryReadyToRead();

signals:
  void keyboardLayoutIndexChanged();

private:
  explicit Hyprland(QObject *parent = nullptr);

  myqmlplugin::HyprEvents *m_eventHandler;
  int                      m_keyboardLayoutIndex;

  bool      m_hyprInputQueryQueued = false;
  QProcess *m_hyprInputQueryProcess;
};
} // namespace ns::hyprland
