pragma ComponentBehavior: Bound

import qs.components
import qs.services
import org.nightshell.Components
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Layouts

RoundedClipRect {
	id: root

	property int activeTab: Tabs.T.Apps

	anchors {
		left: parent.left
	}

	function onTabPressed() {
		root.activeTab = (root.activeTab + 1) % 3;
	}
	function onBackTabPressed() {
		// I am very proud of myself for this simple equation
		// ((n - 1) + current) % n
		root.activeTab = (2 + root.activeTab) % 3;
	}

	implicitWidth: mainLayout.width
	implicitHeight: mainLayout.height
	radius: Config.appearance.rounding.sm

	enum T {
		Apps = 0,
		Windows = 1,
		Workspaces = 2
	}

	Component.onCompleted: {
		switch (GlobalShellState.desiredLauncherTab) {
		case "windows":
			root.activeTab = Tabs.T.Windows;
			break;
		default:
			root.activeTab = Tabs.T.Apps;
			break;
		}
		GlobalShellState.desiredLauncherTab = "";
	}

	Rectangle {
		anchors.fill: parent
		color: Qt.alpha(Colors.colors.neutral, 0.75)
	}

	RowLayout {
		id: mainLayout
		spacing: 1

		anchors {
			left: parent.left
			top: parent.top
		}

		TabItem {
			label: "Applications"
			index: 0
		}

		TabItem {
			label: "Windows"
			index: 1
		}

		TabItem {
			label: "Workspaces"
			index: 2
		}
	}

	component TabItem: MouseArea {
		id: tabItem

		cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

		required property string label
		required property int index

		readonly property bool isSelected: root.activeTab === index
		readonly property int hPadding: Config.appearance.padding.sm
		readonly property int vPadding: Config.appearance.padding.xs

		Layout.fillHeight: true

		implicitWidth: textMetrics.width + hPadding * 2
		implicitHeight: textMetrics.height + vPadding * 2

		Rectangle {
			anchors.fill: parent
			color: tabItem.isSelected ? Colors.colors.primary : Colors.colors.base2

			Behavior on color {
				CAnim {
					duration: 100
				}
			}
		}

		onClicked: {
			root.activeTab = tabItem.index;
		}

		StyledText {
			id: tabLabel

			anchors.fill: parent

			text: textMetrics.text

			font.pointSize: Config.appearance.fontSize.xs
			font.family: Config.appearance.fontFamily.sans
			font.weight: tabItem.isSelected ? 700 : 500
			color: tabItem.isSelected ? Colors.colors.primaryContent : Colors.colors.baseContent

			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter

			Behavior on color {
				CAnim {
					duration: 100
				}
			}
		}

		TextMetrics {
			id: textMetrics

			font.weight: 700
			font.pointSize: Config.appearance.fontSize.xs
			font.family: Config.appearance.fontFamily.sans

			text: tabItem.label
		}
	}
}
