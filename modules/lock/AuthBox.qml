pragma ComponentBehavior: Bound

import qs.services
import qs.config
import qs.components
import qs.utils
import Quickshell
import Quickshell.Services.Pam
import QtQuick
import QtQuick.Effects
import QtQuick.Layouts

Item {
    id: root

    required property var lock

    readonly property string userName: UserInfo.userName
    readonly property int boxPadding: Config.appearance.padding.lg

    signal exitAnimationFinished

    anchors.centerIn: parent

    implicitWidth: 800
    implicitHeight: 420

    opacity: 0

    focus: true
    onActiveFocusChanged: {
        if (!activeFocus)
            forceActiveFocus();
    }

    Keys.onPressed: event => {
        if (root.lock.unlocking)
            return;

        root.lock.pam.handleKey(event);
    }

    Connections {
        target: root.lock.lock

        function onUnlock() {
            enterAnim.stop();
            exitAnim.restart();
        }
    }

    Connections {
        target: root.lock.pam

        function onFlashMessage() {
            pamMessage.performAnimFlash();
        }
    }

    ParallelAnimation {
        id: enterAnim
        running: true

        NAnim {
            target: root
            property: "opacity"
            to: 1
            duration: 300
        }
        NAnim {
            target: root
            property: "anchors.verticalCenterOffset"
            from: 32
            to: 0
            duration: 400
        }
    }

    SequentialAnimation {
        id: exitAnim

        ParallelAnimation {
            NAnim {
                target: root
                property: "opacity"
                to: 0
                duration: 300
            }
            NAnim {
                target: root
                property: "anchors.verticalCenterOffset"
                to: 48
                duration: 400
            }
        }

        ScriptAction {
            script: root.exitAnimationFinished()
        }
    }

    RectangularShadow {
        anchors.fill: parent
        color: "black"

        radius: Config.appearance.rounding.md
        blur: 12
        spread: 2
        opacity: 0.25
        offset: Qt.vector2d(0, 2.0)
    }

    Rectangle {
        id: bgRect

        anchors.fill: parent

        color: ColorService.current.base
        radius: Config.appearance.rounding.md
    }

    Item {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        implicitWidth: parent.width / 2

        Image {
            id: lockBgImg
            source: `${Quickshell.shellDir}/assets/lockbg.jpg`
            asynchronous: true
            cache: false
            fillMode: Image.PreserveAspectCrop

            horizontalAlignment: Image.AlignHCenter
            verticalAlignment: Image.AlignVCenter

            anchors.fill: parent

            layer.enabled: true
            layer.effect: MultiEffect {
                maskEnabled: true
                maskSource: rectMask
            }
        }

        Rectangle {
            id: rectMask

            readonly property int rounding: Config.appearance.rounding.md

            anchors.fill: parent
            topLeftRadius: rounding
            bottomLeftRadius: rounding

            visible: false
            color: "white"

            layer.enabled: true
        }
    }

    RowLayout {
        id: layoutRoot
        anchors.fill: parent

        ColumnLayout {
            id: leftColumn
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredWidth: parent.width / 2 - root.boxPadding * 2
            Layout.margins: root.boxPadding

            StyledText {
                id: dateClock
                Layout.fillWidth: true

                text: Time.dateStr
                horizontalAlignment: Text.AlignHCenter

                font.pointSize: 16
                font.family: Config.appearance.fontFamily.sans

                color: ColorService.current.base0
            }

            StyledText {
                id: timerClock
                Layout.fillWidth: true

                text: Time.timeStr
                horizontalAlignment: Text.AlignHCenter

                font.weight: 600
                font.pointSize: 48
                font.family: Config.appearance.fontFamily.sans

                color: ColorService.current.base0
            }
        }

        ColumnLayout {
            id: rightColumn
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredWidth: parent.width / 2 - root.boxPadding * 2
            Layout.margins: root.boxPadding

            spacing: 0

            Spacer {}

            StyledText {
                id: authIcon
                Layout.fillWidth: true

                text: "󱅞"
                horizontalAlignment: Text.AlignHCenter

                font.family: Config.appearance.fontFamily.monoIcon
                font.pointSize: Config.appearance.fontSize.xxl * 4
            }

            StyledText {
                id: authBoxHeader
                Layout.fillWidth: true

                text: root.userName
                horizontalAlignment: Text.AlignHCenter

                font.weight: 600
                font.pointSize: Config.appearance.fontSize.lg
            }

            StyledText {
                id: pamMessage

                Layout.fillWidth: true
                Layout.bottomMargin: Config.appearance.spacing.sm

                readonly property Pam pam: root.lock.pam
                readonly property color baseColor: ColorService.current.baseContent
                readonly property color errorColor: ColorService.current.destructive

                readonly property string msg: {
                    color: baseColor;
                    if (pam.pamResult === PamResult.Failed) {
                        return "Authentication failed";
                    } else if (pam.pamResult === PamResult.Error && pam.lockMessage) {
                        return pam.lockMessage;
                    } else if (pam.pamResult !== null && pam.lockMessage) {
                        return pam.lockMessage;
                    }

                    return "";
                }

                function performAnimFlash() {
                    flashAnim.restart();
                }

                text: msg
                color: baseColor
                horizontalAlignment: Text.AlignHCenter

                SequentialAnimation {
                    id: flashAnim

                    CAnim {
                        target: pamMessage
                        from: pamMessage.baseColor
                        to: pamMessage.errorColor
                        property: "color"
                        duration: 100
                    }

                    CAnim {
                        target: pamMessage
                        from: pamMessage.errorColor
                        to: pamMessage.baseColor
                        property: "color"
                        duration: 1000
                        easing.bezierCurve: Config.appearance.animCurves.easeInOut
                    }
                }
            }

            InputBar {
                id: inputBar

                pam: root.lock.pam

                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Layout.leftMargin: 42
                Layout.rightMargin: 42
            }

            StyledText {
                id: maxTriesWarning

                Layout.fillWidth: true
                Layout.topMargin: 4

                readonly property int remainingTries: root.lock.pam.maxTries - root.lock.pam.totalTries
                readonly property bool shouldBeActive: root.lock.pam.totalTries > 0 && remainingTries > 0

                opacity: shouldBeActive ? 1 : 0

                text: `${remainingTries} attempts remaining.`

                horizontalAlignment: Text.AlignHCenter
                font.pointSize: Config.appearance.fontSize.xxs
            }

            Spacer {}
        }
    }

    component Spacer: Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}
