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
    readonly property int rounding: Config.appearance.rounding.md
    readonly property real padding: Config.appearance.padding.lg
    readonly property bool isActive: openPanels.startmenu
    readonly property bool hasQuery: isActive && cmdinput?.debouncedInput && cmdinput.debouncedInput.length > 0

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

    // Connections {
    //     target: root.openPanels
    //
    //     function onStartmenuChanged(): void {
    //         if (!root.openPanels.startmenu)
    //             cmdinputtxt.text = "";
    //     }
    // }

    function showStateMessage(message: string): void {
    }

    Item {
        id: stateMessage
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

        TextInput {
            id: cmdinputtxt

            focus: root.isActive
            // activeFocusOnTab: true

            font.pointSize: Config.appearance.fontSize.md
            font.family: Config.appearance.fontFamily.mono

            anchors.left: parent.left
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
                }
            }
        }
    }

    Item {
        id: stateWrapper

        property StateWrapper activeChildItem: children.find(child => child.active === true)

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

                anchors.fill: parent

                spacing: root.padding

                openPanels: root.openPanels
                textInput: cmdinputtxt
            }
        }
    }
}
