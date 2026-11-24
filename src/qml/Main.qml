import QtQuick
import org.nightshell.Wayland

Window {
  id: root

  color: "red"
  visible: true

  width: 320
  height: 240

  LayerWindow.layer: WlrLayer.Top
  LayerWindow.anchors: LayerAnchor.Top
}
