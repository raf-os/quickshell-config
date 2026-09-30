import org.nightshell.Configs

import QtQuick

NumberAnimation {
	id: root

	duration: 500
	easing.type: Easing.BezierSpline
	easing.bezierCurve: Styles.anim_defaultEase
}
