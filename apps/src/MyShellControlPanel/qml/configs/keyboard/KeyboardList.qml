pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.components
import MyShellControlPanel.singletons
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    readonly property int padding: Config.appearance.padding.sm
    property int expandedId: -1

    function toggleExpandById(id: int) {
        if (id < 0 || id > Hypr.allLayouts.length) {
            expandedId = -1;
            return;
        }
        if (expandedId === id)
            expandedId = -1;
        else
            expandedId = id;
    }

    Layout.fillWidth: true
    Layout.fillHeight: true

    SectionBg {
        anchors.margins: root.padding
    }

    ListView {
        id: listView

        readonly property bool isScrollBarActive: contentHeight > height
        readonly property int scrollBarWidth: 12

        anchors.fill: parent
        anchors.margins: root.padding * 2
        model: Hypr.allLayouts

        boundsBehavior: Flickable.StopAtBounds
        flickDeceleration: 5000

        ScrollBar.vertical: ScrollBar {
            policy: listView.isScrollBarActive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff

            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            padding: 0

            implicitWidth: listView.scrollBarWidth

            contentItem: Rectangle {
                color: Colors.colors.primary
                radius: width / 2
            }

            background: Rectangle {
                color: Colors.colors.base2
                radius: width / 2
            }
        }

        clip: true

        delegate: Item {
            id: kbd
            required property KKeyboardLayout modelData
            required property int index
            property bool isInstalled: evalIsInstalled()

            readonly property bool hasVariants: modelData && modelData.variants.length > 0
            readonly property bool isExpanded: root.expandedId === index

            implicitWidth: ListView.view ? ListView.view.width - listView.scrollBarWidth - root.padding : 0
            implicitHeight: kbdInfo.implicitHeight

            function evalIsInstalled(): bool {
                if (!modelData) {
                    isInstalled = false;
                    return;
                }
                isInstalled = Hypr.inputLayouts.some(layout => modelData.name === layout.layout);
            }

            onModelDataChanged: evalIsInstalled()

            Component.onDestruction: {
                if (root.expandedId === kbd.index) {
                    root.expandedId = -1;
                }
            }

            Connections {
                target: Hypr

                function onInputLayoutsChanged() {
                    kbd.evalIsInstalled();
                }
            }

            Rectangle {
                opacity: kbd.isInstalled ? 1 : 0
                anchors.fill: parent

                color: Colors.colors.primary5
                radius: Config.appearance.rounding.sm
            }

            ColumnLayout {
                id: kbdInfo

                readonly property color textCol: kbd.isInstalled ? Colors.colors.base0 : Colors.colors.baseContent

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter

                spacing: 0

                Item {
                    id: kbdInfoTitle

                    Layout.fillWidth: true
                    implicitHeight: kbdInfoTitleText.implicitHeight

                    MouseArea {
                        enabled: kbd.hasVariants

                        anchors.fill: parent

                        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

                        onClicked: {
                            root.toggleExpandById(kbd.index);
                        }
                    }

                    StyledText {
                        id: kbdInfoTitleExpand

                        property real rot: kbd.isExpanded ? 90 : 0

                        visible: kbd.hasVariants
                        anchors.left: parent.left
                        anchors.leftMargin: root.padding
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom

                        text: "󰅂"
                        color: kbdInfo.textCol

                        verticalAlignment: Text.AlignVCenter

                        transform: Rotation {
                            origin.x: kbdInfoTitleExpand.width / 2
                            origin.y: kbdInfoTitleExpand.height / 2
                            angle: kbdInfoTitleExpand.rot
                        }

                        font.family: Config.appearance.fontFamily.mono
                        font.pointSize: Config.appearance.fontSize.xl

                        Behavior on rot {
                            NAnim {}
                        }
                    }

                    StyledText {
                        id: kbdInfoTitleText

                        text: kbd.modelData ? `[${kbd.modelData.name}] ${kbd.modelData.description}` : ""
                        color: kbdInfo.textCol

                        font.pointSize: Config.appearance.fontSize.xs

                        anchors.left: kbdInfoTitleExpand.right
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter

                        elide: Text.ElideRight

                        padding: root.padding
                    }
                }

                Loader {
                    id: variantsWrapper

                    active: kbd.hasVariants

                    Layout.fillWidth: true

                    sourceComponent: Item {
                        readonly property real fullHeight: variantsLayout.implicitHeight + root.padding * 3

                        anchors.left: parent.left
                        anchors.right: parent.right

                        anchors.leftMargin: root.padding
                        anchors.rightMargin: root.padding

                        implicitHeight: kbd.isExpanded ? fullHeight : 0

                        clip: true
                        visible: implicitHeight > 0

                        Rectangle {
                            anchors.fill: parent
                            anchors.bottomMargin: root.padding
                            color: Colors.colors.base0
                            radius: Config.appearance.rounding.sm
                        }

                        ColumnLayout {
                            id: variantsLayout
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.margins: root.padding

                            Repeater {
                                model: kbd.modelData.variants

                                delegate: StyledText {
                                    required property KKeyboardVariant modelData

                                    Layout.fillWidth: true

                                    text: modelData ? `[${modelData.name}] ${modelData.description}` : ""
                                    elide: Text.ElideRight
                                }
                            }
                        }

                        Behavior on implicitHeight {
                            NAnim {}
                        }
                    }
                }
            }
        }
    }
}
