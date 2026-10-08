import qs.modules as MODULES
import qs.modules.bar as MODULES_BAR
import qs.modules.osd as MODULES_OSD

import org.nightshell.Hyprland as N_HYPRLAND

import Quickshell as QS
import QtQuick

QtObject {
	id: root

	property MODULES_BAR.Bar bar
	property MODULES.Content content
	property QS.ShellScreen shellScreen
	property QS.QsWindow win
	property N_HYPRLAND.HyprMonitor hyprMonitor

	readonly property bool isLauncherActive: content?.launcherWrapper?.isActive ?? false

	signal requestToggleLauncher
	signal requestToggleDashboard
}
