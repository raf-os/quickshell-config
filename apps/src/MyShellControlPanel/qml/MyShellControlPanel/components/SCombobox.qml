pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Effects
import QtQuick.Controls

ComboBox {
    id: root
    model: ["Default", "test"]

    readonly property int textPadding: Config.appearance.padding.md
    font.family: Config.appearance.fontFamily.sans
    font.pointSize: Config.appearance.fontSize.sm

    implicitWidth: 320

    delegate: ItemDelegate {
        id: delegate

        required property var model
        required property int index

        width: root.width
        background: null
        contentItem: StyledText {
            text: delegate.model[root.textRole]
            color: Colors.colors.baseContent
            font: root.font
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
        highlighted: root.highlightedIndex === index
    }

    indicator: StyledText {
        x: root.width - width

        width: root.height
        height: root.height
        text: ""

        font.family: Config.appearance.fontFamily.mono
        font.pointSize: Config.appearance.fontSize.md

        opacity: root.popup.visible ? 1 : 0.5
        scale: root.popup.visible ? 1.25 : 1

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter

        Behavior on opacity {
            NAnim {}
        }

        Behavior on scale {
            NAnim {}
        }
    }

    contentItem: StyledText {
        padding: root.textPadding

        text: root.displayText
        font: root.font
    }

    background: Rectangle {
        // implicitWidth: 320
        // implicitHeight: root.font.pointSize

        color: Colors.colors.base
        border.width: root.visualFocus ? 2 : 1
        border.color: root.visualFocus ? Colors.colors.primary : root.popup.visible ? Colors.colors.base5 : Colors.colors.base3
        radius: Config.appearance.rounding.md
    }

    popup: Popup {
        id: rootPopup
        y: root.height + 4
        width: root.width
        height: contentItem.implicitHeight + padding * 2
        padding: 4

        readonly property int animDuration: 200

        enter: Transition {
            ParallelAnimation {
                NAnim {
                    property: "scale"
                    from: 0.9
                    to: 1
                    duration: rootPopup.animDuration
                }
                NAnim {
                    property: "opacity"
                    from: 0
                    to: 1
                    duration: rootPopup.animDuration
                }
            }
        }

        exit: Transition {
            ParallelAnimation {
                NAnim {
                    property: "scale"
                    from: 1
                    to: 0.9
                    duration: rootPopup.animDuration
                }
                NAnim {
                    property: "opacity"
                    from: 1
                    to: 0
                    duration: rootPopup.animDuration
                }
            }
        }

        contentItem: ListView {
            id: pplv
            clip: true
            implicitHeight: contentHeight
            spacing: 0
            model: root.popup.visible ? root.delegateModel : null
            currentIndex: root.highlightedIndex

            highlight: Rectangle {
                id: highlightRect
                y: ListView.view?.currentItem ? ListView.view.currentItem.y : 0
                implicitWidth: ListView.view ? ListView.view.width : 0
                implicitHeight: ListView.view?.currentItem ? ListView.view.currentItem.implicitHeight : 0

                property bool enableAnim: false

                color: Colors.colors.base4
                radius: Config.appearance.rounding.md

                Component.onCompleted: {
                    Qt.callLater(() => highlightRect.enableAnim = true);
                }

                Behavior on y {
                    enabled: highlightRect.enableAnim
                    NAnim {
                        duration: 200
                    }
                }
            }
            highlightFollowsCurrentItem: false
        }

        background: Item {
            RectangularShadow {
                anchors.fill: parent
                offset: Qt.vector2d(0, 4)
                radius: Config.appearance.rounding.md
                color: Qt.rgba(0.0, 0.0, 0.0, 0.25)
                blur: 8
            }
            Rectangle {
                anchors.fill: parent
                radius: Config.appearance.rounding.md
                color: Colors.colors.base2
                border.width: 1
                border.color: Colors.colors.base3
            }
        }
    }
}
