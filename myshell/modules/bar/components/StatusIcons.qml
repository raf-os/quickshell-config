pragma ComponentBehavior: Bound

import "statusIcons"
import qs.components
import qs.utils
import qs.modules.bar
import qs.modules.bar.popouts
import qs.services.network
import MyShellPlugin
import MyShellPlugin.Configs
import QtQuick
import QtQuick.Layouts

Item {
	id: root

	required property PopoutHandler popoutHandler
	required property PopoutWrapper popoutWrapper
	required property OpenPanels openPanels

	readonly property string currentPopoutName: popoutWrapper.hasCurrent ? root.popoutWrapper.currentName : ""

	implicitWidth: iconRow.implicitWidth + Config.appearance.padding.xl
	implicitHeight: Config.bar.sizes.innerHeight

	StyledRect {
		id: bgRect

		anchors.fill: parent

		color: Colors.colors.base0
		radius: Config.appearance.rounding.sm
	}

	RowLayout {
		id: iconRow

		anchors.top: parent.top
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter

		spacing: Config.appearance.spacing.xs

		WrappedLoader {
			id: kbdLayoutStatus
			name: "Keyboard layout"
			active: true //Config.keymap.enabled === true

			sourceComponent: KeyboardLayoutIcon {
				popoutHandler: root.popoutHandler
				currentPopoutName: root.currentPopoutName
			}
		}

		WrappedLoader {
			id: gameModeStatus
			name: "Game mode"
			active: true

			sourceComponent: GameModeIcon {}
		}

		WrappedLoader {
			id: notifStatus
			name: "Notifications"
			active: Config.notification.enabled

			sourceComponent: NotificationsIcon {
				onTriggerNotifications: {
					root.openPanels.toggleNotifications();
				}
			}
		}

		WrappedLoader {
			id: audioStatus
			name: "Audio settings"
			active: true

			sourceComponent: AudioStatusIcon {
				popoutHandler: root.popoutHandler
			}
		}

		WrappedLoader {
			id: networkStatus
			name: "Network settings"
			active: true

			sourceComponent: NetworkStatusIcon {}
		}
	}

	component WrappedLoader: Loader {
		required property string name

		Layout.alignment: Qt.AlignVCenter
		Layout.fillHeight: true
		visible: active
	}
}
