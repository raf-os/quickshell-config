pragma ComponentBehavior: Bound

import "items"
import "states"
import qs.components
import qs.services
import qs.config
import Quickshell
import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: root

    required property PersistentProperties openPanels
    required property ShellScreen screen
    readonly property int rounding: Config.appearance.rounding.md
    readonly property real padding: Config.appearance.padding.lg
    readonly property bool isActive: openPanels.startmenu
    readonly property bool hasQuery: isActive && cmdinput?.debouncedInput && cmdinput.debouncedInput.length > 0

    property string errorMessage: ""
    // property string mode: openPanels.desiredStartMenuTab === "" ? "apps" : openPanels.desiredStartMenuTab ?? "apps"
    property string mode: "apps"
    // Modes: apps | command

    property list<QtObject> filteredList

    focus: isActive

    spacing: -padding

    Keys.onEscapePressed: {
        openPanels.startmenu = false;
    }

    Keys.onUpPressed: stateWrapper.receiveKeyPress(Qt.Key_Up)
    Keys.onDownPressed: stateWrapper.receiveKeyPress(Qt.Key_Down)
    Keys.onTabPressed: stateWrapper.receiveKeyPress(Qt.Key_Tab)
    Keys.onBacktabPressed: stateWrapper.receiveKeyPress(Qt.Key_Backtab)

    function forceFocusInput() {
        cmdinputtxt?.forceActiveFocus();
    }

    StateTitle {
        currentState: root.mode
        padding: root.padding
    }

    Component.onCompleted: {
        checkDesiredTab();
    }

    function checkDesiredTab() {
        const desiredTab = root.openPanels.desiredStartMenuTab;
        console.log(desiredTab);
        if (desiredTab === "command") {
            root.mode = "command";
            cmdinputtxt.text = Config.launcher.commandPrefix;
        }
        root.openPanels.desiredStartMenuTab = "";
    }

    // Connections {
    //     target: root.openPanels
    //
    //     function onStartmenuChanged(): void {
    //         if (!root.openPanels.startmenu)
    //             cmdinputtxt.text = "";
    //     }
    // }

    function showStateMessage(message: string): void {
        root.errorMessage = message;
        stateMessage.displayMessage();
    }

    function dismissStateMessage(): void {
        stateMessage.clearMessage();
    }

    Item {
        id: cmdinput

        property string debouncedInput: ""

        Layout.fillWidth: true
        Layout.margins: root.padding

        implicitHeight: cmdinputtxt.implicitHeight

        clip: true

        Rectangle {
            id: cmdinputbg
            anchors.fill: parent
            radius: Config.appearance.rounding.sm

            color: ColorService.current.base0
        }

        Item {
            id: modeIconWrapper

            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            implicitWidth: 24

            StyledText {
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right

                font.family: Config.appearance.fontFamily.monoIcon
                font.pixelSize: 24

                verticalAlignment: Text.AlignVCenter

                text: {
                    switch (root.mode) {
                    case "apps":
                        return "󰍉";
                    case "command":
                        return "";
                    default:
                        return "";
                    }
                }
            }
        }

        TextInput {
            id: cmdinputtxt

            focus: root.isActive
            // activeFocusOnTab: true

            font.pointSize: Config.appearance.fontSize.md
            font.family: Config.appearance.fontFamily.mono

            anchors.left: modeIconWrapper.right
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            padding: Config.appearance.spacing.sm

            color: ColorService.current.baseContent

            onTextChanged: {
                if (!root.isActive) {
                    return;
                }
                if (text.startsWith(Config.launcher.commandPrefix)) {
                    root.mode = "command";
                } else {
                    root.mode = "apps";
                    root.dismissStateMessage();
                }
            }
        }
    }

    Loader {
        id: stateMessage

        property bool isActive: false

        active: isActive
        visible: isActive

        Layout.fillWidth: true
        Layout.margins: root.padding

        signal clearMessage

        function displayMessage() {
            stateMessage.isActive = true;
            if (item) {
                item?.exitTimer?.restart();
            }
        }

        sourceComponent: Item {
            id: errorWrapper

            readonly property real initialHeight: errorText.implicitHeight ?? 24

            implicitWidth: parent.width - root.padding * 2
            implicitHeight: initialHeight

            clip: true
            scale: 0.8
            opacity: 0

            Component.onCompleted: {
                entryAnim.start();
                exitTimer.start();
            }

            Connections {
                target: stateMessage

                function onClearMessage() {
                    if (entryAnim.running)
                        entryAnim.stop();
                    exitAnim.start();
                }
            }

            Timer {
                id: exitTimer
                interval: 3000

                onTriggered: {
                    if (entryAnim.running)
                        entryAnim.stop();
                    exitAnim.start();
                }
            }

            Behavior on implicitHeight {
                NAnim {
                    duration: 300
                }
            }

            ParallelAnimation {
                id: entryAnim

                NAnim {
                    target: errorWrapper
                    property: "opacity"
                    to: 1
                    easing.bezierCurve: Config.appearance.animCurves.easeInOut
                    duration: 400
                }

                NAnim {
                    target: errorWrapper
                    property: "scale"
                    to: 1
                    easing.bezierCurve: Config.appearance.animCurves.accelerateOverCorrect
                    duration: 500
                }
            }

            SequentialAnimation {
                id: exitAnim

                NAnim {
                    target: errorWrapper
                    property: "opacity"
                    to: 0
                    duration: 300
                    easing.bezierCurve: Config.appearance.animCurves.linear
                }
                ScriptAction {
                    script: stateMessage.isActive = false
                }
            }

            StyledRect {
                color: ColorService.current.destructive

                anchors.fill: parent

                radius: Config.appearance.rounding.sm
            }

            StyledText {
                id: errorText

                anchors.fill: parent

                width: parent.width
                text: root.errorMessage
                color: ColorService.current.baseContent

                topPadding: 6
                bottomPadding: 6
                leftPadding: 10
                rightPadding: 10
                font.pointSize: Config.appearance.fontSize.sm

                maximumLineCount: 6
                wrapMode: Text.Wrap
            }
        }
    }

    Item {
        id: stateWrapper

        property var activeChildItem: children.find(child => child.active === true)

        implicitWidth: Config.launcher.width

        Layout.fillHeight: true

        function receiveKeyPress(key: int): void {
            const activeList = activeChildItem.item;
            if (!activeList)
                return;
            activeList?.onKeyPressReceived?.(key);
        }

        StateWrapper {
            id: appStateWrap
            myState: "apps"
            currentState: root.mode

            sourceComponent: AppList {
                id: lview

                textInput: cmdinputtxt

                anchors.fill: parent
                spacing: root.padding

                onSendStateMessage: message => root.showStateMessage(message)

                delegate: AppItem {
                    openPanels: root.openPanels
                }
            }
        }

        StateWrapper {
            id: cmdStateWrap
            myState: "command"
            currentState: root.mode

            sourceComponent: CommandList {
                id: cview

                screen: root.screen

                anchors.fill: parent

                spacing: root.padding

                openPanels: root.openPanels
                textInput: cmdinputtxt

                onSendStateMessage: message => root.showStateMessage(message)
            }
        }
    }
}
