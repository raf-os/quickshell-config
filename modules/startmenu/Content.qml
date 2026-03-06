pragma ComponentBehavior: Bound

import "items"
import qs.components
import qs.services
import qs.config
import Quickshell
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: root

    required property PersistentProperties openPanels
    readonly property int rounding: Config.appearance.rounding.md
    readonly property real padding: Config.appearance.padding.lg
    readonly property bool isActive: openPanels.startmenu

    focus: isActive

    spacing: -padding

    Keys.onEscapePressed: {
        openPanels.startmenu = false;
    }

    Keys.onUpPressed: lview.decrementCurrentIndex()
    Keys.onDownPressed: lview.incrementCurrentIndex()

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
            Component.onCompleted: {
                forceActiveFocus(Qt.PopupFocusReason);
            }

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

            interval: 1000

            onTriggered: {
                cmdinput.debouncedInput = cmdinputtxt.text;
                if (lview)
                    lview.currentIndex = 0;
            }
        }
    }

    ListView {
        id: lview
        model: ScriptModel {
            values: [...DesktopEntries.applications.values].filter(app => {
                if (app.runInTerminal === true)
                    return false;
                if (!cmdinput.debouncedInput || cmdinput.debouncedInput === "")
                    return true;
                return app.name.toLowerCase().startsWith(cmdinput.debouncedInput.toLowerCase());
            })
        }

        implicitWidth: 320
        Layout.fillHeight: true
        Layout.margins: root.padding
        spacing: root.padding

        clip: true

        delegateModelAccess: DelegateModel.ReadOnly

        delegate: AppItem {
            openPanels: root.openPanels
        }
    }

    Item {
        implicitWidth: 320
        Layout.fillHeight: true
    }
}
