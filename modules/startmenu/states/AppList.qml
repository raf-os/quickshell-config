import qs.components
import qs.services
import qs.utils
import QtQuick
import Quickshell

ListView {
    id: root

    required property PersistentProperties openPanels
    required property string query

    readonly property bool hasQuery: query !== ""

    model: ScriptModel {
        values: {
            root.hasQuery ? [...AppService.filteredApps] : [...AppService.list].filter(app => app.runInTerminal !== true);
        }
    }

    clip: true
    reuseItems: true

    delegateModelAccess: DelegateModel.ReadOnly

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
