pragma ComponentBehavior: Bound

import qs.components
import qs.services
import qs.config
import Quickshell
import Quickshell.Widgets
import QtQuick
import QtQuick.Controls

StackView {
    id: root

    required property Item popouts
    required property QsMenuHandle trayItem

    implicitWidth: currentItem.implicitWidth
    implicitHeight: currentItem.implicitHeight

    initialItem: SubMenu {
        handle: root.trayItem
    }

    pushEnter: NoAnim {}
    pushExit: NoAnim {}
    popEnter: NoAnim {}
    popExit: NoAnim {}

    component NoAnim: Transition {
        NAnim {
            duration: 0
        }
    }

    component SubMenu: Column {
        id: menu

        required property QsMenuHandle handle
        property bool isSubMenu
        property bool shown

        property var modelBuffer: []

        padding: Config.appearance.padding.xs
        spacing: Config.appearance.spacing.sm

        opacity: shown ? 1 : 0

        Component.onCompleted: shown = true
        StackView.onActivating: shown = true
        StackView.onDeactivating: shown = false
        StackView.onRemoved: destroy()

        Behavior on opacity {
            NAnim {}
        }

        Behavior on scale {
            NAnim {}
        }

        QsMenuOpener {
            id: menuOpener

            menu: menu.handle
        }

        Repeater {
            model: menuOpener.children

            StyledRect {
                id: item

                required property QsMenuEntry modelData

                implicitWidth: Config.bar.sizes.trayMenuWidth
                implicitHeight: modelData?.isSeparator ? 1 : children.implicitHeight

                radius: Config.appearance.rounding.sm
                color: modelData?.isSeparator ? ColorService.current.base3 : "transparent"

                Loader {
                    id: children

                    anchors.left: parent.left
                    anchors.right: parent.right

                    active: !(item.modelData?.isSeparator ?? false)

                    sourceComponent: Item {
                        implicitHeight: label.implicitHeight

                        ButtonStateLayer {
                            anchors.margins: -Config.appearance.padding.sm / 2
                            anchors.leftMargin: -Config.appearance.padding.xs
                            anchors.rightMargin: -Config.appearance.padding.xs

                            color: ColorService.current.base4

                            radius: item.radius
                            disabled: !(item.modelData?.enabled ?? false)

                            function onClicked(): void {
                                const entry = item.modelData;
                                if (entry.hasChildren) {
                                    root.push(subMenuComp.createObject(null, {
                                        handle: entry,
                                        isSubMenu: true
                                    }));
                                } else {
                                    item.modelData?.triggered();
                                    root.popouts.close();
                                }
                            }
                        }
                    }

                    Loader {
                        id: icon

                        anchors.left: parent.left

                        active: (item.modelData?.icon !== "") ?? false

                        sourceComponent: IconImage {
                            implicitSize: label.implicitHeight
                            source: item.modelData?.icon ?? ""
                        }
                    }

                    StyledText {
                        id: label

                        anchors.left: icon.right
                        anchors.leftMargin: icon.active ? Config.appearance.spacing.xs : 0

                        text: labelMetrics.elidedText
                        color: item.modelData?.enabled ? ColorService.current.baseContent : ColorService.current.base3
                    }

                    TextMetrics {
                        id: labelMetrics

                        text: item.modelData?.text ?? ""
                        font.pointSize: label.font.pointSize
                        font.family: label.font.family

                        elide: Text.ElideRight
                        elideWidth: Config.bar.sizes.trayMenuWidth - (icon.active ? icon.implicitWidth + label.anchors.leftMargin : 0) - (expand.active ? expand.implicitWidth + Config.appearance.spacing.md : 0)
                    }

                    Loader {
                        id: expand

                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right

                        active: item.modelData?.hasChildren ?? false

                        sourceComponent: MaterialIcon {
                            text: "chevron_right"
                            color: item.modelData?.enabled ? ColorService.current.baseContent : "gray"
                        }
                    }
                }
            }
        }

        Loader {
            active: menu.isSubMenu

            sourceComponent: Item {
                implicitWidth: back.implicitWidth
                implicitHeight: back.implicitHeight + Config.appearance.spacing.sm / 2

                Item {
                    anchors.bottom: parent.bottom
                    implicitWidth: back.implicitWidth
                    implicitHeight: back.implicitHeight

                    StyledRect {
                        anchors.fill: parent
                        anchors.margins: -Config.appearance.padding.sm / 2
                        anchors.leftMargin: -Config.appearance.padding.xs
                        anchors.rightMargin: -Config.appearance.padding.xs * 2

                        radius: Config.appearance.rounding.full
                        color: ColorService.current.base2

                        ButtonStateLayer {
                            radius: parent.radius

                            function onClicked(): void {
                                root.pop();
                            }
                        }
                    }

                    Row {
                        id: back

                        anchors.verticalCenter: parent.verticalCenter

                        MaterialIcon {
                            anchors.verticalCenter: parent.verticalCenter
                            text: "chevron_left"
                            color: ColorService.current.baseContent
                        }

                        StyledText {
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("Back")
                            color: ColorService.current.baseContent
                        }
                    }
                }
            }
        }
    }

    Component {
        id: subMenuComp
        SubMenu {}
    }
}
