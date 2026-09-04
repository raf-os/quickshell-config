pragma ComponentBehavior: Bound

import qs.components
import MyShellPlugin
import MyShellPlugin.Configs
import org.nightshell.DBusMenu
import org.nightshell.SystemTray
import QtQuick

Item {
	id: root

	required property StatusNotifierItem statusItem

	implicitHeight: Math.min(lview.contentHeight, 512)

	ScopedDBusMenuView {
		id: scope
		model: root.statusItem.menuHandle
		modelIndex: root.statusItem.menuHandle.rootIndex
	}

	DelegateModel {
		id: itemModel
		model: root.statusItem.menuHandle
		rootIndex: root.statusItem.menuHandle.rootIndex

		delegate: Item {
			id: itemDelegate
			required property int index
			required property var selfIndex
			required property DBusMenuModelItem modelData

			implicitWidth: ListView.view ? ListView.view.width : 0
			implicitHeight: childrenRect.height

			Loader {
				active: !itemDelegate.modelData.isSeparator
				anchors {
					left: parent.left
					right: parent.right
				}
				sourceComponent: StyledText {
					anchors {
						left: parent.left
						right: parent.right
					}

					text: itemDelegate.modelData.text
					elide: Text.ElideRight

					font.pointSize: Config.appearance.fontSize.sm
				}
			}

			Loader {
				active: itemDelegate.modelData.isSeparator
				anchors {
					left: parent.left
					right: parent.right
				}
				sourceComponent: Text {
					text: "me separate"
				}
			}
		}
	}

	ListView {
		id: lview

		anchors.fill: parent

		model: itemModel
	}
}
