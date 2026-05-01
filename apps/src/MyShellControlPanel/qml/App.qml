import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.components
import QtQuick
import QtQuick.Layouts

FocusScope {
    id: root
    anchors.fill: parent
    objectName: "App"

    focus: true

    property alias stackInterface: stackInterface
    property list<string> titleList: []

    Keys.priority: Keys.AfterItem

    Keys.onEscapePressed: event => {
        resetAppFocus();
        event.accepted = true;
    }

    function resetAppFocus() {
        focusSink.forceActiveFocus();
    }

    QtObject {
        id: stackInterface

        readonly property int depth: pageStack.depth
        readonly property SidebarModel sidebarModel: SidebarModel {}
        property string currentPath: "/"
        property list<string> titleList: []
        property list<string> pathList: []
        readonly property string titleSegments: titleList.join("|")

        function cleanPathString(path: string): string {
            if (path.length < 2)
                return "";

            const cleanPath = path[0] === "/" ? path.slice(1) : path;
            return cleanPath;
        }

        function clearAndPush(path: string): void {
            if (path === currentPath)
                return;
            // pageStack.clear();
            navigateTo(path);
        }

        function navigateTo(path: string): void {
            if (path === currentPath) {
                return;
            }

            const existingItem = pageStack.find(item => {
                return item.path === path;
            });

            if (existingItem) {
                pageStack.popToItem(existingItem);
                currentPath = existingItem.path;
                modelPathIterate(existingItem.path);
                return;
            }

            const cleanPath = cleanPathString(path);
            pageStack.pushItem(Qt.resolvedUrl(`configs/${cleanPath}/Index.qml`), {
                "path": path
            });
            currentPath = "/" + cleanPath;
            modelPathIterate(path);
        }

        function navigateBack(): void {
            if (pageStack.depth > 1) {
                pageStack.popCurrentItem();
                titleList.pop();
            }
        }

        function modelPathIterate(path: string): void {
            const cleanPath = cleanPathString(path);
            const pathSegments = cleanPath.split("/");
            const listBuf = [];
            const pListBuf = [];
            pathSegments.reduce((acc, cur, idx) => {
                acc += `/${cur}`;

                for (let i = 0; i < sidebarModel.count; i += 1) {
                    const m = sidebarModel.get(i);
                    if (m.path === acc) {
                        if (m.label && m.label != "") {
                            listBuf.push(m.label);
                            pListBuf.push(m.path);
                        }

                        break;
                    }
                }
            }, "");
            titleList = listBuf;
            pathList = pListBuf;
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

            RowLayout {
                id: titleBreadcrumbs

                readonly property string fontSize: Config.appearance.fontSize.xs

                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: closeButton.left

                anchors.leftMargin: Config.appearance.padding.md

                MouseArea {
                    id: indexBreadcrumb

                    readonly property bool isActive: stackInterface.titleList.length > 0

                    Layout.fillHeight: true
                    implicitWidth: indexBreadcrumbText.width

                    enabled: isActive
                    cursorShape: isActive ? Qt.PointingHandCursor : Qt.ArrowCursor

                    onClicked: {
                        stackInterface.navigateTo("/");
                    }

                    StyledText {
                        id: indexBreadcrumbText

                        anchors.top: parent.top
                        anchors.bottom: parent.bottom

                        text: "Index"
                        font.pointSize: titleBreadcrumbs.fontSize
                        font.weight: 700

                        verticalAlignment: Text.AlignVCenter

                        color: indexBreadcrumb.isActive ? Colors.colors.primary : Colors.colors.baseContent
                    }
                }

                Repeater {
                    id: breadcrumbs

                    model: stackInterface.titleSegments.length > 0 ? stackInterface.titleSegments.split("|") : 0

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    delegate: RowLayout {
                        id: bcrumb
                        required property string modelData
                        required property int index

                        StyledText {
                            font.family: Config.appearance.fontFamily.mono
                            text: "󰅂"

                            font.pointSize: titleBreadcrumbs.fontSize
                        }

                        MouseArea {
                            readonly property bool isActive: bcrumb.index + 1 < breadcrumbs.model.length

                            Layout.fillHeight: true
                            implicitWidth: bcrumbTitle.width

                            hoverEnabled: true
                            cursorShape: isActive ? Qt.PointingHandCursor : Qt.ArrowCursor

                            enabled: isActive

                            onClicked: {
                                stackInterface.navigateTo(bcrumb.modelData);
                            }

                            StyledText {
                                id: bcrumbTitle

                                anchors.top: parent.top
                                anchors.bottom: parent.bottom

                                text: bcrumb.modelData
                                font.pointSize: titleBreadcrumbs.fontSize
                                font.weight: 600

                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
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
