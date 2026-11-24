pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Window

import org.nightshell.Wayland
import org.nightshell

ScreenObjectRepeater {
  id: root
  screens: ShellGlobal.screens

  Window {
    id: bwin

    required property var modelData
    screen: modelData

    color: "white"
    visible: true

    width: 320
    height: 240

    LayerWindow.layer: WlrLayer.Top
    LayerWindow.anchors: LayerAnchor.Top
    LayerWindow.screen: modelData
  }
}
