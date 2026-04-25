pragma ComponentBehavior: Bound

import qs.services
import qs.config
import qs.components
import QtQuick
import QtQuick.Layouts

RowLayout {
    id: root
    property color color: ColorService.current.baseContent
    spacing: Config.appearance.spacing.md
    // Layout.alignment: Qt.AlignVCenter
    Layout.fillHeight: true

    StyledText {
        text: `${Time.format("dd/MM")} 󰃮 ${Time.format("hh:mm")} `
        font.family: Config.appearance.fontFamily.mono
        font.pointSize: Config.appearance.fontSize.sm

        verticalAlignment: Text.AlignVCenter
    }
}
