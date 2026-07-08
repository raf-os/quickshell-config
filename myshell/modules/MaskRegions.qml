pragma ComponentBehavior: Bound

import qs.modules.bar
import MyShellPlugin.Configs
import Quickshell
import QtQuick

Region {
	id: root

	required property BarWrapper bar
	required property Panels panels
	required property QsWindow win

	readonly property int borderThickness: Config.border.thickness

	x: borderThickness
	y: bar.implicitHeight
	width: win.width - Config.border.thickness * 2
	height: win.height - bar.implicitHeight - Config.border.thickness

	intersection: Intersection.Xor

	R {
		panel: root.panels.commandCapture
	}

	R {
		panel: root.panels.mprisViewer
	}

	R {
		panel: root.panels.notifications
	}

	R {
		panel: root.panels.notificationOverlay
	}

	R {
		panel: root.panels.startmenu
	}

	RR {
		region: root.panels.launcher.region
	}

	R {
		panel: root.panels.popouts
	}

	R {
		panel: root.panels.session
	}

	component RR: Region {
		required property Region region

		x: region.x + root.borderThickness
		y: region.y + root.bar.implicitHeight
		width: region.width
		height: region.height
		intersection: Intersection.Subtract
	}

	component R: Region {
		required property Item panel

		x: panel.x + root.borderThickness
		y: panel.y + root.bar.implicitHeight
		width: panel.width
		height: panel.height
		intersection: Intersection.Subtract
	}
}
