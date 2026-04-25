import qs.services
import qs.config
import qs.utils
import qs.components
import Quickshell
import QtQuick
import QtQuick.Layouts

Item {
    id: root
    // Layout.fillWidth: true
    Layout.fillHeight: true

    clip: true

    Rectangle {
        id: rectBg
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        implicitHeight: Config.bar.sizes.innerHeight
        // implicitWidth: parent.width
        color: ColorService.current.base0
        radius: Config.appearance.rounding.sm
    }

    StyledText {
        anchors.centerIn: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.family: textMetrics.font.family
        font.pointSize: textMetrics.font.pointSize
        text: textMetrics.elidedText
    }

    TextMetrics {
        id: textMetrics
        font.family: Config.appearance.fontFamily.mono
        font.pointSize: Config.appearance.fontSize.sm
        elide: Qt.ElideRight
        elideWidth: rectBg.width - Config.appearance.padding.md * 2
        text: Hypr.activeTopLevel?.title ?? qsTr("Desktop")
    }
}
