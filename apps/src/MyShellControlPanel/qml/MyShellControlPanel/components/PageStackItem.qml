import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Controls

FocusScope {
    id: root

    default property alias content: container.data
    property Item sizeReference: container

    readonly property int scrollBarWidth: 8

    implicitWidth: StackView.view ? StackView.view.width : 0
    implicitHeight: StackView.view ? StackView.view.height : 0

    required property string title
    property string path: ""

    activeFocusOnTab: true

    Flickable {
        id: flickable

        // WARNING: Weird behavior - this could break in the future
        contentHeight: Math.max(root.sizeReference.height, root.implicitHeight)
        readonly property bool isScrollBarActive: contentHeight > height

        anchors.fill: parent

        focus: true
        clip: true

        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds

        ScrollBar.vertical: ScrollBar {
            policy: flickable.isScrollBarActive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff

            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            padding: 0

            implicitWidth: root.scrollBarWidth

            contentItem: Rectangle {
                color: Colors.colors.primary
                radius: width / 2
            }

            background: Rectangle {
                color: Colors.colors.base2
                radius: width / 2
            }
        }

        Item {
            id: container

            anchors.fill: parent
            anchors.rightMargin: flickable.isScrollBarActive ? root.scrollBarWidth + Config.appearance.spacing.sm : 0

            Behavior on anchors.rightMargin {
                NAnim {}
            }
        }
    }
}
