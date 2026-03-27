import QtQuick
import qs.services
import qs.config

Text {
    id: root

    color: ColorService.current.baseContent

    font.family: Config.appearance.fontFamily.sans
    font.pointSize: Config.appearance.fontSize.sm
}
