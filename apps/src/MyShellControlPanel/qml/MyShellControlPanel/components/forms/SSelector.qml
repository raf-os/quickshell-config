pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.components
import MyShellControlPanel.plugin
import MyShellControlPanel.plugin.validators
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects

FormInput {
	id: root

	property int textInset: Config.appearance.spacing.xs
	property Component delegate: ComboItemDelegate {}
	readonly property real windowHeight: Window.height

	property alias control: control

	property list<var> model

	Layout.fillWidth: true
	implicitHeight: layout.implicitHeight

	inputItem: selectorAdapter

	function modelFindIndex(val: string): int {
		return root.model.findIndex(v => v === value);
	}

	function modelGetName(item: var): string {
		return String(item);
	}

	QtObject {
		id: selectorAdapter

		property string value
		readonly property int selectedIndex: control.currentIndex

		function findNameByIndex(idx: int): string {
			const st = root.model[idx];
			if (st === undefined || st === null) {
				return "";
			} else {
				return root.modelGetName(st);
			}
		}

		onValueChanged: {
			const selId = root.modelFindIndex(value);
			if (selId != -1) { // found something
				control.currentIndex = selId;
			}
		}

		onSelectedIndexChanged: {
			selectorAdapter.value = findNameByIndex(selectedIndex);
		}
	}

	RowLayout {
		id: layout

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top

		spacing: Config.appearance.spacing.sm

		StyledText {
			id: label

			text: root.name

			font.family: Config.appearance.fontFamily.sans
			font.pointSize: Config.appearance.fontSize.sm
			font.weight: 600

			leftPadding: root.textInset
		}

		ComboBox {
			id: control

			readonly property int rounding: Config.appearance.rounding.sm

			Layout.fillWidth: true

			font.pointSize: Config.appearance.fontSize.sm
			font.family: Config.appearance.fontFamily.mono
			spacing: Config.appearance.spacing.sm

			selectTextByMouse: true
			editable: true
			activeFocusOnTab: true

			model: root.model

			delegate: root.delegate

			indicator: SMaterialIcon {
				id: controlIndicator
				readonly property int animDuration: 200
				readonly property int size: control.height
				text: "arrow_drop_down"

				x: control.width - width

				width: size
				height: size

				opacity: control.popup.visible ? 1 : 0.5
				scale: control.popup.visible ? 1.25 : 1
				font.pointSize: Config.appearance.fontSize.xxl

				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter

				Behavior on opacity {
					NAnim {
						duration: controlIndicator.animDuration
					}
				}

				Behavior on scale {
					NAnim {
						duration: controlIndicator.animDuration
					}
				}
			}

			popup: Popup {
				id: controlPopup
				readonly property int parentOffset: Config.appearance.spacing.xs
				readonly property int marginSum: topMargin + bottomMargin
				readonly property int desiredHeight: Math.min(root.windowHeight - marginSum, contentItem.implicitHeight + marginSum, 256)
				readonly property bool reversePosition: desiredHeight + parentOffset + control.mapToGlobal(0, 0).y > root.windowHeight
				readonly property int animDuration: 150

				y: reversePosition ? -parentOffset - desiredHeight : control.height + parentOffset

				implicitWidth: control.width
				implicitHeight: desiredHeight
				padding: 8

				topMargin: 8
				bottomMargin: 8

				modal: true

				contentItem: Item {
					id: controlPopupItemWrapper
					implicitHeight: popupLv.contentHeight

					ListView {
						id: popupLv

						readonly property bool isScrollBarActive: contentHeight > height
						readonly property int scrollBarWidth: 8

						acceptedButtons: Qt.NoButton
						boundsBehavior: Flickable.StopAtBounds

						clip: true
						anchors.fill: parent
						anchors.rightMargin: isScrollBarActive ? scrollBarWidth + controlPopup.rightPadding : 0
						model: control.popup.visible ? control.delegateModel : null
						currentIndex: control.highlightedIndex
						spacing: 0

						highlightFollowsCurrentItem: false
						highlight: Rectangle {
							id: highlightRect
							y: ListView.view?.currentItem ? ListView.view.currentItem.y : 0
							implicitWidth: ListView.view ? ListView.view.width : 0
							implicitHeight: ListView.view?.currentItem ? ListView.view.currentItem.implicitHeight : 0

							property bool enableAnim: false

							color: Colors.colors.primary
							radius: Config.appearance.rounding.sm

							Component.onCompleted: {
								Qt.callLater(() => highlightRect.enableAnim = true);
							}

							Behavior on y {
								enabled: highlightRect.enableAnim
								NAnim {
									duration: 200
									easing.bezierCurve: Config.appearance.animCurves.defaultEase
								}
							}
						}

						ScrollBar.vertical: ScrollBar {
							id: sbar
							parent: controlPopupItemWrapper
							policy: popupLv.isScrollBarActive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff

							anchors {
								top: parent.top
								right: parent.right
								bottom: parent.bottom
							}

							implicitWidth: popupLv.scrollBarWidth
							padding: 0

							contentItem: Rectangle {
								radius: width / 2
								color: Colors.colors.primary
							}

							background: Rectangle {
								radius: width / 2
								color: Colors.colors.base
							}
						}
					}
				}

				background: Item {
					id: popupBg
					readonly property int boxRadius: Config.appearance.rounding.sm

					RectangularShadow {
						anchors.fill: parent
						offset: Qt.vector2d(0, 4)
						radius: popupBg.boxRadius
						blur: 8
						color: Qt.rgba(0.0, 0.0, 0.0, 0.25)
					}
					Rectangle {
						anchors.fill: parent
						radius: popupBg.boxRadius

						color: Colors.colors.base2
					}
				}

				enter: Transition {
					ParallelAnimation {
						NAnim {
							property: "scale"
							from: 0.9
							to: 1
							duration: controlPopup.animDuration
						}
						NAnim {
							property: "opacity"
							from: 0
							to: 1
							duration: controlPopup.animDuration
							easing.bezierCurve: Config.appearance.animCurves.linear
						}
					}
				}

				exit: Transition {
					ParallelAnimation {
						NAnim {
							property: "scale"
							from: 1
							to: 0.9
							duration: controlPopup.animDuration
						}
						NAnim {
							property: "opacity"
							from: 1
							to: 0
							duration: controlPopup.animDuration
							easing.bezierCurve: Config.appearance.animCurves.linear
						}
					}
				}
			}

			background: Rectangle {
				id: controlBg
				anchors.fill: parent
				color: Colors.colors.base2

				radius: control.rounding
				border.width: 2
				border.color: control.activeFocus ? Colors.colors.primary : "transparent"

				states: [
					State {
						name: "invalid"
						when: root.isValid === false || control.currentIndex === -1
						PropertyChanges {
							controlBg.border.color: Colors.colors.destructive
						}
					},
					State {
						name: "active"
						when: control.activeFocus
						PropertyChanges {
							controlBg.border.color: Colors.colors.primary
						}
					},
					State {
						name: "dirty"
						when: root.isDirty
						PropertyChanges {
							controlBg.border.color: Colors.colors.base5
						}
					}
				]
			}

			contentItem: TextField {
				readonly property int tPad: Config.appearance.padding.sm
				text: control.editText
				font: control.font
				color: Colors.colors.baseContent

				verticalAlignment: TextInput.AlignVCenter
				selectByMouse: true

				background: null

				topPadding: tPad
				bottomPadding: tPad
				rightPadding: control.indicator ? control.indicator.width + control.spacing : Config.appearance.padding.sm
				leftPadding: tPad
			}
		}
	}

	component ComboItemDelegate: ItemDelegate {
		id: defaultDel

		required property var model
		required property int index

		implicitWidth: ListView.view ? ListView.view.width : control.width
		background: null

		contentItem: StyledText {
			text: defaultDel.model[control.textRole]
			font: control.font
			color: Colors.colors.baseContent
			elide: Text.ElideRight
			verticalAlignment: Text.AlignVCenter
		}
		highlighted: control.highlightedIndex === index
	}
}
