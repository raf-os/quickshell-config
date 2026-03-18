pragma ComponentBehavior: Bound

import "../items"
import qs.config
import qs.components
import qs.services
import Quickshell
import QtQuick

ListView {
    id: root

    required property PersistentProperties openPanels
    required property TextInput textInput

    property bool isSelectionActive: false

    signal sendStateMessage(message: string)

    model: UserCommandService.entries

    clip: true
    reuseItems: true
    keyNavigationWraps: true

    Component.onCompleted: {
        UserCommandService.attemptFirstLoad();
        updateQueryString();
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
            }
        }
    }

    function updateQueryString() {
        UserCommandService.setQuery(root.textInput.text.slice(Config.launcher.commandPrefix.length));
        root.currentIndex = 0;
        isSelectionActive = false;
    }

    delegateModelAccess: DelegateModel.ReadOnly

    function onKeyPressReceived(key: int): void {
        if (key === Qt.Key_Tab || key === Qt.Key_Backtab) {
            if (isSelectionActive) {
                if (key === Qt.Key_Tab)
                    root.incrementCurrentIndex();
                else if (key === Qt.Key_Backtab)
                    root.decrementCurrentIndex();
            } else {
                if (root.count === 0)
                    return;
                if (key === Qt.Key_Tab)
                    root.currentIndex = 0;
                else if (key === Qt.Key_Backtab)
                    root.currentIndex = root.count - 1;
                isSelectionActive = true;
            }
        }
    }

    highlight: Rectangle {
        implicitWidth: parent?.width ?? 100
        implicitHeight: root.currentItem?.height ?? 0

        y: root.currentItem?.y ?? 0
        color: ColorService.current.base2
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
    highlightFollowsCurrentItem: false

    delegate: CommandItem {
        openPanels: root.openPanels
    }
}
