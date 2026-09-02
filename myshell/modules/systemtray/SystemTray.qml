pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import org.nightshell.SystemTray
import org.nightshell.DBusMenu
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
	id: root

	readonly property bool hasItems: SystemTrayQML.items.values.length > 0
	readonly property int padding: 4
	property StatusNotifierItem current: null
	property SystemTrayItem currentActive: null

	function evalCurrentActive(): void {
		for (let i = 0; i < systemTrayRepeater.count; i += 1) {
			const it = systemTrayRepeater.itemAt(i);
			if (it) {
				const castItem = it as SystemTrayItem;
				if (castItem && castItem.modelData === root.current) {
					root.currentActive = castItem;
					return;
				}
			}
		}

		root.currentActive = null;
	}

	onCurrentChanged: evalCurrentActive()

	function toggleItem(item: StatusNotifierItem): void {
		if (root.current == item)
			root.current = null;
		else
			root.current = item;
	}

	Layout.fillHeight: true

	implicitWidth: mainLayout.width + (hasItems ? padding * 2 : 0)
	visible: width > 0

	Rectangle {
		anchors.fill: parent
		color: Colors.colors.base0
		radius: Config.appearance.rounding.sm
	}

	RowLayout {
		id: mainLayout

		anchors {
			top: parent.top
			bottom: parent.bottom
			horizontalCenter: parent.horizontalCenter
		}

		Repeater {
			id: systemTrayRepeater
			model: SystemTrayQML.items
			delegate: SystemTrayItem {
				id: sysTrayItem
				isActive: root.current === modelData

				Layout.topMargin: root.padding
				Layout.bottomMargin: root.padding
				Layout.fillHeight: true

				onClicked: {
					root.toggleItem(modelData);
				}
			}

			onItemAdded: root.evalCurrentActive()
			onItemRemoved: root.evalCurrentActive()
		}
	}

	Item {
		id: popoutMenu

		anchors {
			top: parent.bottom
		}

		Connections {
			target: root

			function onCurrentActiveChanged() {
				if (!root.currentActive) {
					return;
				}
				const mapPos = root.currentActive.mapToItem(root, Qt.point(root.currentActive.width / 2, 0));
				popoutMenu.x = mapPos.x;
			}
		}

		Item {
			id: popoutMenuContent

			anchors {
				top: parent.bottom
				horizontalCenter: parent.horizontalCenter
			}

			implicitWidth: popoutCurrent.implicitWidth
			implicitHeight: root.currentActive ? popoutCurrent.implicitHeight : 0

			Rectangle {
				anchors.fill: parent
				color: Colors.colors.base0
			}

			StackView {
				id: popoutCurrent

				anchors {
					top: parent.top
					horizontalCenter: parent.horizontalCenter
				}

				implicitWidth: popoutCurrent.currentItem ? popoutCurrent.currentItem.implicitWidth : 0
				implicitHeight: popoutCurrent.currentItem ? popoutCurrent.currentItem.implicitHeight : 0

				Connections {
					target: root

					function onCurrentChanged() {
						if (root.current) {
							popoutCurrent.replace(traySubmenuComponent, {
								statusItem: root.current,
								parentId: 0
							});
						} else {
							popoutCurrent.clear(StackView.PopTransition);
						}
					}
				}
			}
		}
	}

	Component {
		id: traySubmenuComponent

		Item {
			id: traySubmenuItem
			required property StatusNotifierItem statusItem
			required property int parentId

			implicitWidth: submenuLayout.implicitWidth
			implicitHeight: submenuLayout.implicitHeight

			DBusMenuOpener {
				id: menuOpener
				menu: traySubmenuItem.statusItem.menuHandle
				parent: traySubmenuItem.parentId
			}

			ColumnLayout {
				id: submenuLayout

				anchors {
					top: parent.top
					horizontalCenter: parent.horizontalCenter
				}

				Repeater {
					model: menuOpener.item.children
					delegate: Text {
						required property DBusMenuItem modelData

						text: modelData.label
					}
				}
			}
		}
	}
}
