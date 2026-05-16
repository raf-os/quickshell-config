import MyShellControlPanel.components
import QtQuick
import QtQuick.Controls

Item {
    id: root

    default property alias content: scrollable.content

    implicitWidth: StackView.view ? StackView.view.width : 0
    implicitHeight: StackView.view ? StackView.view.height : 0

    required property string title
    property string path: ""

    SSCrollable {
        id: scrollable

        anchors.fill: parent

        desiredHeight: root.implicitHeight
    }
}
