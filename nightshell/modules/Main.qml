pragma ComponentBehavior: Bound

import org.nightshell.Utils
import org.nightshell.Hyprland as N_HYPRLAND
import org.nightshell.Wayland as N_WAYLAND

import qs.components
import qs.shapes as Shapes
import qs.modules.bar
import qs.utils

import Quickshell
import Quickshell.Wayland
import QtQuick

Variants {
  id: root

  model: Quickshell.screens

  Scope {
    id: scope
    required property ShellScreen modelData

    Exclusions {
      screen: scope.modelData
      bar: barComp
    }

    BasePanelWindow {
      id: winComp

      name: "main-shell"
      screen: scope.modelData

      // WlrLayershell.keyboardFocus: FocusGrabberManager.active ? WlrKeyboardFocus.Exclusive : WlrKeyboardFocus.OnDemand

      exclusionMode: ExclusionMode.Ignore
      mask: MaskRegions {
        id: windowMask

        bar: barComp
        win: winComp
        content: contentComp
      }

      anchors {
        top: true
        right: true
        bottom: true
        left: true
      }

      N_WAYLAND.EagerBufferInitializer {}

      InstanceContext {
        id: contextComp

        hyprMonitor: N_HYPRLAND.Hyprland.monitorsModel.values.find(m => m.name === scope.modelData.name)
        win: winComp
        shellScreen: scope.modelData
        bar: barComp
        content: contentComp // qmllint disable incompatible-type
      }

      RootInteractionHandler {
        anchors.fill: parent

        Bar {
          id: barComp
          panelWindow: winComp
          context: contextComp

          anchors {
            top: parent.top
            left: parent.left
            right: parent.right
          }
        }

        Content {
          id: contentComp
          context: contextComp

          anchors {
            top: bar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
          }
        }
      }
    }
  }
}
