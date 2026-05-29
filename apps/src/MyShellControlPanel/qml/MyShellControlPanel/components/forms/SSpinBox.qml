pragma ComponentBehavior: Bound

import org.nightshell.Components
import MyShellControlPanel.components
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Controls

SpinBox {
    id: root

    activeFocusOnTab: true
    editable: true

    property color textColor: Colors.colors.baseContent
    property color selectionColor: Colors.colors.primary
    property int boxRounding: Config.appearance.rounding.sm
    property int highlightBorderWidth: 2
    property bool isValid: true
    property bool isDirty: false

    property bool useIndicatorButtons: true
    property int indicatorButtonSize: Config.appearance.fontSize.lg

    font.family: Config.appearance.fontFamily.mono
    font.pointSize: Config.appearance.fontSize.sm

    padding: Config.appearance.padding.sm

    contentItem: TextInput {
        id: textInput
        z: 2

        readonly property int horizontalSpacing: root.useIndicatorButtons ? root.indicatorButtonSize + root.padding * 3 : root.padding

        text: root.textFromValue(root.value, root.locale)

        font: root.font
        color: root.textColor
        selectionColor: root.selectionColor
        selectedTextColor: Colors.colors.baseContent

        anchors.fill: parent
        anchors.leftMargin: horizontalSpacing
        anchors.rightMargin: horizontalSpacing

        verticalAlignment: Qt.AlignVCenter

        readOnly: !root.editable
        validator: root.validator
        inputMethodHints: Qt.ImhFormattedNumbersOnly
    }

    up.indicator: IndicatorWrapper {
        anchors.right: parent.right
        isHovered: root.up.hovered
        rightRounding: true
        icon: "plus"
    }

    down.indicator: IndicatorWrapper {
        icon: "minus"
        isHovered: root.down.hovered
        leftRounding: true
    }

    background: Rectangle {
        id: bgRect

        anchors.fill: parent
        radius: root.boxRounding

        color: root.activeFocus ? Colors.colors.base : Colors.colors.base2

        Behavior on color {
            CAnim {
                duration: 100
            }
        }
    }

    Rectangle {
        id: highlight
        z: 2

        property bool isActive: root.activeFocus

        anchors.fill: parent

        color: "transparent"
        radius: root.boxRounding

        border.width: root.highlightBorderWidth
        border.color: "transparent"

        Behavior on border.color {
            CAnim {
                duration: 100
            }
        }

        states: [
            State {
                name: "invalid"
                when: root.isValid === false
                PropertyChanges {
                    highlight.border.color: Colors.colors.destructive
                }
            },
            State {
                name: "active"
                when: root.isValid && highlight.isActive
                PropertyChanges {
                    highlight.border.color: Colors.colors.primary
                }
            },
            State {
                name: "dirty"
                when: root.isValid && root.isDirty
                PropertyChanges {
                    highlight.border.color: Colors.colors.neutralContent
                }
            }
        ]
    }

    component IndicatorWrapper: Loader {
        id: indicatorWrapper

        required property string icon
        required property bool isHovered
        property bool leftRounding: false
        property bool rightRounding: false
        active: root.useIndicatorButtons

        anchors.top: parent.top
        anchors.bottom: parent.bottom

        sourceComponent: Item {
            implicitWidth: root.indicatorButtonSize + Config.appearance.padding.sm * 2

            Rectangle {
                id: hoverComp

                readonly property int rounding: root.boxRounding
                property bool leftRounding: indicatorWrapper.leftRounding
                property bool rightRounding: indicatorWrapper.rightRounding
                anchors.centerIn: parent

                anchors.fill: parent

                color: Colors.colors.base4
                opacity: indicatorWrapper.isHovered ? 1 : 0

                topLeftRadius: leftRounding ? rounding : 0
                bottomLeftRadius: leftRounding ? rounding : 0

                topRightRadius: rightRounding ? rounding : 0
                bottomRightRadius: rightRounding ? rounding : 0

                Behavior on opacity {
                    NAnim {
                        duration: 200
                    }
                }
            }

            SVGIcon {
                anchors.centerIn: parent
                iconName: indicatorWrapper.icon
                size: root.indicatorButtonSize
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                cursorShape: root.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
        }
    }
}
