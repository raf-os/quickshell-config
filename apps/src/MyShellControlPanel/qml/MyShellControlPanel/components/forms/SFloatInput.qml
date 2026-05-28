pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.plugin
import MyShellControlPanel.components
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

FormInput {
    id: root

    // required property string name
    // property real initialValue
    // property real value: initialValue
    property int textInset: Config.appearance.spacing.sm
    property bool boxLayoutFillWidth: false

    // property bool isDirty: value !== initialValue
    // property bool isValid: true

    property alias stepSize: control.stepSize
    property alias from: control.from
    property alias to: control.to

    Layout.fillWidth: true
    implicitHeight: layout.implicitHeight

    inputItem: control

    ColumnLayout {
        id: layout

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        StyledText {
            id: label

            text: root.name

            Layout.fillWidth: true
            elide: Text.ElideRight
            leftPadding: root.textInset

            font.pointSize: Config.appearance.fontSize.sm
            font.weight: 600
        }

        DoubleSpinBox {
            id: control

            readonly property int rounding: Config.appearance.rounding.sm
            readonly property int buttonSize: 32

            font.family: Config.appearance.fontFamily.mono
            font.pointSize: Config.appearance.fontSize.sm

            Layout.fillWidth: root.boxLayoutFillWidth
            Layout.margins: 2
            decimals: 2
            editable: true
            activeFocusOnTab: true

            // value: root.initialValue

            validator: DoubleValidator {
                bottom: root.from
                top: root.to
                decimals: 2
                notation: DoubleValidator.StandardNotation
            }

            Rectangle {
                id: highlight
                readonly property bool isActive: control.activeFocus
                readonly property real sizeOffset: 4
                z: -1
                anchors.centerIn: parent

                implicitWidth: parent.width + sizeOffset
                implicitHeight: parent.height + sizeOffset
                radius: control.rounding

                color: "transparent"

                states: [
                    State {
                        name: "invalid"
                        when: root.isValid == false
                        PropertyChanges {
                            highlight.color: Colors.colors.destructive
                        }
                    },
                    State {
                        name: "active"
                        when: root.isValid && highlight.isActive
                        PropertyChanges {
                            highlight.color: Colors.colors.primary
                        }
                    },
                    State {
                        name: "dirty"
                        when: root.isValid && root.isDirty
                        PropertyChanges {
                            highlight.color: Colors.colors.base5
                        }
                    }
                ]

                Behavior on color {
                    CAnim {
                        duration: 100
                    }
                }
            }

            contentItem: TextInput {
                z: 2
                text: control.textFromValue(control.value, 2, control.locale)

                font: control.font
                color: Colors.colors.baseContent
                selectionColor: Colors.colors.primary
                selectedTextColor: Colors.colors.baseContent

                anchors.fill: parent
                anchors.leftMargin: control.buttonSize
                anchors.rightMargin: control.buttonSize

                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter

                readOnly: !control.editable
                validator: control.validator
                inputMethodHints: Qt.ImhFormattedNumbersOnly
            }

            down.indicator: SpinboxIndicator {
                x: 0
                text: "remove"

                topLeftRadius: control.rounding
                bottomLeftRadius: control.rounding

                isHovered: control.down.hovered
            }

            up.indicator: SpinboxIndicator {
                x: parent.width - implicitWidth
                text: "add"

                topRightRadius: control.rounding
                bottomRightRadius: control.rounding

                isHovered: control.up.hovered
            }

            background: Item {
                Rectangle {
                    color: Colors.colors.base2

                    border.width: 0

                    anchors.centerIn: parent

                    implicitWidth: control.contentItem.width
                    implicitHeight: control.contentItem.height
                }
            }
        }
    }

    component SpinboxIndicator: Rectangle {
        id: indicatorComponent
        property bool isHovered: false
        property string text

        anchors.top: parent.top
        anchors.bottom: parent.bottom

        implicitWidth: control.buttonSize
        height: parent.height

        color: isHovered ? Colors.colors.base4 : Colors.colors.base

        SMaterialIcon {
            text: indicatorComponent.text
            grade: 200
            anchors.fill: parent
            fontSizeMode: Text.Fit
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            cursorShape: Qt.PointingHandCursor
        }
    }
}
