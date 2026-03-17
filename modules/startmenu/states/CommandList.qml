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

    model: UserCommandService.entries

    clip: true
    reuseItems: true

    Component.onCompleted: {
        UserCommandService.attemptFirstLoad();
    }

    Connections {
        target: root.textInput

        function onTextChanged() {
            UserCommandService.setQuery(root.textInput.text.slice(Config.launcher.commandPrefix.length));
        }
    }

    delegateModelAccess: DelegateModel.ReadOnly

    function onKeyPressReceived(key: int): void {
    }

    highlight: Rectangle {
        implicitWidth: parent?.width ?? 100
        implicitHeight: 32
    }

    delegate: CommandItem {
        openPanels: root.openPanels
    }
}
