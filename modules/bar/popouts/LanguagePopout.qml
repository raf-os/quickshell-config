pragma ComponentBehavior: Bound

import qs.services
import qs.config
import qs.components
import QtQuick
import QtQuick.Effects

Item {
    id: root

    required property var wrapper

    implicitWidth: 200
    implicitHeight: 160

    StyledRect {
        id: bgRect
        anchors.fill: parent
        anchors.margins: 2
        // clip: true

        color: ColorService.current.base
        radius: Config.appearance.rounding.md

        layer.enabled: true
        layer.effect: MultiEffect {
            maskEnabled: true
            maskSource: rectMask
        }

        ListView {
            id: listView

            anchors.fill: parent
            anchors.margins: Config.appearance.padding.xs * 0.5

            spacing: 0

            model: Hypr.inputConfig?.layouts ?? 0

            delegate: LayoutItem {}

            highlightFollowsCurrentItem: false
            highlight: Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                color: Hypr.isKeyboardSwitchOnCooldown ? ColorService.current.base2 : ColorService.current.base4
                y: listView.itemAtIndex(Hypr.currentIndex).y

                implicitWidth: listView.width
                implicitHeight: listView.currentItem.height

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

    Item {
        id: rectMask
        anchors.fill: bgRect
        layer.enabled: true
        visible: false

        Rectangle {
            anchors.fill: parent
            radius: bgRect.radius
        }
    }

    component LayoutItem: Item {
        id: liRoot
        required property var modelData
        required property int index

        readonly property int padding: Config.appearance.padding.sm

        anchors.left: parent.left
        anchors.right: parent.right

        implicitHeight: liLangText.height

        StyledText {
            id: liLangText

            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.right: parent.right

            text: liRoot.modelData?.description ?? ""
            elide: Text.ElideRight
            padding: liRoot.padding

            color: Hypr.currentIndex === liRoot.index ? ColorService.current.baseContent : ColorService.current.baseContentMuted

            font.pointSize: Config.appearance.fontSize.sm
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
                Hypr.switchKeyboardLayout(liRoot.index);
            }
        }
    }
}
