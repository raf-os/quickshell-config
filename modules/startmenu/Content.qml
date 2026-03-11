pragma ComponentBehavior: Bound

import "items"
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
    readonly property bool hasQuery: isActive && cmdinput?.debouncedInput && cmdinput.debouncedInput.trim().length > 0

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

    StyledText {
        text: "Start"
        font.pointSize: Config.appearance.fontSize.lg
        font.weight: 700

        Layout.margins: root.padding
    }

    Connections {
        target: root.openPanels

        function onStartmenuChanged(): void {
            if (!root.openPanels.startmenu)
                cmdinputtxt.text = "";
        }
    }

    Item {
        id: cmdinput

        property string debouncedInput: ""

        Layout.fillWidth: true
        Layout.margins: root.padding

        implicitHeight: cmdinputtxt.implicitHeight

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
            padding: 8

            onAccepted: {
                const item = lview.currentItem;
                if (!item)
                    return;

                if ((item?.itemType ?? "") === "appitem")
                    item?.triggerItem?.();
            }

            color: ColorService.current.baseContent

            onTextChanged: {
                debounceTimer.restart();
            }
        }

        Timer {
            id: debounceTimer

            interval: 250

            onTriggered: {
                const newFilter = cmdinputtxt.text;
                cmdinput.debouncedInput = newFilter;
                if (root.hasQuery) {
                    root.filteredList = AppService.query(newFilter);
                }
                if (lview)
                    lview.currentIndex = 0;
            }
        }
    }

    ListView {
        id: lview
        // HACK: moved away from "ScriptModel", but this means animations will be broken
        model: root.hasQuery ? [...AppService.filteredApps] : [...AppService.list].filter(app => {
            if (app.runInTerminal === true)
                return false;
            return true;
        })

        implicitWidth: Config.launcher.width
        Layout.fillHeight: true
        Layout.margins: root.padding
        spacing: root.padding

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

        delegate: AppItem {
            openPanels: root.openPanels
        }
    }

    Item {
        implicitWidth: 320
        Layout.fillHeight: true
    }
}
