pragma ComponentBehavior: Bound

import qs.services
import qs.config
import qs.components
import QtQuick
import QtQuick.Layouts
import QtQuick.Effects

Item {
    id: root

    required property var wrapper

    implicitWidth: 200
    implicitHeight: 200

    clip: true
    focus: true

    Keys.onTabPressed: listView.incrementCurrentIndex()
    Keys.onBacktabPressed: listView.decrementCurrentIndex()
    Keys.onReturnPressed: {
        Hypr.switchKeyboardLayout(listView.currentIndex);
    }

    ColumnLayout {
        id: header

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        anchors.margins: 4
        spacing: Config.appearance.spacing.xxs

        StyledText {
            id: headerTxt

            Layout.fillWidth: true

            text: "Current keyboard layout:"

            font.pointSize: Config.appearance.fontSize.xxs
        }

        StyledText {
            id: currentKbdLayoutText

            Layout.fillWidth: true
            elide: Text.ElideRight

            text: Hypr.kbdLayout.description ?? "Unknown layout"

            font.pointSize: Config.appearance.fontSize.md
            font.weight: 600
        }
    }

    StyledRect {
        id: bgRect

        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        // anchors.margins: 2
        anchors.topMargin: 6
        anchors.bottomMargin: 2

        color: ColorService.current.base
        radius: Config.appearance.rounding.md

        layer.enabled: true
        layer.effect: MultiEffect {
            maskEnabled: true
            maskSource: rectMask
            maskThresholdMin: 0.5
            maskSpreadAtMin: 1.0
        }

        Item {
            id: rectMask
            anchors.fill: parent
            layer.enabled: true
            visible: false

            Rectangle {
                anchors.fill: parent
                radius: bgRect.radius
                color: "white"
            }
        }

        ListView {
            id: listView

            anchors.fill: parent
            anchors.margins: 2

            spacing: 0

            model: Hypr.inputConfig?.layouts ?? 0
            keyNavigationWraps: true

            delegate: LayoutItem {}

            highlightFollowsCurrentItem: false
            highlight: Rectangle {
                readonly property int paddingH: Config.appearance.padding.xs
                readonly property int paddingV: Config.appearance.padding.xs * 0.5
                anchors.horizontalCenter: parent.horizontalCenter
                color: Hypr.isKeyboardSwitchOnCooldown ? ColorService.current.base : ColorService.current.base3
                y: listView.currentItem.y + paddingV * 0.5

                implicitWidth: listView.width - paddingH
                implicitHeight: listView.currentItem.height - paddingV
                radius: Config.appearance.rounding.md

                Behavior on color {
                    CAnim {
                        duration: 200
                    }
                }

                Behavior on y {
                    NAnim {
                        duration: 400
                    }
                }
            }
        }
    }

    component LayoutItem: Item {
        id: liRoot
        required property var modelData
        required property int index

        readonly property int padding: Config.appearance.padding.sm
        readonly property bool isCurrent: Hypr.currentIndex === index

        anchors.left: parent.left
        anchors.right: parent.right

        anchors.leftMargin: padding
        anchors.rightMargin: padding

        implicitHeight: liLangText.height

        StyledText {
            id: liLangText

            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.right: parent.right

            text: liRoot.modelData?.description ?? ""
            elide: Text.ElideRight
            padding: liRoot.padding

            color: liRoot.isCurrent ? ColorService.current.baseContent : ColorService.current.baseContentMuted

            font.weight: liRoot.isCurrent ? 600 : 500
            font.pointSize: Config.appearance.fontSize.xs
        }

        MouseArea {
            anchors.fill: parent

            hoverEnabled: true
            preventStealing: true

            cursorShape: Qt.PointingHandCursor

            onEntered: {
                listView.currentIndex = liRoot.index;
            }

            onClicked: {
                // if (!Hypr.isKeyboardSwitchOnCooldown) {
                //     root.wrapper?.close?.();
                // }
                Hypr.switchKeyboardLayout(liRoot.index);
            }
        }
    }
}
