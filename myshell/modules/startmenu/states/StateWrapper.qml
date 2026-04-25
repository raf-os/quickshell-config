import qs.config
import Quickshell
import QtQuick
import QtQuick.Layouts

Loader {
    id: root

    required property string myState
    required property string currentState

    property real padding: Config.appearance.padding.lg

    active: myState === currentState
    visible: active

    anchors.fill: parent
    anchors.margins: padding
}
