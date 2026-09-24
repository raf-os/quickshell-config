pragma ComponentBehavior: Bound

import qs.modules.bar

import org.nightshell.Utils
import Quickshell
import QtQuick

Region {
	id: root

	required property Bar bar
	required property QsWindow win

	intersection: Intersection.Xor

	x: bar.x
	y: bar.y + bar.implicitHeight
	width: win.width - x
	height: win.height - y

	Region {
		id: focusBlock

		intersection: Intersection.Subtract

		x: root.x
		y: root.y
		width: root.width
		height: FocusGrabberManager.active ? root.height : 0
	}

	component ContentRegion: Region {
		required property Item surface

		x: root.x + surface.x
		y: root.y + surface.y
		width: surface.width
		height: surface.height
		intersection: Intersection.Subtract
	}
}
