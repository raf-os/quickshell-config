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

    property bool isFavorite: Config.launcher.favoriteApps.includes(modelData.id)

    implicitHeight: 64
    implicitWidth: parent?.width ?? 100

    function triggerItem() {
        AppService.launchDesktopApp(desktopEntry);
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

        MouseArea {
            id: favInteractionArea

            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            acceptedButtons: Qt.LeftButton
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor

            implicitWidth: 32

            onClicked: {
                if (Config.launcher.favoriteApps.includes(root.modelData.id)) {
                    Config.launcher.favoriteApps = Config.launcher.favoriteApps.filter(a => a !== root.modelData.id);
                } else {
                    // Config.launcher.favoriteApps.push(root.modelData.id);
                    Config.launcher.favoriteApps = [...Config.launcher.favoriteApps, root.modelData.id];
                }
                Config.save();
            }
        }
    }

    RowLayout {
        id: appContent
        anchors.fill: parent
        anchors.margins: Config.appearance.padding.md
        spacing: Config.appearance.spacing.sm

        IconImage {
            id: icon
            source: root.iconPath
            implicitSize: parent.height * 0.8
            asynchronous: true
        }

        Column {
            Layout.fillWidth: true
            spacing: 4

            StyledText {
                text: root.modelData.name

                font.weight: 600
                font.family: Config.appearance.fontFamily.sans
                font.pointSize: Config.appearance.fontSize.sm
            }

            StyledText {
                text: (root.modelData?.comment || root.modelData?.genericName) ?? "No description available."
                font.family: Config.appearance.fontFamily.sans
                font.pointSize: Config.appearance.fontSize.sm

                color: ColorService.current.baseContentMuted
                elide: Text.ElideRight
                width: root.width - icon.width - favIconWrapper.width - appContent.anchors.margins * 2 - parent.spacing * 2
            }
        }
    }

    Item {
        id: favIconWrapper

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        implicitWidth: favInteractionArea.implicitWidth

        Rectangle {
            property int verticalMargins: Config.appearance.padding.sm

            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            anchors.topMargin: verticalMargins
            anchors.bottomMargin: verticalMargins

            implicitWidth: 1

            color: ColorService.current.base3
        }

        StyledText {
            id: starIcon
            text: "󰓎"

            anchors.centerIn: parent

            font.pointSize: 16
            color: favInteractionArea.containsMouse ? ColorService.current.primary : root.isFavorite ? ColorService.current.emphasisFavorite : ColorService.current.baseContentMuted

            Behavior on color {
                CAnim {
                    duration: 200
                }
            }
        }
    }
}
