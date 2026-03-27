pragma ComponentBehavior: Bound

import qs.services
import qs.config
import qs.components
import Quickshell
import Quickshell.Services.Pam
import QtQuick

Item {
    id: root

    required property Pam pam
    property string buffer
    property int bufLength

    // implicitWidth: 420
    implicitHeight: 32

    clip: true

    Connections {
        target: root.pam

        function onBufferChanged(): void {
            root.buffer = root.pam.buffer;
            root.bufLength = root.buffer.length;
        }
    }

    Rectangle {
        id: bgRect

        anchors.fill: parent

        color: ColorService.current.base0
        radius: Config.appearance.rounding.md

        border.width: 1
        border.color: ColorService.current.base3
    }

    StyledText {
        id: placeholderText

        anchors.centerIn: parent

        text: {
            if (root.pam.passwordContext.active)
                return qsTr("Authenticating...");
            if (root.pam.pamResult === PamResult.MaxTries)
                return qsTr("Maximum amount of tries exceeded.");
            if (root.pam.totalTries > 0 && root.pam.totalTries < root.pam.maxTries)
                return qsTr(`Invalid password.`);
            return qsTr("Enter password");
        }

        opacity: root.buffer ? 0 : 1

        Behavior on opacity {
            NAnim {}
        }
    }

    ListView {
        id: charList

        readonly property color charColor: ColorService.current.baseContent

        currentIndex: charList.count - 1

        anchors.fill: parent
        anchors.margins: Config.appearance.padding.md

        clip: true

        orientation: Qt.Horizontal
        interactive: false
        spacing: Config.appearance.spacing.xs

        model: ScriptModel {
            values: root.buffer.split("")
        }

        delegate: StyledRect {
            id: chr

            readonly property int size: charList.height

            implicitWidth: size
            implicitHeight: size

            radius: 1000
            color: charList.charColor

            opacity: 0
            scale: 0

            Component.onCompleted: {
                opacity = 1;
                scale = 1;
            }

            ListView.onRemove: removeAnim.start()

            SequentialAnimation {
                id: removeAnim

                PropertyAction {
                    target: chr
                    property: "ListView.delayRemove"
                    value: true
                }
                ParallelAnimation {
                    NAnim {
                        target: chr
                        property: "opacity"
                        to: 0
                        duration: 300
                    }
                    NAnim {
                        target: chr
                        property: "scale"
                        to: 0.5
                        duration: 200
                    }
                }
                PropertyAction {
                    target: chr
                    property: "ListView.delayRemove"
                    value: false
                }
            }

            Behavior on opacity {
                NAnim {
                    duration: 300
                }
            }

            Behavior on scale {
                NAnim {
                    duration: 200
                }
            }
        }
    }
}
