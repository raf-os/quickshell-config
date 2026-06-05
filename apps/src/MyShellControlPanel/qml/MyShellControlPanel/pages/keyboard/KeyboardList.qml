pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.plugin
import MyShellControlPanel.components
import MyShellControlPanel.singletons
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    required property KeyboardSettingsBuffer settingsBuffer
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

    SListView {
        id: listView

        anchors.fill: parent
        anchors.margins: root.padding * 2
        model: Hypr.allLayouts

        delegate: Item {
            id: kbd

            required property KKeyboardLayout modelData
            required property int index
            readonly property bool isInstalled: {
                if (!modelData)
                    return false;
                root.settingsBuffer.layouts;
                return root.settingsBuffer.layouts.some(layout => modelData.name === layout.layout);
            }
            readonly property bool hasVariants: modelData && modelData.variants && modelData.variants.length > 0
            readonly property bool isExpanded: root.expandedId === index

            implicitWidth: ListView.view ? ListView.view.width : 0
            implicitHeight: kbdInfo.implicitHeight

            Component.onDestruction: {
                if (root.expandedId === kbd.index) {
                    root.expandedId = -1;
                }
            }

            Rectangle {
                opacity: kbd.isInstalled || kbd.isExpanded ? 1 : 0
                visible: opacity > 0

                anchors.fill: parent
                color: kbd.isInstalled ? Colors.colors.primary : Colors.colors.base2
                radius: Config.appearance.rounding.sm

                Behavior on opacity {
                    NAnim {
                        duration: 300
                    }
                }

                Behavior on color {
                    CAnim {
                        duration: 200
                    }
                }
            }

            ColumnLayout {
                id: kbdInfo

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter

                spacing: root.padding / 2

                MouseArea {
                    id: kbdInfoTitle

                    Layout.fillWidth: true

                    implicitHeight: kbdInfoTitleText.height
                    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

                    enabled: root.enabled && kbd.hasVariants

                    onClicked: {
                        root.toggleExpandById(kbd.index);
                    }

                    SMaterialIcon {
                        id: kbdInfoTitleExpand

                        property real rot: kbd.isExpanded ? 90 : 0

                        visible: kbd.hasVariants

                        text: "chevron_forward"
                        font.pointSize: Config.appearance.fontSize.xl

                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom

                        verticalAlignment: Text.AlignVCenter

                        transform: Rotation {
                            origin.x: kbdInfoTitleExpand.width / 2
                            origin.y: kbdInfoTitleExpand.height / 2
                            angle: kbdInfoTitleExpand.rot
                        }

                        Behavior on rot {
                            NAnim {
                                duration: 100
                            }
                        }
                    }

                    StyledText {
                        id: kbdInfoTitleText
                        text: kbd.modelData ? `[${kbd.modelData?.name ?? ""}] ${kbd.modelData?.description ?? ""}` : ""

                        font.family: Config.appearance.fontFamily.sans
                        font.pointSize: Config.appearance.fontSize.sm
                        font.weight: kbd.isInstalled ? 700 : 500

                        anchors.left: kbdInfoTitleExpand.right
                        anchors.right: kbdInfoActionButton.left
                        anchors.verticalCenter: parent.verticalCenter

                        padding: 4
                    }

                    MouseArea {
                        id: kbdInfoActionButton

                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        anchors.rightMargin: root.padding

                        implicitWidth: kbdInfoActionIcon.width

                        enabled: root.enabled
                        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

                        onClicked: {
                            if (!kbd.modelData)
                                return;

                            if (kbd.isInstalled)
                                root.settingsBuffer.removeLayoutAtIndex(kbd.index);
                            else
                                root.settingsBuffer.addLayout(kbd.modelData.name, "");
                        }

                        SMaterialIcon {
                            id: kbdInfoActionIcon

                            anchors.centerIn: parent
                            text: kbd.isInstalled ? "remove" : "add"

                            font.pixelSize: Config.appearance.fontSize.xl
                            font.weight: 700

                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }

                Loader {
                    id: variantsWrapper

                    active: kbd.hasVariants
                    Layout.fillWidth: true

                    sourceComponent: Item {
                        id: variantKbdItem
                        readonly property int padding: Config.appearance.padding.xxs
                        readonly property real fullHeight: variantsLayout.implicitHeight + padding * 3

                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top

                        anchors.leftMargin: padding
                        anchors.rightMargin: padding
                        anchors.bottomMargin: padding

                        implicitHeight: kbd.isExpanded ? fullHeight : 0

                        clip: true
                        visible: implicitHeight > 0

                        Rectangle {
                            anchors.fill: parent
                            anchors.bottomMargin: variantKbdItem.padding
                            color: Colors.colors.base0
                            radius: Config.appearance.rounding.sm
                        }

                        ColumnLayout {
                            id: variantsLayout

                            anchors.top: parent.top
                            anchors.topMargin: variantKbdItem.padding
                            anchors.bottomMargin: variantKbdItem.padding
                            anchors.left: parent.left
                            anchors.right: parent.right

                            spacing: 0

                            Repeater {
                                model: kbd.modelData.variants

                                delegate: Item {
                                    id: variantItem
                                    required property KKeyboardVariant modelData
                                    Layout.fillWidth: true

                                    implicitHeight: variantItemTitle.height

                                    StyledText {
                                        id: variantItemTitle

                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        anchors.verticalCenter: parent.verticalCenter

                                        text: variantItem.modelData ? `[${variantItem.modelData.name ?? ""}]${variantItem.modelData.description ?? ""}` : ""
                                        elide: Text.ElideRight

                                        font.family: Config.appearance.fontFamily.sans
                                        font.pointSize: Config.appearance.fontSize.sm

                                        verticalAlignment: Text.AlignVCenter

                                        leftPadding: Config.appearance.padding.md
                                        rightPadding: leftPadding

                                        topPadding: Config.appearance.padding.sm
                                        bottomPadding: topPadding
                                    }
                                }
                            }
                        }

                        Behavior on implicitHeight {
                            NAnim {
                                duration: 300
                            }
                        }
                    }
                }
            }
        }

        // delegate: Item {
        //     id: kbd
        //     required property KKeyboardLayout modelData
        //     required property int index
        //     property bool isInstalled: evalIsInstalled()
        //
        //     readonly property bool hasVariants: modelData && modelData.variants.length > 0
        //     readonly property bool isExpanded: root.expandedId === index
        //
        //     implicitWidth: ListView.view ? ListView.view.width : 0
        //     implicitHeight: kbdInfo.implicitHeight
        //
        //     function evalIsInstalled(): bool {
        //         if (!modelData) {
        //             isInstalled = false;
        //             return;
        //         }
        //         isInstalled = root.settingsBuffer.layouts.some(layout => modelData.name === layout.layout);
        //     }
        //
        //     onModelDataChanged: evalIsInstalled()
        //
        //     Component.onDestruction: {
        //         if (root.expandedId === kbd.index) {
        //             root.expandedId = -1;
        //         }
        //     }
        //
        //     Connections {
        //         target: Hypr
        //
        //         function onInputLayoutsChanged() {
        //             kbd.evalIsInstalled();
        //         }
        //     }
        //
        //     Rectangle {
        //         opacity: kbd.isInstalled ? 1 : 0
        //         anchors.fill: parent
        //
        //         color: Colors.colors.primary2
        //         radius: Config.appearance.rounding.sm
        //     }
        //
        //     ColumnLayout {
        //         id: kbdInfo
        //
        //         readonly property color textCol: kbd.isInstalled ? Colors.colors.base0 : Colors.colors.baseContent
        //
        //         anchors.left: parent.left
        //         anchors.right: parent.right
        //         anchors.verticalCenter: parent.verticalCenter
        //
        //         spacing: 0
        //
        //         MouseArea {
        //             id: kbdInfoTitle
        //
        //             Layout.fillWidth: true
        //             implicitHeight: kbdInfoTitleText.implicitHeight
        //             enabled: kbd.hasVariants
        //             cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
        //
        //             onClicked: {
        //                 root.toggleExpandById(kbd.index);
        //             }
        //
        //             StyledText {
        //                 id: kbdInfoTitleExpand
        //
        //                 property real rot: kbd.isExpanded ? 90 : 0
        //
        //                 visible: kbd.hasVariants
        //                 anchors.left: parent.left
        //                 anchors.leftMargin: root.padding
        //                 anchors.top: parent.top
        //                 anchors.bottom: parent.bottom
        //
        //                 text: "󰅂"
        //                 color: kbdInfo.textCol
        //
        //                 verticalAlignment: Text.AlignVCenter
        //
        //                 transform: Rotation {
        //                     origin.x: kbdInfoTitleExpand.width / 2
        //                     origin.y: kbdInfoTitleExpand.height / 2
        //                     angle: kbdInfoTitleExpand.rot
        //                 }
        //
        //                 font.family: Config.appearance.fontFamily.mono
        //                 font.pointSize: Config.appearance.fontSize.xl
        //
        //                 Behavior on rot {
        //                     NAnim {}
        //                 }
        //             }
        //
        //             StyledText {
        //                 id: kbdInfoTitleText
        //
        //                 text: kbd.modelData ? `[${kbd.modelData.name}] ${kbd.modelData.description}` : ""
        //                 color: kbdInfo.textCol
        //
        //                 font.pointSize: Config.appearance.fontSize.xs
        //
        //                 anchors.left: kbdInfoTitleExpand.right
        //                 anchors.right: parent.right
        //                 anchors.verticalCenter: parent.verticalCenter
        //
        //                 elide: Text.ElideRight
        //
        //                 padding: root.padding
        //             }
        //
        //             MouseArea {
        //                 id: addRemoveArea
        //
        //                 anchors.right: parent.right
        //                 anchors.top: parent.top
        //                 anchors.bottom: parent.bottom
        //
        //                 implicitWidth: height
        //
        //                 StyledText {
        //                     id: addRemoveText
        //
        //                     anchors.centerIn: parent
        //
        //                     text: kbd.isInstalled ? "󰍴" : "󰐕"
        //                     color: kbdInfo.textCol
        //                     font.pointSize: Config.appearance.fontSize.md
        //
        //                     verticalAlignment: Text.AlignVCenter
        //                     horizontalAlignment: Text.AlignLeft
        //                 }
        //             }
        //         }
        //
        //         Loader {
        //             id: variantsWrapper
        //
        //             active: kbd.hasVariants
        //
        //             Layout.fillWidth: true
        //
        //             sourceComponent: Item {
        //                 readonly property real fullHeight: variantsLayout.implicitHeight + root.padding * 3
        //
        //                 anchors.left: parent.left
        //                 anchors.right: parent.right
        //
        //                 anchors.leftMargin: root.padding
        //                 anchors.rightMargin: root.padding
        //
        //                 implicitHeight: kbd.isExpanded ? fullHeight : 0
        //
        //                 clip: true
        //                 visible: implicitHeight > 0
        //
        //                 Rectangle {
        //                     anchors.fill: parent
        //                     anchors.bottomMargin: root.padding
        //                     color: Colors.colors.base0
        //                     radius: Config.appearance.rounding.sm
        //                 }
        //
        //                 ColumnLayout {
        //                     id: variantsLayout
        //                     anchors.top: parent.top
        //                     anchors.left: parent.left
        //                     anchors.right: parent.right
        //                     anchors.margins: root.padding
        //
        //                     spacing: Config.appearance.spacing.sm
        //
        //                     Repeater {
        //                         model: kbd.modelData.variants
        //
        //                         delegate: StyledText {
        //                             required property KKeyboardVariant modelData
        //
        //                             Layout.fillWidth: true
        //
        //                             text: modelData ? `[${modelData.name}] ${modelData.description}` : ""
        //                             elide: Text.ElideRight
        //                         }
        //                     }
        //                 }
        //
        //                 Behavior on implicitHeight {
        //                     NAnim {}
        //                 }
        //             }
        //         }
        //     }
        // }
    }
}
