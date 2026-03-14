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

    Keys.onUpPressed: lview.decrementCurrentIndex()
    Keys.onDownPressed: lview.incrementCurrentIndex()

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
            activeFocusOnTab: true

            font.pointSize: Config.appearance.fontSize.md
            font.family: Config.appearance.fontFamily.mono

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            padding: Config.appearance.spacing.sm

            onAccepted: {
                const item = lview.currentItem;
                if (!item)
                    return;

                if ((item?.itemType ?? "") === "appitem")
                    item?.triggerItem?.();
            }

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
                debounceTimer.restart();
            }
        }

        Timer {
            id: debounceTimer

            interval: 250

            onTriggered: {
                const newFilter = cmdinputtxt.text.trim();
                if (newFilter === cmdinput.debouncedInput)
                    return;
                cmdinput.debouncedInput = newFilter;
                if (!root.isActive)
                    return;
                if (root.hasQuery) {
                    root.filteredList = AppService.query(newFilter);
                }
                if (lview)
                    lview.currentIndex = 0;
            }
        }
    }

    AppList {
        id: lview

        query: cmdinput.debouncedInput

        visible: root.mode === "apps"

        implicitWidth: Config.launcher.width - root.padding * 2
        Layout.fillHeight: true
        Layout.margins: root.padding
        spacing: root.padding

        delegate: AppItem {
            openPanels: root.openPanels
        }
    }

    CommandList {
        id: cview

        visible: root.mode === "command"
        cmdQuery: cmdinputtxt.text

        implicitWidth: Config.launcher.width - root.padding * 2
        Layout.fillHeight: true
        Layout.margins: root.padding
        spacing: root.padding
    }

    Item {
        implicitWidth: Config.launcher.width
        Layout.fillHeight: true
    }
}
