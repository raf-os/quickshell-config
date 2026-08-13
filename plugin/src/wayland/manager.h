#pragma once

#include <qjsengine.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qquickwindow.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>

#include "wlbuffer.h"
#include "wlbufferrequest.h"

namespace ns::wayland::buffer {
class WlBufferManagerPrivate;

class WlBufferManager : public QObject {
  Q_OBJECT

public:
  Q_DISABLE_COPY_MOVE(WlBufferManager)

  ~WlBufferManager() override;

  static WlBufferManager *instance();

  void                    initWindow(QQuickWindow *window);
  [[nodiscard]] bool      isReady() const;
  [[nodiscard]] WlBuffer *createBuffer(const WlBufferRequest &request);

signals:
  void ready();

private:
  explicit WlBufferManager();

  WlBufferManagerPrivate *p;
};
} // namespace ns::wayland::buffer
