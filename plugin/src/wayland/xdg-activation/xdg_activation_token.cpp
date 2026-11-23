#include "xdg_activation_token.h"

namespace ns::wayland::xdg {
XdgActivationToken::~XdgActivationToken() { destroy(); }

void XdgActivationToken::xdg_activation_token_v1_done(const QString &token) {
  emit done(token);
}
} // namespace ns::wayland::xdg
