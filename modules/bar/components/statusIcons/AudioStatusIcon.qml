import qs.config
import qs.modules.bar
import qs.components
import qs.utils
import qs.services
import QtQuick

Item {
    id: root

    required property PopoutHandler popoutHandler

    property real roundedVolume: Math.round((AudioService.volume * 10000)) / 100

    // implicitHeight: Config.bar.sizes.innerHeight
    implicitWidth: Config.appearance.fontSize.xl

    anchors.top: parent.top
    anchors.bottom: parent.bottom

    StyledText {
        id: iconTxt
        anchors.fill: parent
        text: `${Icons.getVolumeIcon(AudioService.volume, AudioService.isMuted)}`
        font.pixelSize: Config.appearance.fontSize.xl
        font.family: Config.appearance.fontFamily.monoIcon

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    MouseArea {
        id: interactionArea

        cursorShape: Qt.PointingHandCursor
        anchors.fill: parent

        onClicked: {
            root.popoutHandler.triggerPopout(root, "audio");
        }
    }
}
