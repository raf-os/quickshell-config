pragma ComponentBehavior: Bound

import "items"
import "states"
import qs.components
import qs.services
import MyShellPlugin
import MyShellPlugin.Configs
import Quickshell
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ColumnLayout {
	id: root

	enum SState {
		AppList = 0,
		CommandList = 1
	}

	required property PersistentProperties openPanels
	required property ShellScreen screen
	readonly property int rounding: Config.appearance.rounding.md
	readonly property real padding: Config.appearance.padding.lg
	readonly property bool isActive: openPanels.startmenu
	readonly property bool hasQuery: isActive && cmdinput?.debouncedInput && cmdinput.debouncedInput.length > 0

	property string errorMessage: ""
	// property string mode: openPanels.desiredStartMenuTab === "" ? "apps" : openPanels.desiredStartMenuTab ?? "apps"
	property int mode: Content.SState.AppList
	// Modes: apps | command

	property list<QtObject> filteredList

	focus: true

	spacing: -padding

	Keys.onEscapePressed: {
		openPanels.startmenu = false;
	}

	Keys.onUpPressed: startMenuStack.moveBackwards()
	Keys.onDownPressed: startMenuStack.moveForwards()
	Keys.onTabPressed: startMenuStack.moveForwards()
	Keys.onBacktabPressed: startMenuStack.moveBackwards()

	StateTitle {
		currentState: root.mode
		padding: root.padding

		onRequestPanelClose: {
			root.closePanel();
		}
	}

	Component.onCompleted: {
		cmdinputtxt.forceActiveFocus();
		checkDesiredTab();
	}

	function closePanel() {
		openPanels.startmenu = false;
	}

	function checkDesiredTab() {
		const desiredTab = root.openPanels.desiredStartMenuTab;
		if (desiredTab === "command") {
			root.mode = Content.SState.CommandList;
			cmdinputtxt.text = Config.launcher.commandPrefix;
		}
		root.openPanels.desiredStartMenuTab = "";
	}

	function showStateMessage(message: string): void {
		root.errorMessage = message;
		stateMessage.displayMessage();
	}

	function dismissStateMessage(): void {
		stateMessage.clearMessage();
	}

	Item {
		id: cmdinput

		property string debouncedInput: ""

		Layout.fillWidth: true
		Layout.margins: root.padding

		implicitHeight: cmdinputtxt.implicitHeight

		clip: true

		Rectangle {
			id: cmdinputbg
			readonly property bool isActive: cmdinputtxt.activeFocus

			anchors.fill: parent
			radius: Config.appearance.rounding.sm

			color: ColorService.current.base0
			border.width: isActive ? 2 : 0
			border.color: isActive ? Colors.colors.primary : "transparent"
		}

		Item {
			id: modeIconWrapper

			anchors.left: parent.left
			anchors.top: parent.top
			anchors.bottom: parent.bottom

			implicitWidth: 24

			StyledText {
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right

				font.family: Config.appearance.fontFamily.monoIcon
				font.pixelSize: 24

				verticalAlignment: Text.AlignVCenter

				text: {
					switch (root.mode) {
					case Content.SState.AppList:
						return "󰍉";
					case Content.SState.CommandList:
						return "";
					default:
						return "";
					}
				}
			}
		}

		TextInput {
			id: cmdinputtxt

			focus: root.isActive

			font.pointSize: Config.appearance.fontSize.md
			font.family: Config.appearance.fontFamily.sans

			anchors.left: modeIconWrapper.right
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter
			padding: Config.appearance.spacing.sm

			color: ColorService.current.baseContent

			onTextChanged: {
				if (!root.isActive) {
					return;
				}
				if (text.startsWith(Config.launcher.commandPrefix)) {
					root.mode = Content.SState.CommandList;
				} else {
					root.mode = Content.SState.AppList;
					root.dismissStateMessage();
				}
			}
		}
	}

	Loader {
		id: stateMessage

		property bool isActive: false

		active: isActive
		visible: isActive

		Layout.fillWidth: true
		Layout.margins: root.padding

		signal clearMessage

		function displayMessage() {
			stateMessage.isActive = true;
			if (item) {
				item?.exitTimer?.restart();
			}
		}

		sourceComponent: Item {
			id: errorWrapper

			readonly property real initialHeight: errorText.implicitHeight ?? 24

			implicitWidth: parent.width - root.padding * 2
			implicitHeight: initialHeight

			clip: true
			scale: 0.8
			opacity: 0

			Component.onCompleted: {
				entryAnim.start();
				exitTimer.start();
			}

			Connections {
				target: stateMessage

				function onClearMessage() {
					if (entryAnim.running)
						entryAnim.stop();
					exitAnim.start();
				}
			}

			Timer {
				id: exitTimer
				interval: 3000

				onTriggered: {
					if (entryAnim.running)
						entryAnim.stop();
					exitAnim.start();
				}
			}

			Behavior on implicitHeight {
				NAnim {
					duration: 300
				}
			}

			ParallelAnimation {
				id: entryAnim

				NAnim {
					target: errorWrapper
					property: "opacity"
					to: 1
					easing.bezierCurve: Config.appearance.animCurves.easeInOut
					duration: 400
				}

				NAnim {
					target: errorWrapper
					property: "scale"
					to: 1
					easing.bezierCurve: Config.appearance.animCurves.accelerateOverCorrect
					duration: 500
				}
			}

			SequentialAnimation {
				id: exitAnim

				NAnim {
					target: errorWrapper
					property: "opacity"
					to: 0
					duration: 300
					easing.bezierCurve: Config.appearance.animCurves.linear
				}
				ScriptAction {
					script: stateMessage.isActive = false
				}
			}

			StyledRect {
				color: ColorService.current.destructive

				anchors.fill: parent

				radius: Config.appearance.rounding.sm
			}

			StyledText {
				id: errorText

				anchors.fill: parent

				width: parent.width
				text: root.errorMessage
				color: ColorService.current.baseContent

				topPadding: 6
				bottomPadding: 6
				leftPadding: 10
				rightPadding: 10
				font.pointSize: Config.appearance.fontSize.sm

				maximumLineCount: 6
				wrapMode: Text.Wrap
			}
		}
	}

	StackView {
		id: startMenuStack

		property bool isCompleted

		implicitWidth: Config.launcher.width

		Layout.fillHeight: true
		Layout.margins: root.padding

		Component.onCompleted: {
			navigateComponent();
			isCompleted = true;
		}

		Connections {
			target: root

			function onModeChanged() {
				startMenuStack.navigateComponent();
			}
		}

		function navigateComponent() {
			const newComp = mapStateToComponent(root.mode);
			startMenuStack.replaceCurrentItem(newComp);
		}

		function mapStateToComponent(state: int): Component {
			switch (state) {
			case Content.SState.AppList:
				return appList;
			case Content.SState.CommandList:
				return commandList;
			default:
				return appList;
			}
		}

		function moveForwards() {
			const item = currentItem as StateWrapper;
			item.moveForwards();
		}
		function moveBackwards() {
			const item = currentItem as StateWrapper;
			item.moveBackwards();
		}

		replaceEnter: Transition {
			enabled: startMenuStack.isCompleted

			ParallelAnimation {
				NAnim {
					property: "opacity"
					from: 0
					to: 1
					duration: 200
				}
			}
		}
		replaceExit: Transition {
			enabled: startMenuStack.isCompleted

			ParallelAnimation {
				NAnim {
					property: "opacity"
					from: 1
					to: 0
					duration: 200
				}
			}
		}
	}

	Component {
		id: appList

		AppList {
			id: lview

			openPanels: root.openPanels
			textInput: cmdinputtxt
			onSendStateMessage: message => root.showStateMessage(message)

			delegate: AppItem {
				openPanels: root.openPanels
			}
		}
	}

	Component {
		id: commandList

		CommandList {
			id: cview

			screen: root.screen
			openPanels: root.openPanels
			textInput: cmdinputtxt
			onSendStateMessage: message => root.showStateMessage(message)
		}
	}
}
