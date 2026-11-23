#pragma once

#include <QObject>

#include "qwayland-xdg-activation-v1.h"

namespace ns::wayland::xdg {
class XdgActivationToken : public QObject,
                           public QtWayland::xdg_activation_token_v1 {
  Q_OBJECT

public:
  ~XdgActivationToken() override;

protected:
  void xdg_activation_token_v1_done(const QString &token) override;

signals:
  void done(const QString &token);
};
} // namespace ns::wayland::xdg
