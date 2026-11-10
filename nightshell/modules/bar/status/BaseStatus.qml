pragma ComponentBehavior: Bound

import qs.modules.bar

import org.nightshell.Components

import QtQuick

BaseWidget {
  id: root

  property string tooltipText: ""
  property bool isActive: false
  required property string name
  property Component popoutComponent: null

  signal clicked(ev: MouseEvent)

  anchors {
    top: parent.top
    bottom: parent.bottom
  }

  implicitWidth: height

  MouseArea {
    id: mouseArea
    anchors.fill: parent

    onClicked: ev => root.clicked(ev)
    enabled: root.enabled
    hoverEnabled: true
    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

    SToolTip {
      active: !root.isActive && root.tooltipText !== "" && mouseArea.containsMouse
      text: root.tooltipText
      delay: 500
      offset: 2
    }
  }
}
