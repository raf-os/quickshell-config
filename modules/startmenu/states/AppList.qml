import qs.components
import qs.services
import QtQuick
import Quickshell

ListView {
    id: root

    required property PersistentProperties openPanels
    required property string query

    readonly property bool hasQuery: query.trim() !== ""

    model: ScriptModel {
        values: {
            root.hasQuery ? [...AppService.filteredApps] : [...AppService.list].filter(app => app.runInTerminal !== true);
        }
    }

    clip: true

    delegateModelAccess: DelegateModel.ReadOnly

    remove: Transition {
        NAnim {
            property: "opacity"
            to: 0
            duration: 150
        }
    }

    move: Transition {
        NumberAnimation {
            properties: "x,y"
            duration: 150
        }
    }
}
