pragma ComponentBehavior: Bound

import "../items"
import qs.components
import qs.services
import MyShellPlugin
import MyShellPlugin.Configs
import Quickshell
import QtQuick

StateWrapper {
    id: root

    required property ShellScreen screen

    property bool isSelectionActive: false

    model: UserCommandService.entries

    clip: true
    listView.reuseItems: true
    listView.keyNavigationWraps: true

    Component.onCompleted: {
        UserCommandService.attemptFirstLoad();
        updateQueryString();
    }

    function getCommandString(): string {
        return root.textInput.text.slice(Config.launcher.commandPrefix.length);
    }

    Connections {
        target: root.textInput

        function onTextChanged() {
            root.updateQueryString();
        }

        function onAccepted() {
            if (root.isSelectionActive) {
                const cmdPrefix = root.currentItem?.modelData?.prefix ?? "";
                const cmdSeparator = root.currentItem?.modelData?.separator ?? " ";
                root.textInput.text = `${Config.launcher.commandPrefix}${cmdPrefix}${cmdSeparator}`;
                root.isSelectionActive = false;
            } else {
                const msg = UserCommandService.executeCommand(root.getCommandString());
                const isSuccess = msg["success"] === true ?? false;
                if (!isSuccess) {
                    root.sendStateMessage(msg["message"]);
                } else {
                    root.openPanels.startmenu = false;
                }
            }
        }
    }

    function updateQueryString() {
        UserCommandService.setQuery(root.getCommandString());
        root.currentIndex = 0;
        isSelectionActive = false;
    }

    function onKeyPressReceived(key: int): void {
        const isDownMovement = (key === Qt.Key_Tab || key === Qt.Key_Down);
        const isUpMovement = (key === Qt.Key_Backtab || key === Qt.Key_Up);

        if (isSelectionActive) {
            if (isDownMovement)
                root.listView.incrementCurrentIndex();
            else if (isUpMovement)
                root.listView.decrementCurrentIndex();
        } else {
            if (root.count === 0)
                return;
            if (isDownMovement)
                root.currentIndex = 0;
            else if (isUpMovement)
                root.currentIndex = root.count - 1;
            isSelectionActive = true;
        }
    }

    highlight: Rectangle {
        implicitWidth: parent?.width ?? 100
        implicitHeight: root.currentItem?.height ?? 0

        y: root.currentItem?.y ?? 0
        color: Colors.colors.base2
        radius: Config.appearance.rounding.sm

        opacity: root.isSelectionActive ? 1 : 0

        Behavior on y {
            NAnim {
                duration: 300
                easing.bezierCurve: Config.appearance.animCurves.defaultEase
            }
        }

        Behavior on opacity {
            NAnim {}
        }
    }
    listView.highlightFollowsCurrentItem: false

    delegate: CommandItem {
        openPanels: root.openPanels
    }
}
