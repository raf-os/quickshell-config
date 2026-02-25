import qs.services
import qs.config
import QtQuick
import QtQuick.Effects
import Quickshell.Widgets

Item {
    id: root

    implicitWidth: Config.appearance.fontSize.lg * 1.2
    implicitHeight: Config.appearance.fontSize.lg * 1.2

    IconImage {
        anchors.centerIn: parent
        source: SysInfo.osLogo
        implicitSize: Config.appearance.fontSize.lg * 1.2

        layer.enabled: true
        layer.effect: MultiEffect {
            brightness: 0.5
            colorization: 1
            colorizationColor: ColorService.current.baseContent
        }
    }
}
