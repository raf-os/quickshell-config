import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Controls

TextField {
    id: root

    property bool isValid: true
    property bool isDirty: false
    property string value

    activeFocusOnTab: true

    text: value

    color: Colors.colors.baseContent
    padding: Config.appearance.padding.sm
    font.pointSize: Config.appearance.fontSize.sm

    onValueChanged: {
        text = value;
    }

    onEditingFinished: {
        value = text;
    }

    background: Rectangle {
        id: bgRect

        color: root.activeFocus ? Colors.colors.base : Colors.colors.base2
        radius: Config.appearance.rounding.sm

        border.width: 2
        border.color: "transparent"

        states: [
            State {
                name: "invalid"
                when: root.isValid === false
                PropertyChanges {
                    bgRect.border.color: Colors.colors.destructive
                }
            },
            State {
                name: "active"
                when: root.isValid && root.activeFocus
                PropertyChanges {
                    bgRect.border.color: Colors.colors.primary
                }
            },
            State {
                name: "dirty"
                when: root.isValid && root.isDirty
                PropertyChanges {
                    bgRect.border.color: Colors.colors.neutralContent
                }
            }
        ]

        Behavior on border.color {
            CAnim {
                duration: 100
            }
        }
    }
}
