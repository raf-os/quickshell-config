pragma ComponentBehavior: Bound

import qs.components
import qs.config
import qs.services
import MyShellPlugin
import Quickshell
import Quickshell.Widgets
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    readonly property string itemType: "appitem"

    required property AppEntry modelData
    required property PersistentProperties openPanels

    readonly property DesktopEntry desktopEntry: DesktopEntries.byId(modelData.id)

    property string iconPath: Quickshell.iconPath(desktopEntry?.icon)
    readonly property int boxRounding: Config.appearance.rounding.sm

    implicitHeight: 64
    anchors.left: parent?.left
    anchors.right: parent?.right

    function triggerItem() {
        AppService.launchDesktopApp(modelData);
        root.openPanels.startmenu = false;
    }

    Rectangle {
        id: rectBg

        anchors.fill: parent
        color: interactionArea.containsMouse || root.ListView.isCurrentItem ? ColorService.current.base2 : ColorService.current.base0
        radius: root.boxRounding
    }

    MouseArea {
        id: interactionArea

        anchors.fill: parent

        acceptedButtons: Qt.LeftButton
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true

        onClicked: ev => root.triggerItem()
    }

    RowLayout {
        id: appContent
        anchors.fill: parent
        anchors.margins: 8
        spacing: Config.appearance.spacing.sm

        IconImage {
            id: icon
            source: root.iconPath
            implicitSize: parent.height * 0.65
            asynchronous: true
        }

        Column {
            Layout.fillWidth: true
            spacing: 4

            StyledText {
                text: root.modelData.name

                font.weight: 600
            }

            StyledText {
                text: (root.modelData?.comment || root.modelData?.genericName) ?? "No description available."

                color: ColorService.current.baseContentMuted
                elide: Text.ElideRight
                width: root.width - icon.width - appContent.anchors.margins * 2 - parent.spacing
            }
        }
    }
}
