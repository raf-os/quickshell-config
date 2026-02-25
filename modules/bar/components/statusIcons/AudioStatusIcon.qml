import qs.config
import qs.components
import qs.utils
import qs.services
import QtQuick
import QtQuick.Layouts

RowLayout {
    id: root
    property real roundedVolume: Math.round((AudioService.volume * 10000)) / 100

    spacing: Config.appearance.spacing.sm

    StyledText {
        text: `${root.roundedVolume}%`
        font.pointSize: Config.appearance.fontSize.sm
        font.family: Config.appearance.fontFamily.mono
    }

    StyledText {
        text: `${Icons.getVolumeIcon(AudioService.volume, AudioService.isMuted)}`
        font.pointSize: Config.appearance.fontSize.md
        font.family: Config.appearance.fontFamily.mono
    }
}
