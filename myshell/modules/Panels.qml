pragma ComponentBehavior: Bound

import qs.modules.session as Session
import qs.modules.mprisviewer as MprisViewer
import qs.modules.bar.popouts as BarPopouts
import qs.modules.startmenu as StartMenuPopout
import qs.modules.notifications as NotifWrapper
import qs.modules.commandcapture
import MyShellPlugin.Configs
import Quickshell
import QtQuick

Item {
	id: root

	required property ShellScreen screen
	required property PersistentProperties openPanels
	required property Item bar

	readonly property alias session: session
	readonly property alias popouts: popouts
	readonly property alias startmenu: startmenu
	readonly property alias mprisViewer: mprisViewer

	readonly property list<string> validPanelProps: ["session", "startmenu", "mprisViewer"]

	anchors.fill: parent
	anchors.margins: Config.border.thickness
	anchors.topMargin: bar.implicitHeight

	function forceClosePanels(): void {
		for (const item of root.validPanelProps) {
			if (openPanels.hasOwnProperty(item)) {
				openPanels[item] = false;
			}
		}
	}

	function openExclusivePanel(prop: string): void {
		if (root.validPanelProps.includes(prop)) {
			for (const item of root.validPanelProps) {
				if (item === prop)
					openPanels[item] = !openPanels[item];
				else
					openPanels[item] = false;
			}
		}
	}

	function openHoverExclusivePanel(prop: string) {
		if (root.openPanels.session || root.openPanels.startmenu)
			return;
		return openExclusivePanel(prop);
	}

	CommandCaptureWrapper {
		id: commandcapture
		openPanels: root.openPanels
		panels: root

		anchors.right: parent.right
		anchors.bottom: parent.bottom
		// anchors.verticalCenter: parent.verticalCenter
		// anchors.top: parent.top
		// anchors.bottom: parent.bottom
	}

	MprisViewer.MprisWrapper {
		id: mprisViewer

		anchors.top: parent.top

		// TODO: Clean up spaghetti below
		x: root.bar.mediaInfo ? (root.bar.mediaInfo.x + (root.bar.mediaInfo.item?.implicitWidth ?? 0) / 2 - (implicitWidth / 2) + padding * 2) : 0

		openPanels: root.openPanels
	}

	NotifWrapper.Wrapper {
		id: notificationWrapper

		screen: root.screen
		openPanels: root.openPanels

		anchors {
			top: parent.top
			right: parent.right
		}
	}

	StartMenuPopout.Wrapper {
		id: startmenu
		openPanels: root.openPanels
		panels: root
		screen: root.screen

		anchors.left: parent.left
		anchors.top: parent.top
		anchors.bottom: parent.bottom
	}

	Session.Wrapper {
		id: session
		openPanels: root.openPanels
		panels: root

		// anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.top
		anchors.right: parent.right
	}

	BarPopouts.PopoutWrapper {
		id: popouts

		screen: root.screen

		x: currentCenter - baseWidth / 2
		// y: 4
	}
}
