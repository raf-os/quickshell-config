pragma ComponentBehavior: Bound

import qs.components
import qs.services
import org.nightshell.DesktopEntries
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Controls
import QtQuick.Effects

import "apps"
import "windows"

MouseArea {
	id: root

	required property real boundsWidth
	required property real boundsHeight
	readonly property int borderPadding: 200
	readonly property int animationDuration: 150
	readonly property int spacing: Config.appearance.spacing.xs

	property bool isAltPressed: false

	implicitWidth: boundsWidth - borderPadding * 2
	implicitHeight: boundsHeight - borderPadding * 2

	focus: true
	scale: 0.9
	opacity: 0

	Component.onCompleted: {
		queryInput.forceActiveFocus();
		enterAnim.start();

		const desiredTab = GlobalShellState.desiredLauncherTab;
	}

	DesktopEntriesModel {
		id: entriesModel
		queryString: queryInput.text
	}

	onClicked: {
		queryInput.forceActiveFocus();
	}

	function mapTabToComponent(tab: int): Component {
		switch (tab) {
		case Tabs.T.Apps:
			return appsListComponent;
		case Tabs.T.Windows:
			return windowsListComponent;
		default:
			return null;
		}
	}

	function triggerEnterAnim() {
		exitAnim.stop();
		enterAnim.start();
	}

	function triggerExitAnim() {
		enterAnim.stop();
		exitAnim.start();
	}

	function checkReEnter() {
		if (!exitAnim.running)
			return;

		triggerEnterAnim();
	}

	function onCloseSignal() {
		triggerExitAnim();
	}

	signal exitAnimationFinished
	signal closeLauncherRequested
	signal jumpToListStart
	signal jumpToListEnd
	signal nextItemRequested
	signal previousItemRequested
	signal currentItemActivated
	signal hotkeyIndexActivated(key: int)

	ParallelAnimation {
		id: enterAnim
		NAnim {
			target: root
			property: "scale"
			to: 1
			duration: root.animationDuration
		}
		NumberAnimation {
			target: root
			property: "opacity"
			to: 1
			duration: root.animationDuration
		}
	}

	SequentialAnimation {
		id: exitAnim
		ParallelAnimation {
			NAnim {
				target: root
				property: "scale"
				to: 0.9
				duration: root.animationDuration
			}
			NAnim {
				target: root
				property: "opacity"
				to: 0
				duration: root.animationDuration
			}
		}
		ScriptAction {
			script: root.exitAnimationFinished()
		}
	}

	Keys.onPressed: ev => {
		if (ev.key === Qt.Key_Alt) {
			root.isAltPressed = true;
			ev.accepted = true;
			return;
		}

		if (ev.modifiers & Qt.AltModifier) {
			if (ev.key >= Qt.Key_1 && ev.key <= Qt.Key_9) {
				ev.accepted = true;
				root.hotkeyIndexActivated(ev.key);
				return;
			} else if (ev.key === Qt.Key_Tab) {
				ev.accepted = true;
				launcherTabs.onTabPressed();
				return;
			} else if (ev.key === Qt.Key_Backtab) {
				ev.accepted = true;
				launcherTabs.onBackTabPressed();
				return;
			}
		}

		if (ev.key === Qt.Key_Tab || ev.key === Qt.Key_Down) {
			ev.accepted = true;
			root.nextItemRequested();
			return;
		}

		if (ev.key === Qt.Key_Backtab || ev.key === Qt.Key_Up) {
			ev.accepted = true;
			root.previousItemRequested();
			return;
		}

		if (ev.key === Qt.Key_Home) {
			ev.accepted = true;
			root.jumpToListStart();
			return;
		}

		if (ev.key === Qt.Key_End) {
			ev.accepted = true;
			root.jumpToListEnd();
			return;
		}
	}

	Keys.onReleased: ev => {
		if (ev.key === Qt.Key_Alt) {
			root.isAltPressed = false;
		}
	}

	Keys.onReturnPressed: ev => {
		root.currentItemActivated();
	}

	Keys.onEscapePressed: ev => {
		ev.accepted = true;
		root.closeLauncherRequested();
	}

	RectangularShadow {
		anchors.fill: parent

		color: "black"
		radius: bgRect.radius
		spread: 2.0
		offset: Qt.vector2d(0.0, 2.0)
	}

	Rectangle {
		id: bgRect

		anchors.fill: parent

		color: Colors.colors.base
		radius: Config.appearance.rounding.sm

		border.width: root.activeFocus ? 2 : 0
		border.color: root.activeFocus ? Colors.colors.primary : "transparent"
	}

	Item {
		id: content

		anchors.fill: parent
		anchors.margins: Config.appearance.padding.sm

		Tabs {
			id: launcherTabs
			anchors.top: parent.top
		}

		StackView {
			id: contentStack
			initialItem: root.mapTabToComponent(launcherTabs.activeTab)
			clip: true

			anchors {
				top: launcherTabs.bottom
				topMargin: root.spacing
				left: parent.left
				right: parent.right
				bottom: inputWrapper.top
				bottomMargin: root.spacing
			}

			function switchTabs(newTab: int) {
				const newComponent = root.mapTabToComponent(newTab);
				if (newComponent) {
					replaceCurrentItem(newComponent, {});
				} else {
					if (!empty)
						popCurrentItem(StackView.ReplaceTransition);
				}
			}

			Connections {
				target: launcherTabs

				function onActiveTabChanged() {
					contentStack.switchTabs(launcherTabs.activeTab);
				}
			}

			replaceEnter: Transition {
				NAnim {
					property: "opacity"
					from: 0
					to: 1
					duration: 200
				}
			}

			replaceExit: Transition {
				NAnim {
					property: "opacity"
					to: 0
					duration: 200
				}
			}
		}

		Item {
			id: inputWrapper

			readonly property int textPadding: Config.appearance.padding.sm

			anchors {
				left: parent.left
				right: parent.right
				bottom: parent.bottom
			}

			implicitHeight: queryInput.implicitHeight

			Rectangle {
				id: bgInput
				anchors.fill: parent
				color: Colors.colors.base0
				radius: Config.appearance.rounding.sm

				border.width: queryInput.activeFocus ? 2 : 0
				border.color: Colors.colors.primary
			}

			MaterialIcon {
				id: searchIcon

				anchors {
					left: parent.left
					top: parent.top
					bottom: parent.bottom
				}

				font.pixelSize: parent.implicitHeight * 0.75
				text: "search"

				leftPadding: 4
				rightPadding: 4

				verticalAlignment: Text.AlignVCenter
			}

			Text {
				id: placeholderText

				anchors.fill: parent
				font: queryInput.font
				padding: inputWrapper.textPadding
				leftPadding: searchIcon.width + Config.appearance.spacing.xxs

				text: "Search apps..."
				color: Qt.alpha(Colors.colors.baseContent, 0.5)

				opacity: queryInput.text === "" ? 1 : 0

				Behavior on opacity {
					NumberAnimation {
						duration: 150
					}
				}
			}

			TextInput {
				id: queryInput

				focus: true
				anchors.fill: parent
				padding: inputWrapper.textPadding
				leftPadding: searchIcon.width + Config.appearance.spacing.xxs

				font.family: Config.appearance.fontFamily.mono
				font.pointSize: Config.appearance.fontSize.sm

				color: Colors.colors.baseContent

				Keys.forwardTo: [root]
			}
		}
	}

	Component {
		id: appsListComponent

		AppsList {
			id: appList

			content: root
			tabIdx: Tabs.T.Apps
			queryString: queryInput.text
			highlightHotkeys: root.isAltPressed
			spacing: root.spacing
			model: entriesModel

			onRequestClose: {
				root.closeLauncherRequested();
			}
		}
	}

	Component {
		id: windowsListComponent

		WindowsList {
			id: windowsList

			content: root
			tabIdx: Tabs.T.Windows
			queryString: queryInput.text
			highlightHotkeys: root.isAltPressed
			spacing: root.spacing

			onRequestClose: {
				root.closeLauncherRequested();
			}
		}
	}
}
