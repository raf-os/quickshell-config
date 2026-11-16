import org.nightshell.Mpris

import QtQuick

Item {
  id: root

  readonly property bool isActive: Mpris.model.values.length > 0

  Loader {
    active: root.isActive
    sourceComponent: Item {
      id: lditem
      readonly property MprisPlayer player: Mpris.model.values[0]

      Text {
        text: lditem.player.identity
        color: "white"
      }
    }
  }
}
