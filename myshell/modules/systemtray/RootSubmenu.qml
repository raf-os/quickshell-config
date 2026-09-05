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
	required property var rootIndex
	required property int depth
	readonly property int padding: Config.appearance.padding.sm

	property bool isLoading: false

	implicitHeight: Math.min(lview.contentHeight + returnComponentLoader.height + root.padding * 2, 1024)

	signal requestClose
	signal navigateToIndex(index: var, statusItem: StatusNotifierItem)
	signal navigateBackwards

	ScopedDBusMenuView {
		id: scope
		model: root.statusItem.menuHandle
		modelIndex: root.statusItem.menuHandle.rootIndex
	}

	DelegateModel {
		id: itemModel
		model: root.statusItem.menuHandle
		rootIndex: root.rootIndex

		onCountChanged: {
			if (count == 0) {
				root.navigateBackwards();
			}
		}

		delegate: Item {
			id: itemDelegate
			required property int index
			required property var selfIndex
			required property DBusMenuModelItem modelData

			readonly property int spacing: Config.appearance.spacing.xs

			implicitWidth: ListView.view ? ListView.view.width : 0
			implicitHeight: childrenRect.height

			visible: modelData.isVisible

			Loader {
				active: !itemDelegate.modelData.isSeparator
				anchors {
					left: parent.left
					right: parent.right
				}
				sourceComponent: MouseArea {
					implicitHeight: labelText.height

					enabled: itemDelegate.modelData.isEnabled
					cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

					onClicked: {
						if (root.isLoading)
							return;

						if (itemDelegate.modelData.hasChildren) {
							root.navigateToIndex(itemDelegate.selfIndex, root.statusItem);
						} else {
							itemDelegate.modelData.trigger();
							root.requestClose();
						}
					}

					Loader {
						id: itemIconLoader
						active: itemDelegate.modelData.iconUrl !== ""
						anchors {
							left: parent.left
							top: parent.top
							bottom: parent.bottom
							margins: 1
						}
						sourceComponent: Item {
							implicitWidth: height

							Image {
								asynchronous: true
								source: itemDelegate.modelData.iconUrl
								width: parent.width
								height: parent.height
							}
						}
					}

					StyledText {
						id: labelText
						anchors {
							left: itemIconLoader.right
							leftMargin: itemIconLoader.active ? itemDelegate.spacing : 0
							right: expandIconLoader.left
							rightMargin: itemDelegate.spacing
							verticalCenter: parent.verticalCenter
						}

						text: itemDelegate.modelData.text
						elide: Text.ElideRight

						font.pointSize: Config.appearance.fontSize.sm
						opacity: parent.enabled ? 1 : 0.5
					}

					Loader {
						id: expandIconLoader
						active: itemDelegate.modelData.hasChildren
						anchors {
							right: parent.right
							verticalCenter: parent.verticalCenter
						}
						sourceComponent: StyledText {
							text: ">"
						}
					}
				}
			}

			Loader {
				active: itemDelegate.modelData.isSeparator
				anchors {
					left: parent.left
					right: parent.right
				}
				sourceComponent: Rectangle {
					implicitHeight: 1
					color: Colors.colors.base3
				}
			}
		}
	}

	Loader {
		id: returnComponentLoader
		active: root.depth > 0

		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
		}

		sourceComponent: MouseArea {
			id: returnComponent

			implicitHeight: returnTextItem.height

			onClicked: {
				root.navigateBackwards();
			}

			StyledText {
				id: returnTextItem

				anchors {
					left: parent.left
					right: parent.right
					verticalCenter: parent.verticalCenter
				}
				padding: Config.appearance.padding.sm

				font.pointSize: Config.appearance.fontSize.sm
				text: "< return"
			}
		}
	}

	ListView {
		id: lview

		acceptedButtons: Qt.NoButton

		anchors {
			left: parent.left
			right: parent.right
			top: returnComponentLoader.bottom
			bottom: parent.bottom

			margins: root.padding
		}

		model: itemModel

		spacing: Config.appearance.spacing.xs
	}
}
