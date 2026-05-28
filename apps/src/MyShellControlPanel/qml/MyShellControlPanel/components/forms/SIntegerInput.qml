pragma ComponentBehavior: Bound

import org.nightshell.Components
import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.plugin
import MyShellControlPanel.components
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

FormInput {
    id: root

    enum Type {
        Horizontal = 0,
        Vertical = 1
    }

    property int layoutType: SIntegerInput.Type.Vertical
    property int spacing: Config.appearance.spacing.sm
    property int textInset: Config.appearance.spacing.sm
    property bool boxLayoutFillWidth: true

    property bool useSliderForNumbers: false

    property alias from: control.from
    property alias to: control.to

    Layout.fillWidth: true
    implicitHeight: layout.implicitHeight

    inputItem: control

    GridLayout {
        id: layout

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        states: [
            State {
                when: root.layoutType === SIntegerInput.Type.Horizontal
                PropertyChanges {
                    layout.rows: 1
                    layout.columns: 2
                    layout.columnSpacing: root.spacing
                }
            },
            State {
                when: root.layoutType === SIntegerInput.Type.Vertical
                PropertyChanges {
                    layout.columns: 1
                    layout.rowSpacing: root.spacing
                }
            }
        ]

        StyledText {
            id: label

            text: root.name
            Layout.fillWidth: true
            elide: Text.ElideRight
            leftPadding: root.textInset

            font.pointSize: Config.appearance.fontSize.sm
            font.weight: 600
        }

        SSpinBox {
            id: control

            Layout.fillWidth: root.boxLayoutFillWidth

            isDirty: root.isDirty
            isValid: root.isValid
        }

        Loader {
            id: sliderLoader
            active: root.useSliderForNumbers

            Layout.fillWidth: true

            sourceComponent: SSlider {
                id: slider
                from: control.from
                to: control.to

                activeFocusOnTab: false

                padding: 4

                stepSize: 1
                snapMode: Slider.SnapOnRelease

                value: root.value

                onMoved: {
                    control.value = value;
                }

                Connections {
                    target: root

                    function onValueChanged() {
                        slider.value = root.value;
                    }
                }
            }
        }
    }
}
