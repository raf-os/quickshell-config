import qs.components
import qs.services
import qs.utils
import QtQuick
import Quickshell

ListView {
    id: root

    required property TextInput textInput

    readonly property bool hasQuery: debouncedInput !== ""

    property string debouncedInput
    property list<QtObject> filteredList

    model: ScriptModel {
        values: {
            root.hasQuery ? [...AppService.filteredApps] : [...AppService.list].filter(app => app.runInTerminal !== true);
        }
    }

    clip: true
    reuseItems: true

    delegateModelAccess: DelegateModel.ReadOnly

    Connections {
        target: root.textInput

        function onTextChanged() {
            debounceTimer.restart();
        }

        function onAccepted() {
            const item = root.currentItem;
            if (!item)
                return;

            item?.triggerItem?.();
        }
    }

    function onKeyPressReceived(key: int): void {
        if (key === Qt.Key_Up || key === Qt.Key_Backtab) {
            root.decrementCurrentIndex();
        } else if (key === Qt.Key_Down || key === Qt.Key_Tab) {
            root.incrementCurrentIndex();
        }
    }

    Timer {
        id: debounceTimer
        interval: 250

        onTriggered: {
            const newFilter = root.textInput.text.trim();
            if (newFilter == root.debouncedInput)
                return;

            root.debouncedInput = newFilter;
            root.filteredList = AppService.query(newFilter);
            root.currentIndex = 0;
        }
    }

    add: Transition {
        enabled: !GlobalStateManager.isGameMode
        ParallelAnimation {
            NAnim {
                property: "x"
                from: -root.width
                to: 0
                duration: 300
            }
            NAnim {
                property: "opacity"
                from: 0
                to: 1
                duration: 150
            }
        }
    }
    displaced: Transition {
        enabled: !GlobalStateManager.isGameMode
        ParallelAnimation {
            NAnim {
                property: "x"
                from: -root.width
                to: 0
                duration: 300
            }
            NAnim {
                property: "opacity"
                from: 0
                to: 1
                duration: 150
            }
        }
    }
    remove: Transition {
        enabled: !GlobalStateManager.isGameMode
        ParallelAnimation {
            NAnim {
                property: "opacity"
                to: 0
                duration: 150
            }
            NAnim {
                property: "x"
                to: root.width
                duration: 300
            }
        }
    }
}
