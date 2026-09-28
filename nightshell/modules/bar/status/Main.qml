pragma ComponentBehavior: Bound

import org.nightshell.Configs
import QtQuick
import QtQuick.Layouts

Item {
	id: root

	property BaseStatus currentActive: null

	implicitWidth: mainLayout.width
	Layout.fillHeight: true

	function toggleActive(item: BaseStatus) {
		if (currentActive === item) {
			currentActive = null;
		} else {
			currentActive = item;
		}
	}

	function clearActive() {
		root.currentActive = null;
	}

	RowLayout {
		id: mainLayout

		anchors {
			top: parent.top
			topMargin: 2
			bottom: parent.bottom
			bottomMargin: 2
			horizontalCenter: parent.horizontalCenter
		}

		WrappedStatus {
			id: languageStatus
			sourceComponent: Language {}
		}

		WrappedStatus {
			id: networkStatus
			sourceComponent: Network {}
		}

		WrappedStatus {
			id: gamemodeStatus
			active: Config.modules.enabledModules.gamemode
			sourceComponent: Gamemode {}
		}

		WrappedStatus {
			id: audioStatus
			sourceComponent: Audio {}
		}

		WrappedStatus {
			id: notificationStatus
			active: Config.modules.enabledModules.notifications
			sourceComponent: Notifications {
				id: notifStatus
				onClicked: {
					root.toggleActive(notifStatus);
				}
			}
		}
	}

	component WrappedStatus: Loader {
		active: true
		asynchronous: true
		Layout.fillHeight: true
	}
}
