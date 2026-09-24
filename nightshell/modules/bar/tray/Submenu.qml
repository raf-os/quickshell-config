pragma ComponentBehavior: Bound

import qs.components
import org.nightshell.Configs
import org.nightshell.DBusMenu
import org.nightshell.SystemTray
import QtQuick

Item {
	id: root

	required property StatusNotifierItem statusItem
	required property var rootIndex
	required property int depth
	readonly property int padding: Styles.padding_sm

	signal requestClose
	signal navigateBack
	signal navigateToIndex(index: var, statusItem: StatusNotifierItem)

	implicitHeight: Math.min(listView.contentHeight + returnComponentLoader.height + root.padding * 2, 1024)
	focus: true

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
			if (count === 0)
				root.navigateBack();
		}

		delegate: Item {
			id: itemDelegate
			required property int index
			required property var selfIndex
			required property DBusMenuModelItem modelData

			readonly property int spacing: Styles.spacing_sm

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
					id: menuItem

					enabled: itemDelegate.modelData.isEnabled
					cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
					hoverEnabled: true

					implicitHeight: labelText.height > 0 ? (labelText.height + Styles.padding_sm * 2) : 0

					onClicked: {
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
							leftMargin: Styles.padding_sm
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
							rightMargin: expandIconLoader.active ? itemDelegate.spacing : 0
							verticalCenter: parent.verticalCenter
						}

						text: itemDelegate.modelData.text
						elide: Text.ElideRight

						font.pointSize: Styles.text_sm
						opacity: parent.enabled ? 1 : 0.5
					}

					Loader {
						id: expandIconLoader
						active: itemDelegate.modelData.hasChildren
						anchors {
							right: parent.right
							rightMargin: Styles.padding_sm
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
				sourceComponent: Item {
					implicitHeight: 13

					Rectangle {
						anchors {
							left: parent.left
							right: parent.right
							verticalCenter: parent.verticalCenter
						}
						implicitHeight: 1
						color: Colors.primary
					}
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
				root.navigateBack();
			}

			StyledText {
				id: returnTextItem

				anchors {
					left: parent.left
					right: parent.right
					verticalCenter: parent.verticalCenter
				}
				padding: Styles.padding_sm

				font.pointSize: Styles.text_sm
				text: "< return"
			}
		}
	}

	ListView {
		id: listView
		acceptedButtons: Qt.NoButton
		anchors {
			top: returnComponentLoader.bottom
			left: parent.left
			right: parent.right
			bottom: parent.bottom
		}
		focus: true
		model: itemModel
		spacing: 0
	}
}
