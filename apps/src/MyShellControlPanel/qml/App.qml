import MyShellPlugin
import QtQuick
import QtQuick.Controls

Item {
    id: root
    anchors.fill: parent
    objectName: "App"

    property alias stackInterface: stackInterface
    property list<string> titleList: []

    QtObject {
        id: stackInterface

        readonly property int depth: pageStack.depth

        function clearAndPush(path: string): void {
            pageStack.clear();
            navigateTo(path);
        }

        function navigateTo(path: string): void {
            pageStack.push(Qt.resolvedUrl(`configs/${path}/Index.qml`));
        }

        function navigateBack(): void {
            if (!pageStack.depth > 1)
                pageStack.popCurrentItem();
        }
    }

    FocusScope {
        id: sbScope

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        implicitWidth: sidebar.implicitWidth

        Sidebar {
            id: sidebar

            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            app: root
        }
    }

    Item {
        id: mainView

        anchors.left: sbScope.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        Item {
            id: titleContainer

            implicitHeight: 32

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            Rectangle {
                anchors.fill: parent

                color: Colors.colors.base2
            }
        }

        PageStack {
            id: pageStack

            anchors.left: parent.left
            anchors.top: titleContainer.bottom
            anchors.bottom: parent.bottom
            anchors.right: parent.right
        }
    }
}
