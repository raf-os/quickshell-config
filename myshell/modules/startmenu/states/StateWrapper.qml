import qs.components
import MyShellPlugin.Configs
import Quickshell
import QtQuick
import QtQuick.Controls

SListView {
    id: root

    required property TextInput textInput
    required property PersistentProperties openPanels

    implicitWidth: StackView.view ? StackView.view.width : 0
    implicitHeight: StackView.view ? StackView.view.height : 0
    spacing: Config.appearance.spacing.sm

    signal sendStateMessage(message: string)

    function moveForwards() {
        listView.incrementCurrentIndex();
    }
    function moveBackwards() {
        listView.decrementCurrentIndex();
    }
}
