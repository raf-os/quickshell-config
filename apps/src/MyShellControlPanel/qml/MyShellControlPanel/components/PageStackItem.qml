import MyShellControlPanel.components
import QtQuick
import QtQuick.Controls

FocusScope {
    id: root

    default property alias content: scrollable.content

    implicitWidth: StackView.view ? StackView.view.width : 0
    implicitHeight: StackView.view ? StackView.view.height : 0

    required property string title
    property string path: ""

    activeFocusOnTab: true

    SSCrollable {
        id: scrollable

        anchors.fill: parent

        desiredHeight: root.implicitHeight
    }
}
