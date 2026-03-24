import qs.config
import qs.components
import qs.utils
import qs.services
import QtQuick
import QtQuick.Layouts

Item {
    id: root
    property real roundedVolume: Math.round((AudioService.volume * 10000)) / 100

    implicitHeight: Config.bar.sizes.innerHeight
    implicitWidth: Config.appearance.fontSize.xl

    StyledText {
        id: iconTxt
        anchors.centerIn: parent
        text: `${Icons.getVolumeIcon(AudioService.volume, AudioService.isMuted)}`
        font.pixelSize: Config.appearance.fontSize.xl
        font.family: Config.appearance.fontFamily.monoIcon
    }
}
