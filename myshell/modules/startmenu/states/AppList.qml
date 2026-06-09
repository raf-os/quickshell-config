import qs.components
import qs.services
import QtQuick
import Quickshell

StateWrapper {
    id: root

    readonly property bool hasQuery: debouncedInput !== ""

    property string debouncedInput
    property list<QtObject> filteredList

    model: ScriptModel {
        values: {
            root.hasQuery ? [...AppService.filteredApps] : [...AppService.list].filter(app => app.runInTerminal !== true);
        }
    }

    listView.reuseItems: true

    Component.onCompleted: {
        debouncedInput = textInput.text.trim();
    }

    Connections {
        target: root.textInput

        function onTextChanged() {
            debounceTimer.restart();
        }

        function onAccepted() {
            const item = root.listView.currentItem;
            if (!item)
                return;

            item?.triggerItem?.();
        }
    }

    function onKeyPressReceived(key: int): void {
        if (key === Qt.Key_Up || key === Qt.Key_Backtab) {
            root.listView.decrementCurrentIndex();
        } else if (key === Qt.Key_Down || key === Qt.Key_Tab) {
            root.listView.incrementCurrentIndex();
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
}
