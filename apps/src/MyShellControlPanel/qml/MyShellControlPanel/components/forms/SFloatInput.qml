pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.components
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    required property string name
    required property real value
    property int textInset: Config.appearance.spacing.md
    property bool boxLayoutFillWidth: false

    property alias stepSize: control.stepSize
    property alias from: control.from
    property alias to: control.to

    Layout.fillWidth: true
    implicitHeight: layout.implicitHeight

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
            readonly property int buttonSize: 24

            font.family: Config.appearance.fontFamily.mono
            font.pointSize: Config.appearance.fontSize.sm

            Layout.fillWidth: root.boxLayoutFillWidth
            decimals: 2
            editable: true

            value: root.value

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

                topLeftRadius: control.rounding
                bottomLeftRadius: control.rounding

                isHovered: control.down.hovered
            }

            up.indicator: SpinboxIndicator {
                x: parent.width - implicitWidth

                topRightRadius: control.rounding
                bottomRightRadius: control.rounding

                isHovered: control.up.hovered
            }

            background: Item {
                anchors.fill: parent
                Rectangle {
                    color: Colors.colors.base2
                    radius: control.rounding

                    border.width: 0

                    anchors.fill: parent
                }
            }
        }
    }

    component SpinboxIndicator: Rectangle {
        property bool isHovered: false

        anchors.top: parent.top
        anchors.bottom: parent.bottom

        implicitWidth: control.buttonSize
        height: parent.height

        color: isHovered ? Colors.colors.primary : Colors.colors.base
    }
}
