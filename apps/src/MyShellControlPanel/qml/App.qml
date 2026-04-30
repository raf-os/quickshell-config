import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.components
import QtQuick
import QtQuick.Controls

FocusScope {
    id: root
    anchors.fill: parent
    objectName: "App"

    focus: true

    property alias stackInterface: stackInterface
    property list<string> titleList: []

    Keys.priority: Keys.AfterItem

    Keys.onEscapePressed: event => {
        focusSink.forceActiveFocus();
        event.accepted = true;
    }

    QtObject {
        id: stackInterface

        readonly property int depth: pageStack.depth
        property string currentPath: "/keyboard"

        function clearAndPush(path: string): void {
            pageStack.clear();
            navigateTo(path);
        }

        function navigateTo(path: string): void {
            const cleanPath = path[0] === "/" ? path.slice(1) : path;
            pageStack.push(Qt.resolvedUrl(`configs/${cleanPath}/Index.qml`));
            currentPath = "/" + cleanPath;
        }

        function navigateBack(): void {
            if (!pageStack.depth > 1)
                pageStack.popCurrentItem();
        }
    }

    FocusScope {
        id: sbScope

        activeFocusOnTab: true

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

                color: Colors.colors.base
            }

            StyledText {
                id: titleBreadcrumbs

                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: closeButton.left

                anchors.leftMargin: Config.appearance.padding.md

                text: "Index"

                verticalAlignment: Text.AlignVCenter
            }

            Item {
                id: closeButton

                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                implicitWidth: parent.implicitHeight

                MouseArea {
                    id: closeButtonMouseArea
                    anchors.centerIn: parent

                    implicitWidth: 24
                    implicitHeight: implicitWidth

                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        Qt.quit();
                    }

                    Rectangle {
                        anchors.fill: parent

                        radius: parent.implicitWidth / 2

                        color: Colors.colors.base0

                        border.width: 1
                        border.color: closeButtonMouseArea.containsMouse ? Colors.colors.destructive : "transparent"

                        Behavior on border.color {
                            CAnim {
                                duration: 200
                            }
                        }
                    }

                    StyledText {
                        id: closeButtonIcon
                        anchors.fill: parent
                        anchors.margins: 2

                        text: ""
                        font.family: Config.appearance.fontFamily.mono
                        font.pixelSize: height

                        color: closeButtonMouseArea.containsMouse ? Colors.colors.destructiveHover : Colors.colors.destructive

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter

                        Behavior on color {
                            CAnim {
                                duration: 200
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            id: stackBg

            anchors.left: parent.left
            anchors.top: titleContainer.bottom
            anchors.bottom: parent.bottom
            anchors.right: parent.right

            color: Colors.colors.base0

            topLeftRadius: Config.appearance.rounding.sm
        }

        PageStack {
            id: pageStack

            anchors.fill: stackBg
            anchors.margins: Config.appearance.padding.md
        }
    }

    FocusScope {
        id: focusSink
        activeFocusOnTab: true
    }
}
