#pragma once

#include "hyprevents.h"

#include <canberra.h>
#include <cstdint>
#include <qlist.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qstringview.h>
#include <qtmetamacros.h>

namespace ns {
class SystemSounds : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(myqmlplugin::HyprEvents *eventHandler READ eventHandler WRITE
                 setEventHandler NOTIFY eventHandlerChanged)

public:
  explicit SystemSounds(QObject *parent = nullptr);
  ~SystemSounds();

  enum ContextId : uint32_t {
    BellSound = 0,
  };

  struct CallbackProps {
    SystemSounds *ref = nullptr;
  };

  [[nodiscard]] myqmlplugin::HyprEvents *eventHandler() const;
  void setEventHandler(myqmlplugin::HyprEvents *value);

  Q_SLOT bool onBellRequested();

  static void onSoundFinished(ca_context *c, uint32_t id, int error_code,
                              void *userData);

signals:
  void eventHandlerChanged();

private:
  myqmlplugin::HyprEvents *m_eventHandler = nullptr;
  ca_context *m_ctx = nullptr;

  struct {
    bool BellSound = false;
  } ActiveSounds;
};
} // namespace ns
