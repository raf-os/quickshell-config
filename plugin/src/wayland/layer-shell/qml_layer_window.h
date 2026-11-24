#pragma once

#include <QtQml>

#include "layer_window.h"

QML_DECLARE_TYPEINFO(ns::layershell::LayerWindow, QML_HAS_ATTACHED_PROPERTIES)

class QmlLayerWindow {
  Q_GADGET
  QML_NAMED_ELEMENT(LayerWindow)
  QML_FOREIGN(ns::layershell::LayerWindow)
  QML_UNCREATABLE("")
  QML_ATTACHED(ns::layershell::LayerWindow)
};

namespace QmlLayerAnchor {
Q_NAMESPACE
QML_NAMED_ELEMENT(LayerAnchor)
QML_FOREIGN_NAMESPACE(ns::layershell::LayerAnchor)
}; // namespace QmlLayerAnchor

namespace QmlWlrLayer {
Q_NAMESPACE
QML_NAMED_ELEMENT(WlrLayer)
QML_FOREIGN_NAMESPACE(ns::layershell::WlrLayer)
} // namespace QmlWlrLayer

namespace QmlKeyboardInteractivity {
Q_NAMESPACE
QML_NAMED_ELEMENT(KeyboardInteractivity)
QML_FOREIGN_NAMESPACE(ns::layershell::KeyboardInteractivity)
}; // namespace QmlKeyboardInteractivity
